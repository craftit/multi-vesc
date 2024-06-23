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

    void Motor::setName(const std::string &name)
    {
        std::lock_guard lock(mMutex);
        mName = name;
    }


    void Motor::setCallback(std::function<void(MotorValuesT, float)> callback) {
        std::lock_guard lock(mMutex);
        mCallback = std::move(callback);
    };

    void Motor::doCallback(MotorValuesT type, float value) {
        std::lock_guard lock(mMutex);
        if(mCallback) {
            mCallback(type, value);
        }
    }

    void Motor::update()
    {
        std::lock_guard lock(mDriveMutex);
        auto now = std::chrono::steady_clock::now();
        if((now - mDriveUpdateTime) < mDriveTimeout)
        {
            // No need to update control values.
            return;
        }
        mDriveUpdateTime = now;
        switch(mDriveMode)
        {
            case MotorDriveT::NONE:
                break;
            case MotorDriveT::DUTY:
                mComs->setDuty(mId, mDriveValue);
                break;
            case MotorDriveT::CURRENT:
                mComs->setCurrent(mId,mDriveValue);
                break;
            case MotorDriveT::RPM:
                mComs->setRPM(mId,mDriveValue);
                break;
            case MotorDriveT::POS:
                mComs->setPos(mId,mDriveValue);
                break;
            case MotorDriveT::CURRENT_REL:
                mComs->setCurrentRel(mId,mDriveValue);
                break;
            case MotorDriveT::CURRENT_BREAK:
                mComs->setCurrentBrake(mId,mDriveValue);
                break;
            case MotorDriveT::CURRENT_BREAK_REL:
                mComs->setCurrentBrakeRel(mId,mDriveValue);
                break;
            case MotorDriveT::HAND_BRAKE:
                mComs->setHandbrake(mId,mDriveValue);
                break;
            case MotorDriveT::HAND_BRAKE_REL:
                mComs->setHandbrakeRel(mId,mDriveValue);
                break;
        }

    }


    void Motor::statusCallback(float erpm, float current, float dutyCycle)
    {
        mERpm = erpm;
        mECurrent = current;
        mDuty = dutyCycle;
        doCallback(MotorValuesT::RPM, mERpm);
        doCallback(MotorValuesT::CURRENT, mECurrent);
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

    void Motor::setDuty(float duty)
    {
        if(duty < -1.0) {
            duty = -1.0;
        } else if(duty > 1.0) {
            duty = 1.0;
        }
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::DUTY;
        mDriveValue = duty;
        if(!mComs)
            return ;
        mComs->setDuty(mId, duty);
    }

    void Motor::setCurrent(float current)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT;
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setCurrent(mId, current);
    }

    void Motor::setCurrentOffDelay(float current, float off_delay)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT;
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setCurrentOffDelay(mId, current, off_delay);
    }

    void Motor::setCurrentBrake(float current)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT_BREAK;
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setCurrentBrake(mId, current);
    }

    void Motor::setRPM(float rpm)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::RPM;
        mDriveValue = rpm;
        if(!mComs)
            return ;
        mComs->setRPM(mId, rpm);
    }

    void Motor::setPos(float pos)
    {
        if(!mComs)
            return ;
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::POS;
        mDriveValue = pos;
        mComs->setPos(mId, pos);
    }

    void Motor::setCurrentRel(float current_rel)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setCurrentRel(mId, current_rel);
    }

    void Motor::setCurrentRelOffDelay(float current_rel, float off_delay)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setCurrentRelOffDelay(mId, current_rel, off_delay);
    }

    void Motor::setCurrentBrakeRel(float current_rel)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT_BREAK_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setCurrentBrakeRel(mId, current_rel);
    }

    void Motor::setHandbrake(float current)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::HAND_BRAKE;
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setHandbrake(mId, current);
    }

    void Motor::setHandbrakeRel(float current_rel)
    {
        std::lock_guard lock(mDriveMutex);
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::HAND_BRAKE_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setHandbrakeRel(mId, current_rel);
    }


} // multivesc