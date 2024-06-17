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
        ComsSerial(std::string port);

        ~ComsSerial();


    protected:
        std::string mPort;
        int mFd = -1;
    };

}


#endif //MUTLIVESC_COMSSERIAL_HH
