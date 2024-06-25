//
// Created by charles on 17/06/24.
//

#include <iostream>
#include "multivesc/BusInterface.hh"
#include "multivesc/Motor.hh"

namespace multivesc
{

    //! Constructor
    BusInterface::BusInterface()
    = default;

    BusInterface::BusInterface(const json &config)
        : mVerbose(config.value("verbose",false))
    {
    }


    bool BusInterface::open()
    {
        return false;
    }

    //! Stop the interface
    bool BusInterface::stop()
    {
        return true;
    }

    bool BusInterface::register_motor(const std::shared_ptr<Motor> &motor)
    {
        std::lock_guard lock(mMutex);
        size_t id = motor->id();
        if(id >= mMotors.size()) {
            std::cerr << "Motor id out of range. " << id << " >= " << mMotors.size() << std::endl;
            return false;
        }
        mMotors[id] = motor;
        return true;
    }

    //! Get motor object by id
    std::shared_ptr<Motor> BusInterface::getMotor(uint8_t id)
    {
        std::lock_guard lock(mMutex);
        if(!mMotors[id]) {
            mMotors[id] = std::make_shared<Motor>();
        }
        return mMotors[id];
    }

    void BusInterface::update()
    {
        for(auto& motor : mMotors)
        {
            if(motor == nullptr)
                continue;
            motor->update();
        }
    }


    void BusInterface::statusCallback(uint8_t controllerId, float erpm, float current, float dutyCycle)
    {
        auto motor = getMotor(controllerId);
        motor->statusCallback(erpm, current, dutyCycle);
    }

    void BusInterface::status2Callback(uint8_t controllerId, float ampHours, float ampHoursCharged)
    {
        auto motor = getMotor(controllerId);
        motor->status2Callback(ampHours, ampHoursCharged);
    }

    void BusInterface::status3Callback(uint8_t controllerId, float wattHours, float wattHoursCharged)
    {
        auto motor = getMotor(controllerId);
        motor->status3Callback(wattHours, wattHoursCharged);
    }

    void
    BusInterface::status4Callback(uint8_t controllerId, float tempFet, float tempMotor, float currentIn, float PIDPos)
    {
        auto motor = getMotor(controllerId);
        motor->status4Callback(tempFet, tempMotor, currentIn, PIDPos);
    }

    void BusInterface::status5Callback(uint8_t controllerId, float tachometer, float vIn)
    {
        auto motor = getMotor(controllerId);
        motor->status5Callback(tachometer, vIn);
    }

    void BusInterface::status6Callback(uint8_t controllerId, float adc1, float adc2, float adc3, float ppm)
    {
        auto motor = getMotor(controllerId);
        motor->status6Callback(adc1, adc2, adc3, ppm);
    }

    void BusInterface::setDuty(uint8_t controller_id, float duty)
    {
        std::cerr << "setDuty not implemented" << std::endl;
    }

    void BusInterface::setCurrent(uint8_t controller_id, float current)
    {
        std::cerr << "setCurrent not implemented" << std::endl;
    }

    void BusInterface::setCurrentOffDelay(uint8_t controller_id, float current, float off_delay)
    {
        std::cerr << "setCurrentOffDelay not implemented" << std::endl;
    }

    void BusInterface::setCurrentBrake(uint8_t controller_id, float current)
    {
        std::cerr << "setCurrentBrake not implemented" << std::endl;
    }

    void BusInterface::setRPM(uint8_t controller_id, float rpm)
    {
        std::cerr << "setRPM not implemented" << std::endl;
    }

    void BusInterface::setPos(uint8_t controller_id, float pos)
    {
        std::cerr << "setPos not implemented" << std::endl;
    }

    void BusInterface::setCurrentRel(uint8_t controller_id, float current_rel)
    {
        std::cerr << "setCurrentRel not implemented" << std::endl;
    }

    void BusInterface::setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay)
    {
        std::cerr << "setCurrentRelOffDelay not implemented" << std::endl;
    }

    void BusInterface::setCurrentBrakeRel(uint8_t controller_id, float current_rel)
    {
        std::cerr << "setCurrentBrakeRel not implemented" << std::endl;
    }

    void BusInterface::setHandbrake(uint8_t controller_id, float current)
    {
        std::cerr << "setHandbrake not implemented" << std::endl;
    }

    void BusInterface::setHandbrakeRel(uint8_t controller_id, float current_rel)
    {
        std::cerr << "setHandbrakeRel not implemented" << std::endl;
    }


} // multivesc