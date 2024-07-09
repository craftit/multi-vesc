//
// Created by charles on 17/06/24.
//

#ifndef MUTLIVESC_MOTOR_HH
#define MUTLIVESC_MOTOR_HH

#include <atomic>
#include <string>
#include "multivesc/BusInterface.hh"

namespace multivesc {

    class Manager;

    //! List of motor sensor value types
    enum class MotorValuesT
    {
        RPM,
        CURRENT,
        DUTY,
        AMPHOURS,
        AMPHOURSCHARGED,
        WATTHOURS,
        WATTHOURSCHARGED,
        TEMP_FET,
        TEMP_MOTOR,
        CURRENT_IN,
        PID_POS,
        TACHOMETER,
        VIN,
        ADC1,
        ADC2,
        ADC3,
        PPM
    };

    //! Drive modes for the motor controller
    enum class MotorDriveT
    {
        NONE,
        DUTY,
        CURRENT,
        CURRENT_REL,
        CURRENT_BREAK,
        CURRENT_BREAK_REL,
        RPM,
        POS,
        HAND_BRAKE,
        HAND_BRAKE_REL
    };

    //! Convert a MotorDriveT to a string
    std::string to_string(MotorDriveT type);

    //! Convert a string to a MotorDriveT
    //! If the string is not recognized, MotorDriveT::NONE is returned.
    MotorDriveT driveTypeFromString(const std::string& str);

    //! Motor interface

