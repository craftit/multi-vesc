//
// Created by charles on 17/06/24.
//

#ifndef MULTIVESC_COMSINTERFACE_HH
#define MULTIVESC_COMSINTERFACE_HH

#include <vector>
#include <cstdint>
#include <memory>
#include <thread>
#include <functional>

namespace multivesc
{

    class Motor;

    //! Abstract class for communicating with the VESC
    //! The implementation deal with can bus and serial communication
    //!
    class ComsInterface
    {
    public:
        virtual ~ComsInterface() = default;

        //! Start the coms interface
        virtual bool open();

        //! Stop the interface
        virtual bool stop();

    protected:
        std::vector<std::shared_ptr<Motor>> mMotors; // Map from motor id to motor object
    };

} // multivesc

#endif //MULTIVESC_COMSINTERFACE_HH
