//
// Created by charles on 17/06/24.
//

#include <iostream>
#include <utility>

#include "multivesc/Motor.hh"
#include "multivesc/Manager.hh"

namespace multivesc {

    std::string to_string(MotorDriveT type)
    {
        switch (type)
        {
            case MotorDriveT::NONE: return "NONE";
            case MotorDriveT::DUTY: return "DUTY";
            case MotorDriveT::CURRENT: return "CURRENT";
            case MotorDriveT::CURRENT_REL: return "CURRENT_REL";
            case MotorDriveT::CURRENT_BREAK: return "CURRENT_BREAK";
            case MotorDriveT::CURRENT_BREAK_REL: return "CURRENT_BREAK_REL";
            case MotorDriveT::RPM: return "RPM";
            case MotorDriveT::POS: return "POS";
            case MotorDriveT::HAND_BRAKE: return "HAND_BRAKE";
            case MotorDriveT::HAND_BRAKE_REL: return "HAND_BRAKE_REL";
        }
        return "UNKNOWN";
    }

    MotorDriveT driveTypeFromString(const std::string& str)
    {
        std::string upper = str;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        for(int i = 0; i < (int)MotorDriveT::HAND_BRAKE_REL; i++)
        {
            if(upper == to_string((MotorDriveT)i))
            {
                return (MotorDriveT)i;
            }
        }
        return MotorDriveT::NONE;
    }


    Motor::Motor(std::string name)
     : mName(std::move(name))
    {}

    bool Motor::configure(Manager &manager, json config)
    {
        mVerbose = config.value("verbose",false);
        if(mVerbose) {
            std::cout << "Configuring motor " << mName << std::endl;
        }
        std::string busName = config.value("bus","can0");
        mComs = manager.getBus(busName);
        if(mComs == nullptr) {
            std::cerr << "Failed to find bus " << busName << std::endl;
            return false;
        }
        mId = config.value("id", 0);
        mEnabled = config.value("enabled", true);
        if(mVerbose) {
            std::cout << " Motor id: " << static_cast<int>(mId) << " Enabled:" << mEnabled << std::endl;
        }
        if(mName.empty()) {
            std::string defaultName = "motor_" + busName + "_" + std::to_string(mId);
            setName(config.value("name", defaultName));
        } else {
            if(config.contains("name")) {
                std::cerr << "Motor name already set, ignoring name in config" << std::endl;
            }
        }
        setMaxRPMAcceleration(config.value("maxRPMAcceleration",-1.0f));
        setMinRPM(config.value("minRPM",0.0f));
        setMaxRPM(config.value("maxRPM",12000));
        mNumPolePairs = config.value("numPoles",1.0f) / 2.0f;
        if(config.value("reverse_direction",false))
            mScaleDirection = mScaleDirection * -1.0;

        if(mVerbose) {
            std::cout << " Min RPM: " << mMinRPM << "   Max RPM Acceleration: " << mMaxRPMAcceleration << std::endl;
            std::cout << " Number of poles: " << (mNumPolePairs*2.0f) << std::endl;
        }
        if(!mComs->register_motor(shared_from_this())) {
            return false;
        }

        std::string controlMode = config.value("controlMode","RPM");
        mPrimaryDriveMode = driveTypeFromString(controlMode);
        if(mPrimaryDriveMode == MotorDriveT::NONE) {
            std::cerr << "Invalid control mode " << controlMode << std::endl;
            return false;
        }
        mDriveMode = MotorDriveT::NONE;

        if(mVerbose) {
            std::cout << " Control mode: " << to_string(mPrimaryDriveMode) << std::endl;
        }

        manager.register_motor(*this);

        // Only do initial setup if motor enabled
        if(mEnabled) {
            // Pause a little before sending the first command
            float startupDelay = config.value("startupDelay",0.0f);
            if(startupDelay > 0.0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(int(startupDelay * 1000.0f)));
            }

            switch(mPrimaryDriveMode)
            {
                case MotorDriveT::NONE:
                    break;
                case MotorDriveT::DUTY:
                    if(config.contains("duty")) {
                        float duty = config["duty"];
                        if(mVerbose) {
                            std::cerr << " Setting initial duty cycle to " << duty << std::endl;
                        }
                        setDuty(duty);
                    }
                break;
                case MotorDriveT::CURRENT:
                    if(config.contains("current")) {
                        float current = config["current"];
                        if(mVerbose) {
                            std::cerr << " Setting initial current to " << current << std::endl;
                        }
                        setCurrent(current);
                    }
                break;
                case MotorDriveT::CURRENT_REL:
                case MotorDriveT::CURRENT_BREAK:
                case MotorDriveT::CURRENT_BREAK_REL:
                    break;
                case MotorDriveT::RPM:
                    if(config.contains("rpm")) {
                        float rpm = config["rpm"];
                        if(mVerbose) {
                            std::cerr << " Setting initial RPM to " << rpm << std::endl;
                        }
                        setRPM(rpm);
                    }
                break;
                case MotorDriveT::POS:
                    if(config.contains("pos")) {
                        float pos = config["pos"];
                        if(mVerbose) {
                            std::cerr << " Setting initial position to " << pos << std::endl;
                        }
                        setPos(pos);
                    }
                break;
                case MotorDriveT::HAND_BRAKE:
                case MotorDriveT::HAND_BRAKE_REL:
                    break;
            }
        }

