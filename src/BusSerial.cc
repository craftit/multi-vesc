//
// Created by charles on 17/06/24.
//

#include "multivesc/BusSerial.hh"

#include <iostream>

namespace multivesc
{

    //! Const from port
    BusSerial::BusSerial(std::string port)
    {

    }

    BusSerial::BusSerial(json config)
        : BusInterface(config)
    {
    }

    BusSerial::~BusSerial()
    {
    }

    bool BusSerial::open() {
        return BusInterface::open();
    }

    bool BusSerial::stop() {
        return BusInterface::stop();
    }

    void BusSerial::setDuty(uint8_t controller_id, float duty)
    {
    }

    void BusSerial::setCurrent(uint8_t controller_id, float current) {
        BusInterface::setCurrent(controller_id, current);
    }

    void BusSerial::setCurrentOffDelay(uint8_t controller_id, float current, float off_delay) {
        BusInterface::setCurrentOffDelay(controller_id, current, off_delay);
    }

    void BusSerial::setCurrentBrake(uint8_t controller_id, float current) {
        BusInterface::setCurrentBrake(controller_id, current);
    }

    void BusSerial::setRPM(uint8_t controller_id, float rpm) {
        BusInterface::setRPM(controller_id, rpm);
    }

    void BusSerial::setPos(uint8_t controller_id, float pos) {
        BusInterface::setPos(controller_id, pos);
    }

    void BusSerial::setCurrentRel(uint8_t controller_id, float current_rel) {
        BusInterface::setCurrentRel(controller_id, current_rel);
    }

    void BusSerial::setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay) {
        BusInterface::setCurrentRelOffDelay(controller_id, current_rel, off_delay);
    }

    void BusSerial::setCurrentBrakeRel(uint8_t controller_id, float current_rel) {
        BusInterface::setCurrentBrakeRel(controller_id, current_rel);
    }

    void BusSerial::setHandbrake(uint8_t controller_id, float current) {
        BusInterface::setHandbrake(controller_id, current);
    }

    void BusSerial::setHandbrakeRel(uint8_t controller_id, float current_rel) {
        BusInterface::setHandbrakeRel(controller_id, current_rel);
    }
}
