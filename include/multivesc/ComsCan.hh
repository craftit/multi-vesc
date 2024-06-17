//
// Created by charles on 17/06/24.
//

#ifndef MUTLIVESC_COMSCAN_HH
#define MUTLIVESC_COMSCAN_HH

#include <string>
#include <cstdint>
#include <thread>
#include <atomic>
#include <linux/can.h>
#include "multivesc/ComsInterface.hh"

namespace multivesc {

    // This file is based on examples given in: https://github.com/vedderb/bldc/blob/master/documentation/comm_can.md

    typedef enum {
        CAN_PACKET_SET_DUTY = 0,
        CAN_PACKET_SET_CURRENT,
        CAN_PACKET_SET_CURRENT_BRAKE,
        CAN_PACKET_SET_RPM,
        CAN_PACKET_SET_POS,
        CAN_PACKET_STATUS = 9,
        CAN_PACKET_SET_CURRENT_REL = 10,
        CAN_PACKET_SET_CURRENT_BRAKE_REL,
        CAN_PACKET_SET_CURRENT_HANDBRAKE,
        CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
        CAN_PACKET_STATUS_2 = 14,
        CAN_PACKET_STATUS_3 = 15,
        CAN_PACKET_STATUS_4 = 16,
        CAN_PACKET_STATUS_5 = 27,
        CAN_PACKET_STATUS_6 = 28
    } CAN_PACKET_ID;


    //! Low level can coms class for the VESC motor controller.
    //! This class is designed to be stateless and only contains the necessary functions to send commands to the motor controller.

    class ComsCan
      : public ComsInterface
    {
    public:
        explicit ComsCan(const std::string &deviceName);

        //! Destructor
        virtual ~ComsCan();

        //! Open the CAN interface.
        bool open() override;

        //! Stop the CAN interface and close the socket.
        bool stop() override;

        //! Set the duty cycle of the motor controller. The duty cycle is a value between -1 and 1.
        void set_duty(uint8_t controller_id, float duty);

        //! Set motor current in Amps.
        void set_current(uint8_t controller_id, float current);

        //! Set motor current in Amps with an off delay. The off delay is useful to keep the current controller running for a while even after setting currents below the minimum current.
        void set_current_off_delay(uint8_t controller_id, float current, float off_delay);

        //! Set brake current in Amps.
        void set_current_brake(uint8_t controller_id, float current);

        //! Set the RPM of the motor controller.
        void set_rpm(uint8_t controller_id, float rpm);

        //! Set the position of the motor controller. The position is in turns.
        void set_pos(uint8_t controller_id, float pos);

        //! Set the current of the motor controller as a percentage of the maximum current.
        void set_current_rel(uint8_t controller_id, float current_rel);

        /**
         * Same as above, but also sets the off delay. Note that this command uses 6 bytes now. The off delay is useful to set to keep the current controller running for a while even after setting currents below the minimum current.
         */
        void set_current_rel_off_delay(uint8_t controller_id, float current_rel, float off_delay);

        void set_current_brake_rel(uint8_t controller_id, float current_rel);

        void set_handbrake(uint8_t controller_id, float current);

        void set_handbrake_rel(uint8_t controller_id, float current_rel);

    private:
        void can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len);

        //! Read packets from the CAN interface and call the appropriate callback functions.
        void run_receive_thread();

        //! Wait for data on the socket
        //! @return True if data is available, false if timeout.
        bool wait_for_data(float timeoutSeconds);

        //! Decode a CAN frame and call the appropriate callback functions.
        void decode(const struct can_frame &frame);

        //! Callback function for status packets.
        void status_callback(uint8_t controllerId, float erpm, float current, float dutyCycle);

        //! Callback function for status 2 packets.
        void status_2_callback(uint8_t controllerId, float ampHours, float ampHoursCharged);

        //! Callback function for status 3 packets.
        void status_3_callback(uint8_t controllerId, float wattHours, float wattHoursCharged);

        //! Callback function for status 4 packets.
        void status_4_callback(uint8_t controllerId, float tempFet, float tempMotor, float currentIn, float PIDPos);

        //! Callback function for status 5 packets.
        void status_5_callback(uint8_t controllerId, float tachometer, float vIn);

        //! Callback function for status 6 packets.
        void status_6_callback(uint8_t controllerId, float adc1, float adc2, float adc3, float ppm);


        std::string mDeviceName;
        int mSocket = -1;
        std::atomic_bool mTerminate = false;
        std::thread mReceiveThread;
        bool mVerbose = false;
    };

} // multivesc

#endif //MUTLIVESC_COMSCAN_HH