        return true;
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
        mDriveUpdateTime = now;
        if(!mEnabled) {
            return;
        }
        switch(mDriveMode)
        {
            case MotorDriveT::NONE:
                break;
            case MotorDriveT::DUTY:
                mComs->setDuty(mId, mDriveValue * mScaleDirection);
                break;
            case MotorDriveT::CURRENT:
                mComs->setCurrent(mId, mDriveValue * mScaleDirection);
                break;
            case MotorDriveT::RPM:
                updateRPM(mDriveValue);
                break;
            case MotorDriveT::POS:
                mComs->setPos(mId,mDriveValue * mScaleDirection);
                break;
            case MotorDriveT::CURRENT_REL:
                mComs->setCurrentRel(mId,mDriveValue * mScaleDirection);
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
        duty *= mScaleDirection;
        if(duty < -1.0) {
            duty = -1.0;
        } else if(duty > 1.0) {
            duty = 1.0;
        }
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(mPrimaryDriveMode != MotorDriveT::DUTY) {
            std::cerr << "Motor " << mName << " is not in duty mode" << std::endl;
            return;
        }
        mDriveMode = MotorDriveT::DUTY;
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveValue = duty;
        if(!mComs)
            return ;
        mComs->setDuty(mId, duty * mScaleDirection);
    }

