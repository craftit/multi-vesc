//
// Created by charles on 17/06/24.
//

#include <utility>

#include "multivesc/Motor.hh"

namespace multivesc {

    Motor::Motor(std::shared_ptr<ComsInterface> coms, uint8_t id)
    : mComs(std::move(coms)),
      mId(id)
    {
    }

    void Motor::setCallback(std::function<void(MotorValuesT, float)> callback) {
        std::lock_guard lock(mMutex);
        mCallback = callback;
    };

    void Motor::doCallback(MotorValuesT type, float value) {
        std::lock_guard lock(mMutex);
        if(mCallback) {
            mCallback(type, value);
        }
    }


    void Motor::statusCallback(float erpm, float current, float dutyCycle)
    {
        eRpm = erpm;
        eCurrent = current;
        mDuty = dutyCycle;
        doCallback(MotorValuesT::RPM, eRpm);
        doCallback(MotorValuesT::CURRENT, eCurrent);
        doCallback(MotorValuesT::DUTY, mDuty);
    }

    void Motor::status2Callback(float ampHours, float ampHoursCharged)
    {
        mAmpHours = ampHours;
        mAmpHoursCharged = ampHoursCharged;
        doCallback(MotorValuesT::AMPHOURS, mAmpHours);
        doCallback(MotorValuesT::AMPHOURSCHARGED, mAmpHoursCharged);
    }

    void Motor::status3Callback(float wattHours, float wattHoursCharged)
    {
        mWattHours = wattHours;
        mWattHoursCharged = wattHoursCharged;
        doCallback(MotorValuesT::WATTHOURS, mWattHours);
        doCallback(MotorValuesT::WATTHOURSCHARGED, mWattHoursCharged);
    }

    void Motor::status4Callback(float tempFet, float tempMotor, float currentIn, float PIDPos)
    {
        mTempFet = tempFet;
        mTempMotor = tempMotor;
        mCurrentIn = currentIn;
        mPidPos = PIDPos;
        doCallback(MotorValuesT::TEMP_FET, mTempFet);
        doCallback(MotorValuesT::TEMP_MOTOR, mTempMotor);
        doCallback(MotorValuesT::CURRENT_IN, mCurrentIn);
        doCallback(MotorValuesT::PID_POS, mPidPos);
    }

    void Motor::status5Callback(float tachometer, float vIn)
    {
        mTachometer = tachometer;
        mVIn = vIn;
        doCallback(MotorValuesT::TACHOMETER, mTachometer);
        doCallback(MotorValuesT::VIN, mVIn);
    }

    void Motor::status6Callback(float adc1, float adc2, float adc3, float ppm)
    {
        mADC1 = adc1;
        mADC2 = adc2;
        mADC3 = adc3;
        mPPM = ppm;
        doCallback(MotorValuesT::ADC1, mADC1);
        doCallback(MotorValuesT::ADC2, mADC2);
        doCallback(MotorValuesT::ADC3, mADC3);
        doCallback(MotorValuesT::PPM, mPPM);
    }

    void Motor::setDuty(uint8_t controller_id, float duty)
    {
        if(!mComs)
            return ;
        if(duty < -1.0) {
            duty = -1.0;
        } else if(duty > 1.0) {
            duty = 1.0;
        }
        mComs->setDuty(mId, duty);
    }

    void Motor::setCurrent(uint8_t controller_id, float current)
    {
        if(!mComs)
            return ;
        mComs->setCurrent(mId, current);
    }

    void Motor::setCurrentOffDelay(uint8_t controller_id, float current, float off_delay)
    {
        if(!mComs)
            return ;
        mComs->setCurrentOffDelay(mId, current, off_delay);
    }

    void Motor::setCurrentBrake(uint8_t controller_id, float current)
    {
        if(!mComs)
            return ;
        mComs->setCurrentBrake(mId, current);
    }

    void Motor::setRPM(uint8_t controller_id, float rpm)
    {
        if(!mComs)
            return ;
        mComs->setRPM(mId, rpm);
    }

    void Motor::setPos(uint8_t controller_id, float pos)
    {
        if(!mComs)
            return ;
        mComs->setPos(mId, pos);
    }

    void Motor::setCurrentRel(uint8_t controller_id, float current_rel)
    {
        if(!mComs)
            return ;
        mComs->setCurrentRel(mId, current_rel);
    }

    void Motor::setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay)
    {
        if(!mComs)
            return ;
        mComs->setCurrentRelOffDelay(mId, current_rel, off_delay);
    }

    void Motor::setCurrentBrakeRel(uint8_t controller_id, float current_rel)
    {
        if(!mComs)
            return ;
        mComs->setCurrentBrakeRel(mId, current_rel);
    }

    void Motor::setHandbrake(uint8_t controller_id, float current)
    {
        if(!mComs)
            return ;
        mComs->setHandbrake(mId, current);
    }

    void Motor::setHandbrakeRel(uint8_t controller_id, float current_rel)
    {
        if(!mComs)
            return ;
        mComs->setHandbrakeRel(mId, current_rel);
    }




} // multivesc