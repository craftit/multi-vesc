//
// Created by charles on 17/06/24.
//

#include "multivesc/ComsCan.hh"

#include <unistd.h>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can/raw.h>


namespace multivesc
{
    namespace {
        void buffer_append_int16(uint8_t *buffer, int16_t number, int32_t *index) {
            buffer[(*index)++] = number >> 8;
            buffer[(*index)++] = number;
        }

        void buffer_append_int32(uint8_t *buffer, int32_t number, int32_t *index) {
            buffer[(*index)++] = number >> 24;
            buffer[(*index)++] = number >> 16;
            buffer[(*index)++] = number >> 8;
            buffer[(*index)++] = number;
        }

        void buffer_append_float16(uint8_t *buffer, float number, float scale, int32_t *index) {
            buffer_append_int16(buffer, (int16_t) (number * scale), index);
        }

        void buffer_append_float32(uint8_t *buffer, float number, float scale, int32_t *index) {
            buffer_append_int32(buffer, (int32_t) (number * scale), index);
        }


        float get_scaled_float16(const uint8_t *data, int32_t index, float scale) {
            return (float) ((int16_t) (data[index] << 8 | data[index + 1])) / scale;
        }

        float get_scaled_float32(const uint8_t *data, int32_t index, float scale) {
            return (float) ((int32_t) (data[index] << 24 | data[index + 1] << 16 | data[index + 2] << 8 | data[index + 3])) / scale;
        }
    }


    ComsCan::ComsCan(const std::string &deviceName)
     : mDeviceName(deviceName)
    {

    }

    //! Destructor
    ComsCan::~ComsCan()
    {
        stop();
    }

    //! Open the CAN interface.

    bool ComsCan::open()
    {
        // Check it is not already open
        if(mSocket >= 0) {
            return false;
        }
        mSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if(mSocket < 0) {
            return false;
        }

        struct ifreq ifr {};
        strcpy(ifr.ifr_name, mDeviceName.c_str());
        ioctl(mSocket, SIOCGIFINDEX, &ifr);

        struct sockaddr_can addr {};
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if(bind(mSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(mSocket);
            mSocket = -1;
            return false;
        }

        mReceiveThread = std::thread(&ComsCan::run_receive_thread, this);

        return true;
    }

    //! Stop the CAN interface and close the socket.
    bool ComsCan::stop()
    {
        mTerminate = true;
        if(mReceiveThread.joinable())
            mReceiveThread.join();

        if(mSocket >= 0) {
            close(mSocket);
            mSocket = -1;
        }
        return true;
    }


    // Implementation for sending extended ID CAN-frames

    void ComsCan::can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len)
    {
        struct can_frame frame;
        frame.can_id = id | CAN_EFF_FLAG;
        frame.can_dlc = len;
        memcpy(frame.data, data, len);
        write(mSocket, &frame, sizeof(struct can_frame));
    }