    void Motor::setCurrent(float current)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(mPrimaryDriveMode != MotorDriveT::CURRENT) {
            std::cerr << "Motor " << mName << " is not in current mode" << std::endl;
            return;
        }
        mDriveMode = MotorDriveT::CURRENT;
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setCurrent(mId, current * mScaleDirection);
    }

    void Motor::setCurrentOffDelay(float current, float off_delay)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(mPrimaryDriveMode != MotorDriveT::CURRENT) {
            std::cerr << "Motor " << mName << " is not in current mode" << std::endl;
            return;
        }
        mDriveMode = MotorDriveT::CURRENT;
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setCurrentOffDelay(mId, current * mScaleDirection, off_delay);
    }

    void Motor::setCurrentBrake(float current)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        mDriveMode = MotorDriveT::CURRENT_BREAK;
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveValue = current;
        if(!mComs)
            return ;
        mComs->setCurrentBrake(mId, current);
    }

    void Motor::setRPM(float rpm)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(std::abs(rpm) > mMaxRPM) {
            std::cerr << "RPM " << rpm << " exceeds maximum RPM " << mMaxRPM << " Limiting to max of " << mMaxRPM << std::endl;
            if(rpm > 0) {
                rpm = mMaxRPM;
            } else {
                rpm = -mMaxRPM;
            }
        }
        mDriveUpdateTime = std::chrono::steady_clock::now();
        if(mPrimaryDriveMode != MotorDriveT::RPM) {
            std::cerr << "Motor " << mName << " is not in RPM mode" << std::endl;
            return;
        }
        mDriveMode = MotorDriveT::RPM;
        mDriveValue = rpm;
        if(!mComs)
            return ;
        updateRPM(rpm);
    }

    void Motor::updateRPM(float rpm)
    {
        auto now = std::chrono::steady_clock::now();
        // Make sure we start at the minimum RPM, needed for sensor-less operation
        if (std::abs(rpm) < mMinRPM) {
            // If less than half the minimum RPM, set to zero
            if(std::abs(rpm) < mMinRPM/2) {
                rpm = 0.0;
            } else {
                rpm = std::abs(mMinRPM);
            }
        }
        if(mMaxRPMAcceleration >= 0.0) {
            auto dt = now - mLastRPMDemandChange;
            // Convert dt to floating point seconds
            float dtf = std::chrono::duration<float>(dt).count();
            if (dtf >= 0.0) {
                float deltaRPM = rpm - mLastRPMDemand;
                float maxDeltaRPM = mMaxRPMAcceleration * dtf;
                if (deltaRPM > maxDeltaRPM) {
                    rpm = mLastRPMDemand + maxDeltaRPM;
                } else if (deltaRPM < -maxDeltaRPM) {
                    rpm = mLastRPMDemand - maxDeltaRPM;
                }
            }
        }
        // Limit the maximum RPM
        if (std::abs(rpm) > mMaxRPM) {
            if(rpm > 0)
                rpm = mMaxRPM;
            else
                rpm = -mMaxRPM;
        }

        mLastRPMDemandChange = now;
        mLastRPMDemand = rpm;
        mComs->setRPM(mId, rpm * mNumPolePairs * mScaleDirection);
    }

    void Motor::setPos(float pos)
    {
        if(!mComs)
            return ;
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(mPrimaryDriveMode != MotorDriveT::POS) {
            std::cerr << "Motor " << mName << " is not in position mode" << std::endl;
            return;
        }
        mDriveMode = MotorDriveT::POS;
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveValue = pos;
        mComs->setPos(mId, pos * mScaleDirection);
    }

    void Motor::setCurrentRel(float current_rel)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(mPrimaryDriveMode != MotorDriveT::CURRENT_REL) {
            std::cerr << "Motor " << mName << " is not in current relative mode" << std::endl;
            return;
        }
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setCurrentRel(mId, current_rel  * mScaleDirection);
    }

    void Motor::setCurrentRelOffDelay(float current_rel, float off_delay)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
        if(mPrimaryDriveMode != MotorDriveT::CURRENT_REL) {
            std::cerr << "Motor " << mName << " is not in current relative mode" << std::endl;
            return;
        }
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::CURRENT_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setCurrentRelOffDelay(mId, current_rel  * mScaleDirection, off_delay);
    }

    void Motor::setCurrentBrakeRel(float current_rel)
    {
        std::lock_guard lock(mDriveMutex);
        if(!mEnabled)
            return;
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
        if(!mEnabled)
            return;
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
        if(!mEnabled)
            return;
        mDriveUpdateTime = std::chrono::steady_clock::now();
        mDriveMode = MotorDriveT::HAND_BRAKE_REL;
        mDriveValue = current_rel;
        if(!mComs)
            return ;
        mComs->setHandbrakeRel(mId, current_rel);
    }

    void Motor::setMinRPM(float rpm)
    {
        mMinRPM = rpm;
    }

    void Motor::setMaxRPM(float rpm)
    {
        mMaxRPM = rpm;
    }

    void Motor::setMaxRPMAcceleration(float rpm_per_sec)
    {
        mMaxRPMAcceleration = rpm_per_sec;
    }


} // multivesc