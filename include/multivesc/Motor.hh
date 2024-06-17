//
// Created by charles on 17/06/24.
//

#ifndef MUTLIVESC_MOTOR_HH
#define MUTLIVESC_MOTOR_HH

#include <atomic>
#include <string>
#include "multivesc/ComsInterface.hh"

namespace multivesc {

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

    class Motor
    {
    public:
        Motor() = default;

        //! Constructor
        explicit Motor(std::shared_ptr<ComsInterface> coms, uint8_t id = 0);

        //! Set the duty cycle of the motor controller. The duty cycle is a value between -1 and 1.
        void setDuty(uint8_t controller_id, float duty);

        //! Set motor current in Amps.
        void setCurrent(uint8_t controller_id, float current);

        //! Set motor current in Amps with an off delay. The off delay is useful to keep the current controller running for a while even after setting currents below the minimum current.
        void setCurrentOffDelay(uint8_t controller_id, float current, float off_delay);

        //! Set brake current in Amps.
        void setCurrentBrake(uint8_t controller_id, float current);

        //! Set the RPM of the motor controller.
        void setRPM(uint8_t controller_id, float rpm);

        //! Set the position of the motor controller. The position is in turns.
        void setPos(uint8_t controller_id, float pos);

        //! Set the current of the motor controller as a percentage of the maximum current.
        void setCurrentRel(uint8_t controller_id, float current_rel);

        //! Same as above, but also sets the off delay. Note that this command uses 6 bytes now. The off delay is useful to set to keep the current controller running for a while even after setting currents below the minimum current.
        void setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay);

        //! Set brake current in Amps as a percentage of the maximum current.
        void setCurrentBrakeRel(uint8_t controller_id, float current_rel);

        //! Set handbrake current in Amps.
        void setHandbrake(uint8_t controller_id, float current);

        //! Set handbrake current in Amps as a percentage of the maximum current.
        void setHandbrakeRel(uint8_t controller_id, float current_rel);

        //! Access motor id
        [[nodiscard]] uint8_t id() const { return mId; }

        //! Access motor speed in RPM
        [[nodiscard]] float rpm() const { return eRpm; }

        //! Access motor current in Amps
        [[nodiscard]] float current() const { return eCurrent; }

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


        std::shared_ptr<ComsInterface> mComs;
        std::string mName;
        std::mutex mMutex;
        std::function<void(MotorValuesT,float)> mCallback;

        uint8_t mId = 0; // Controller ID

        std::atomic<float> eRpm = 0.0;
        std::atomic<float> eCurrent = 0.0;
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

        friend class ComsInterface;
    };


} // multivesc

#endif //MUTLIVESC_MOTOR_HH