    void ComsCan::set_duty(uint8_t controller_id, float duty)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_int32(buffer, (int32_t) (duty * 100000.0), &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_DUTY << 8), buffer, send_index);
    }

    void ComsCan::set_current(uint8_t controller_id, float current)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_int32(buffer, (int32_t) (current * 1000.0), &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
    }

    void ComsCan::set_current_off_delay(uint8_t controller_id, float current, float off_delay)
    {
        int32_t send_index = 0;
        uint8_t buffer[6];
        buffer_append_int32(buffer, (int32_t) (current * 1000.0), &send_index);
        buffer_append_float16(buffer, off_delay, 1e3, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
    }

    void ComsCan::set_current_brake(uint8_t controller_id, float current) {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_int32(buffer, (int32_t) (current * 1000.0), &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT_BRAKE << 8), buffer, send_index);
    }

    void ComsCan::set_rpm(uint8_t controller_id, float rpm)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_int32(buffer, (int32_t) rpm, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_RPM << 8), buffer, send_index);
    }

    void ComsCan::set_pos(uint8_t controller_id, float pos)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_int32(buffer, (int32_t) (pos * 1000000.0), &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_POS << 8), buffer, send_index);
    }

    void ComsCan::set_current_rel(uint8_t controller_id, float current_rel)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_float32(buffer, current_rel, 1e5, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT_REL << 8), buffer, send_index);
    }

    /**
     * Same as above, but also sets the off delay. Note that this command uses 6 bytes now. The off delay is useful to set to keep the current controller running for a while even after setting currents below the minimum current.
     */
    void ComsCan::set_current_rel_off_delay(uint8_t controller_id, float current_rel, float off_delay)
    {
        int32_t send_index = 0;
        uint8_t buffer[6];
        buffer_append_float32(buffer, current_rel, 1e5, &send_index);
        buffer_append_float16(buffer, off_delay, 1e3, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT_REL << 8), buffer, send_index);
    }

    void ComsCan::set_current_brake_rel(uint8_t controller_id, float current_rel)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_float32(buffer, current_rel, 1e5, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT_BRAKE_REL << 8), buffer, send_index);
    }

    void ComsCan::set_handbrake(uint8_t controller_id, float current)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_float32(buffer, current, 1e3, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT_HANDBRAKE << 8), buffer, send_index);
    }

    void ComsCan::set_handbrake_rel(uint8_t controller_id, float current_rel)
    {
        int32_t send_index = 0;
        uint8_t buffer[4];
        buffer_append_float32(buffer, current_rel, 1e5, &send_index);
        can_transmit_eid(controller_id |
                         ((uint32_t) CAN_PACKET_SET_CURRENT_HANDBRAKE_REL << 8), buffer, send_index);
    }


    //! Wait for data on the socket
    //! @return True if data is available, false if timeout.
    bool ComsCan::wait_for_data(float timeoutSeconds)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(mSocket, &readfds);

        struct timeval timeout;
        timeout.tv_sec = (int) timeoutSeconds;
        timeout.tv_usec = (int) ((timeoutSeconds - (int) timeoutSeconds) * 1e6);

        return select(mSocket + 1, &readfds, NULL, NULL, &timeout) > 0;
    }


    //! Read packets from the CAN interface and call the appropriate callback functions.
    void ComsCan::run_receive_thread()
    {
        struct can_frame frame;

        while(!mTerminate)
        {
            // Wait for data on the socket
            if(!wait_for_data(0.5))
                continue;
            auto nbytes = read(mSocket, &frame, sizeof(struct can_frame));
            if (nbytes < 0) {
                perror("Read");
                break;
            }

            if(mVerbose) {
                printf("0x%03X [%d] ", frame.can_id, frame.can_dlc);
                for (int i = 0; i < frame.can_dlc; i++)
                    printf("%02X ", frame.data[i]);
                printf("\n");
            }

            decode(frame);
        }
    }

    void ComsCan::decode(const can_frame &frame)
    {
        uint8_t controllerId = frame.can_id & 0xFF;
        uint16_t packetType = (frame.can_id >> 8) & 0xFFFF;

        switch(CAN_PACKET_ID(packetType)) {
            case CAN_PACKET_SET_DUTY:
            case CAN_PACKET_SET_CURRENT:
            case CAN_PACKET_SET_CURRENT_BRAKE:
            case CAN_PACKET_SET_RPM:
            case CAN_PACKET_SET_POS:
            case CAN_PACKET_SET_CURRENT_REL:
            case CAN_PACKET_SET_CURRENT_BRAKE_REL:
            case CAN_PACKET_SET_CURRENT_HANDBRAKE:
            case CAN_PACKET_SET_CURRENT_HANDBRAKE_REL:
                // Do nothing for now, these came from something else sending commands on the bus.
                break;
            case CAN_PACKET_STATUS:
            {
                auto erpm = get_scaled_float32(frame.data, 0, 1);
                auto current = get_scaled_float16(frame.data, 4, 10);
                auto duty = get_scaled_float16(frame.data, 6, 1000);
                status_callback(controllerId, erpm, current, duty);
            }
                break;
            case CAN_PACKET_STATUS_2:
            {
                auto ampHours = get_scaled_float32(frame.data, 0, 10000);
                auto ampHoursCharged = get_scaled_float32(frame.data, 4, 10000);
                status_2_callback(controllerId, ampHours, ampHoursCharged);
            } break;
            case CAN_PACKET_STATUS_3:
            {
                auto wattHours = get_scaled_float32(frame.data, 0, 10000);
                auto wattHoursCharged = get_scaled_float32(frame.data, 4, 10000);
                status_3_callback(controllerId, wattHours, wattHoursCharged);
            } break;
            case CAN_PACKET_STATUS_4:
            {
                auto tempFet = get_scaled_float16(frame.data, 0, 10);
                auto tempMotor = get_scaled_float16(frame.data, 2, 10);
                auto currentIn = get_scaled_float16(frame.data, 4, 10);
                auto PIDPos = get_scaled_float16(frame.data, 6, 50);
                status_4_callback(controllerId, tempFet, tempMotor, currentIn, PIDPos);
            } break;
            case CAN_PACKET_STATUS_5:
            {
                auto tachometer = get_scaled_float32(frame.data, 0, 6);
                auto vIn = get_scaled_float16(frame.data, 4, 10);
                status_5_callback(controllerId, tachometer, vIn);
            } break;
            case CAN_PACKET_STATUS_6:
            {
                auto adc1 = get_scaled_float16(frame.data, 0, 1000);
                auto adc2 = get_scaled_float16(frame.data, 2, 1000);
                auto adc3 = get_scaled_float16(frame.data, 4, 1000);
                auto ppm = get_scaled_float16(frame.data, 6, 1000);
                status_6_callback(controllerId, adc1, adc2, adc3, ppm);

            } break;
        }
    }

    void ComsCan::status_callback(uint8_t controllerId, float erpm, float current, float dutyCycle)
    {

    }

    void ComsCan::status_2_callback(uint8_t controllerId, float ampHours, float ampHoursCharged)
    {

    }

    void ComsCan::status_3_callback(uint8_t controllerId, float wattHours, float wattHoursCharged)
    {

    }

    void
    ComsCan::status_4_callback(uint8_t controllerId, float tempFet, float tempMotor, float currentIn, float PIDPos)
    {

    }

    void ComsCan::status_5_callback(uint8_t controllerId, float tachometer, float vIn)
    {

    }

    void ComsCan::status_6_callback(uint8_t controllerId, float adc1, float adc2, float adc3, float ppm)
    {

    }


} // multivesc