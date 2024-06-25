//
// Created by charles on 17/06/24.
//

#ifndef MUTLIVESC_COMSSERIAL_HH
#define MUTLIVESC_COMSSERIAL_HH

#include <vector>
#include <string>
#include <cstdint>

#include "multivesc/ComsInterface.hh"


namespace multivesc {

    //! Serial interface to communicate with the VESC
    class ComsSerial
       : public ComsInterface
    {
    public:
        //! Const from port
        explicit ComsSerial(std::string port);

        //! Const from port
        explicit ComsSerial(json config);

        //! Destructor
        ~ComsSerial() override;

        //! Open the CAN interface.
        bool open() override;

        //! Stop the CAN interface and close the socket.
        bool stop() override;

        //! Set the duty cycle of the motor controller. The duty cycle is a value between -1 and 1.
        void setDuty(uint8_t controller_id, float duty) override;

        //! Set motor current in Amps.
        void setCurrent(uint8_t controller_id, float current) override;

        //! Set motor current in Amps with an off delay. The off delay is useful to keep the current controller running for a while even after setting currents below the minimum current.
        void setCurrentOffDelay(uint8_t controller_id, float current, float off_delay) override;

        //! Set brake current in Amps.
        void setCurrentBrake(uint8_t controller_id, float current) override;

        //! Set the RPM of the motor controller.
        void setRPM(uint8_t controller_id, float rpm) override;

        //! Set the position of the motor controller. The position is in turns.
        void setPos(uint8_t controller_id, float pos) override;

        //! Set the current of the motor controller as a percentage of the maximum current.
        void setCurrentRel(uint8_t controller_id, float current_rel) override;

        //! Same as above, but also sets the off delay. Note that this command uses 6 bytes now. The off delay is useful to set to keep the current controller running for a while even after setting currents below the minimum current.
        void setCurrentRelOffDelay(uint8_t controller_id, float current_rel, float off_delay) override;

        //! Set brake current in Amps as a percentage of the maximum current.
        void setCurrentBrakeRel(uint8_t controller_id, float current_rel) override;

        //! Set handbrake current in Amps.
        void setHandbrake(uint8_t controller_id, float current) override;

        //! Set handbrake current in Amps as a percentage of the maximum current.
        void setHandbrakeRel(uint8_t controller_id, float current_rel) override;

    protected:
        std::string mPort;
        std::thread mReceiveThread;

        int mFd = -1;
    };

}


#endif //MUTLIVESC_COMSSERIAL_HH
