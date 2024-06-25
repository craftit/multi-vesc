//
// Created by charles on 17/06/24.
//

#ifndef MULTIVESC_COMSINTERFACE_HH
#define MULTIVESC_COMSINTERFACE_HH

#include <vector>
#include <cstdint>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>

namespace multivesc
{
    using json = nlohmann::json;

    class Motor;
    class Manager;

    //! Abstract class for communicating with the VESC
    //! The implementation deal with can bus and serial communication
    //!
    class ComsInterface
    {
    public:
        //! Constructor
        ComsInterface();

        //! Constructor
        explicit ComsInterface(const json &config);

        //! Disable copy and move constructors
        ComsInterface(const ComsInterface&) = delete;
        ComsInterface& operator=(const ComsInterface&) = delete;
        ComsInterface(ComsInterface&&) = delete;
        ComsInterface& operator=(ComsInterface&&) = delete;

        virtual ~ComsInterface() = default;

        //! Start the coms interface
        virtual bool open();

        //! Stop the interface
        virtual bool stop();

        //! Register a motor with the interface
        virtual bool register_motor(const std::shared_ptr<Motor> &motor);

        //! Set the duty cycle of the motor controller. The duty cycle is a value between -1 and 1.
        virtual void setDuty(uint8_t controller_id, float duty);

        //! Set motor current in Amps.
        virtual void setCurrent(uint8_t controller_id, float current);

        //! Set motor current in Amps with an off delay. The off delay is useful to keep the current controller running for a while even after setting currents below the minimum current.
        virtual void setCurrentOffDelay(uint8_t controller_id, float current, float off_delay);

        //! Set brake current in Amps.
        virtual void setCurrentBrake(uint8_t controller_id, float current);

        //! Set the RPM of the motor controller.
        virtual void setRPM(uint8_t controller_id, float rpm);

        //! Set the position of the motor controller. The position is in turns.
        virtual void setPos(uint8_t controller_id, float pos);

        //! Set the current of the motor controller as a percentage of the maximum current.
        virtual void setCurrentRel(uint8_t controller_id, float current_rel);

        //! Same as above, but also sets the off delay. Note that this command uses 6 bytes now. The off delay is useful to set to keep the current controller running for a while even after setting currents below the minimum current.
        virtual void setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay);

        //! Set the brake current of the motor controller as a percentage of the maximum current.
        virtual void setCurrentBrakeRel(uint8_t controller_id, float current_rel);

        //! Set the handbrake current of the motor controller.
        virtual void setHandbrake(uint8_t controller_id, float current);

        //! Set the handbrake current of the motor controller as a percentage of the maximum current.
        virtual void setHandbrakeRel(uint8_t controller_id, float current_rel);

        //! Check if we are verbose
        [[nodiscard]] bool verbose() const { return mVerbose; }

        //! Set verbose mode
        void setVerbose(bool verbose) { mVerbose = verbose; }

        //! Get motor object by id
        [[nodiscard]] std::shared_ptr<Motor> getMotor(uint8_t id);

    protected:
        //! Do update
        virtual void update();

        //! Callback function for status packets.
        void statusCallback(uint8_t controllerId, float erpm, float current, float dutyCycle);

        //! Callback function for status 2 packets.
        void status2Callback(uint8_t controllerId, float ampHours, float ampHoursCharged);

        //! Callback function for status 3 packets.
        void status3Callback(uint8_t controllerId, float wattHours, float wattHoursCharged);

        //! Callback function for status 4 packets.
        void status4Callback(uint8_t controllerId, float tempFet, float tempMotor, float currentIn, float PIDPos);

        //! Callback function for status 5 packets.
        void status5Callback(uint8_t controllerId, float tachometer, float vIn);

        //! Callback function for status 6 packets.
        void status6Callback(uint8_t controllerId, float adc1, float adc2, float adc3, float ppm);

        std::mutex mMutex; // Mutex for accessing the motor map
        std::vector<std::shared_ptr<Motor>> mMotors = std::vector<std::shared_ptr<Motor>>(256); // Map from motor id to motor object
        bool mVerbose = false;

        friend class Manager;
    };

} // multivesc

#endif //MULTIVESC_COMSINTERFACE_HH
