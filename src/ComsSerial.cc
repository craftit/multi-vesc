//
// Created by charles on 17/06/24.
//

#include "multivesc/ComsSerial.hh"

#include <iostream>

namespace multivesc
{

    //! Const from port
    ComsSerial::ComsSerial(std::string port)
    {

    }

    ComsSerial::ComsSerial(json config)
        : ComsInterface(config)
    {
    }

    ComsSerial::~ComsSerial()
    {
    }

    bool ComsSerial::open() {
        return ComsInterface::open();
    }

    bool ComsSerial::stop() {
        return ComsInterface::stop();
    }

    void ComsSerial::setDuty(uint8_t controller_id, float duty)
    {
    }

    void ComsSerial::setCurrent(uint8_t controller_id, float current) {
        ComsInterface::setCurrent(controller_id, current);
    }

    void ComsSerial::setCurrentOffDelay(uint8_t controller_id, float current, float off_delay) {
        ComsInterface::setCurrentOffDelay(controller_id, current, off_delay);
    }

    void ComsSerial::setCurrentBrake(uint8_t controller_id, float current) {
        ComsInterface::setCurrentBrake(controller_id, current);
    }

    void ComsSerial::setRPM(uint8_t controller_id, float rpm) {
        ComsInterface::setRPM(controller_id, rpm);
    }

    void ComsSerial::setPos(uint8_t controller_id, float pos) {
        ComsInterface::setPos(controller_id, pos);
    }

    void ComsSerial::setCurrentRel(uint8_t controller_id, float current_rel) {
        ComsInterface::setCurrentRel(controller_id, current_rel);
    }

    void ComsSerial::setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay) {
        ComsInterface::setCurrentRelOffDelay(controller_id, current_rel, off_delay);
    }

    void ComsSerial::setCurrentBrakeRel(uint8_t controller_id, float current_rel) {
        ComsInterface::setCurrentBrakeRel(controller_id, current_rel);
    }

    void ComsSerial::setHandbrake(uint8_t controller_id, float current) {
        ComsInterface::setHandbrake(controller_id, current);
    }

    void ComsSerial::setHandbrakeRel(uint8_t controller_id, float current_rel) {
        ComsInterface::setHandbrakeRel(controller_id, current_rel);
    }
}