    class Motor
        : public std::enable_shared_from_this<Motor>
    {
    public:
        Motor() = default;

        //! Construct a named motor
        explicit Motor(std::string name);

        //! Disable copy and move constructors
        Motor(const Motor&) = delete;
        Motor& operator=(const Motor&) = delete;
        Motor(Motor&&) = delete;
        Motor& operator=(Motor&&) = delete;

        //! Configure the motor from a json object.
        //! This will register the motor with the manager and set up the motor parameters.
        bool configure(Manager &manager,json config);

        //! Set the name of the motor.
        void setName(const std::string& name);

        //! Get the name of the motor.
        [[nodiscard]] std::string name() const { return mName; }

        //! Set the minimum RPM for the motor.
        //! This is required for sensorless operation.
        void setMinRPM(float rpm);

        //! Set the maximum RPM for the motor.
        //! This is required for sensorless operation.
        void setMaxRPM(float rpm);

        //! Set the maximum RPM acceleration for the motor.
        //! The acceleration is in RPM per second. Negative values disable acceleration limiting.
        void setMaxRPMAcceleration(float rpm_per_sec);

        //! Set the duty cycle of the motor controller. The duty cycle is a value between -1 and 1.
        void setDuty(float duty);

        //! Set motor current in Amps.
        void setCurrent(float current);

        //! Set motor current in Amps with an off delay. The off delay is useful to keep the current controller running for a while even after setting currents below the minimum current.
        void setCurrentOffDelay(float current, float off_delay);

        //! Set brake current in Amps.
        void setCurrentBrake(float current);

        //! Set the RPM of the motor controller.
        void setRPM(float rpm);

        //! Set the position of the motor controller. The position is in turns.
        void setPos(float pos);

        //! Set the current of the motor controller as a percentage of the maximum current.
        void setCurrentRel(float current_rel);

        //! Same as above, but also sets the off delay. Note that this command uses 6 bytes now. The off delay is useful to set to keep the current controller running for a while even after setting currents below the minimum current.
        void setCurrentRelOffDelay(float current_rel, float off_delay);

        //! Set brake current in Amps as a percentage of the maximum current.
        void setCurrentBrakeRel(float current_rel);

        //! Set handbrake current in Amps.
        void setHandbrake(float current);

        //! Set handbrake current in Amps as a percentage of the maximum current.
        void setHandbrakeRel(float current_rel);

        //! Get the RPM maximum limit
        [[nodiscard]] float maxRPM() const { return mMaxRPM; }

        //! Access motor id as used on the bus
        [[nodiscard]] uint8_t id() const { return mId; }

        //! Access motor speed in RPM
        [[nodiscard]] float rpm() const { return mERpm / mNumPolePairs; }

        //! Access motor current in Amps
        [[nodiscard]] float current() const { return mECurrent; }

        //! Access motor duty cycle (0 to 1)
        [[nodiscard]] float duty() const { return mDuty; }

        //! Access motor consumed AmpHours
        [[nodiscard]] float ampHours() const { return mAmpHours; }

        //! Access motor charged AmpHours
        [[nodiscard]] float ampHoursCharged() const { return mAmpHoursCharged; }

        //! Access motor consumed WattHours
        [[nodiscard]] float wattHours() const { return mWattHours; }

        //! Access motor charged WattHours
        [[nodiscard]] float wattHoursCharged() const { return mWattHoursCharged; }

        //! Access motor FET temperature in Celsius
        [[nodiscard]] float tempFet() const { return mTempFet; }

        //! Access motor temperature in Celsius
        [[nodiscard]] float tempMotor() const { return mTempMotor; }

        //! Access motor current input in Amps
        [[nodiscard]] float currentIn() const { return mCurrentIn; }

        //! Access motor PID position
        [[nodiscard]] float pidPos() const { return mPidPos; }

        //! Access motor tachometer
        [[nodiscard]] float tachometer() const { return mTachometer; }

        //! Access motor input voltage
        [[nodiscard]] float vIn() const { return mVIn; }

        //! Access motor ADC1
        [[nodiscard]] float adc1() const { return mADC1; }

        //! Access motor ADC2
        [[nodiscard]] float adc2() const { return mADC2; }

        //! Access motor ADC3
        [[nodiscard]] float adc3() const { return mADC3; }

        //! Access motor PPM
        [[nodiscard]] float ppm() const { return mPPM; }

        //! Set up a callback function to be called when the motor status is updated.
        void setCallback(std::function<void(MotorValuesT,float)> callback);
    protected:
        void doCallback(MotorValuesT type, float value);

        //! Do an update.
        void update();

        //! Callback function for status packets.
        void statusCallback(float erpm, float current, float dutyCycle);

        //! Callback function for status 2 packets.
        void status2Callback(float ampHours, float ampHoursCharged);

        //! Callback function for status 3 packets.
        void status3Callback(float wattHours, float wattHoursCharged);

        //! Callback function for status 4 packets.
        void status4Callback(float tempFet, float tempMotor, float currentIn, float PIDPos);

        //! Callback function for status 5 packets.
        void status5Callback(float tachometer, float vIn);

        //! Callback function for status 6 packets.
        void status6Callback(float adc1, float adc2, float adc3, float ppm);

        //! Update RPM
        void updateRPM(float rpm);

        std::shared_ptr<BusInterface> mComs;
        std::string mName;
        std::mutex mMutex;
        std::function<void(MotorValuesT,float)> mCallback;

        // Drive mode
        std::mutex mDriveMutex;
        bool mEnabled = true;
        std::atomic<bool> mVerbose = false;
        std::atomic<float> mNumPolePairs = 1.0f;
        MotorDriveT mPrimaryDriveMode = MotorDriveT::NONE;
        MotorDriveT mDriveMode = MotorDriveT::NONE;
        float mDriveValue = 0.0f;
        float mLastDriveValue = 0.0f;
        std::chrono::steady_clock::time_point mDriveUpdateTime;
        std::chrono::steady_clock::duration mDriveTimeout = std::chrono::milliseconds(200);

        uint8_t mId = 0; // Controller ID

        std::atomic<float> mMinRPM = 5000.0f;
        std::atomic<float> mMaxRPM = 12000.0f;
        std::atomic<float> mMaxRPMAcceleration = -1.0f; //! In RPM per second, negative values disable acceleration limiting.
        std::chrono::steady_clock::time_point mLastRPMDemandChange;
        float mLastRPMDemand = 0.0f;

        // Sensor values
        std::atomic<float> mERpm = 0.0f;
        std::atomic<float> mECurrent = 0.0f;
        std::atomic<float> mDuty = 0.0;
        std::atomic<float> mAmpHours = 0.0;
        std::atomic<float> mAmpHoursCharged = 0.0;
        std::atomic<float> mWattHours = 0.0;
        std::atomic<float> mWattHoursCharged = 0.0;
        std::atomic<float> mTempFet = 0.0;
        std::atomic<float> mTempMotor = 0.0;
        std::atomic<float> mCurrentIn = 0.0;
        std::atomic<float> mPidPos = 0.0;
        std::atomic<float> mTachometer = 0.0;
        std::atomic<float> mVIn = 0.0;
        std::atomic<float> mADC1 = 0.0;
        std::atomic<float> mADC2 = 0.0;
        std::atomic<float> mADC3 = 0.0;
        std::atomic<float> mPPM = 0.0;

        friend class BusInterface;
        friend class Manager;
    };


} // multivesc

#endif //MUTLIVESC_MOTOR_HH
