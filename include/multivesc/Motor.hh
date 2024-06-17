//
// Created by charles on 17/06/24.
//

#ifndef MUTLIVESC_MOTOR_HH
#define MUTLIVESC_MOTOR_HH

#include "multivesc/ComsInterface.hh"

namespace multivesc {

    class Motor
    {
    public:
        Motor() = default;

        //! Constructor
        explicit Motor(std::shared_ptr<ComsInterface> coms, uint8_t id = 0);

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



    protected:
        std::shared_ptr<ComsInterface> mComs;
        uint8_t mId = 0; // Controller ID

        float eRpm = 0.0;
        float eCurrent = 0.0;
        float mDuty = 0.0;
        float mAmpHours = 0.0;
        float mAmpHoursCharged = 0.0;
        float mWattHours = 0.0;
        float mWattHoursCharged = 0.0;
        float mTempFet = 0.0;
        float mTempMotor = 0.0;
        float mCurrentIn = 0.0;
        float mPidPos = 0.0;
        float mTachometer = 0.0;
        float mVIn = 0.0;
        float mADC1 = 0.0;
        float mADC2 = 0.0;
        float mADC3 = 0.0;
        float mPPM = 0.0;
    };

} // multivesc

#endif //MUTLIVESC_MOTOR_HH
