//
// Created by charles on 17/06/24.
//

#ifndef MULTI_VESC_MANAGER_HH
#define MULTI_VESC_MANAGER_HH

#include <multivesc/ComsInterface.hh>
#include <multivesc/Motor.hh>

namespace multivesc {


    //! Manager class for handling multiple motors
    // This provides a way of finding motors by id and starting/stopping the update thread

    class Manager
    {
    public:
        //! Constructor
        Manager() = default;

        //! Constructor
        explicit Manager(std::shared_ptr<ComsInterface> coms);

        //! Destructor
        virtual ~Manager();

        //! Open a CAN port
        bool openCan(const std::string& port);

        //! Find a motor by id
        std::shared_ptr<Motor> getMotor(uint8_t id);

        //! Start the manager
        bool start();

        //! Stop the manager
        bool stop();

    protected:
        void runUpdate();

        std::atomic<bool> mTerminate = false;
        std::thread mUpdateThread;
        std::mutex mMutex;

        std::shared_ptr<ComsInterface> mPrimaryComs;
        std::vector<std::shared_ptr<Motor>> mMotors;
    };

} // multivesc

#endif //MULTI_VESC_MANAGER_HH
