//
// Created by charles on 17/06/24.
//

#ifndef MULTI_VESC_MANAGER_HH
#define MULTI_VESC_MANAGER_HH

#include <map>
#include <nlohmann/json.hpp>
#include "multivesc/BusInterface.hh"
#include "multivesc/Motor.hh"

namespace multivesc {


    //! Manager class for handling multiple motors
    // This provides a way of finding motors by id and starting/stopping the update thread

    class Manager
    {
    public:
        //! Constructor
        Manager() = default;

        //! Disable copy and move constructors
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

        //! Constructor
        explicit Manager(json config);

        //! Destructor
        virtual ~Manager();

        //! Load configuration from a json object
        bool configure(json config);

        //! Open a CAN port
        bool openCan(const std::string& port);

        //! Find bus
        [[nodiscard]] std::shared_ptr<BusInterface> getBus(const std::string& name);

        //! Find a motor by name
        //! Returns nullptr if the motor is not found
        [[nodiscard]] std::shared_ptr<Motor> getMotor(const std::string& name);

        //! Start the manager
        bool start();

        //! Stop the manager
        bool stop();

        //! Setup verbose output
        void setVerbose(bool verbose)
        { mVerbose = verbose; }

        //! Get verbose output
        [[nodiscard]] bool verbose() const
        { return mVerbose; }

        //! Get all motors
        [[nodiscard]] std::vector<std::shared_ptr<Motor>> motors() const;

    protected:
        //! Add a motor to the manager
        bool register_motor(Motor &motor);

        //! Update thread
        void runUpdate();

        std::atomic<bool> mTerminate = false;
        bool mVerbose = false;
        std::thread mUpdateThread;
        mutable std::mutex mMutex;

        std::map<std::string, std::shared_ptr<BusInterface>> mBusMap;
        std::vector<std::shared_ptr<Motor>> mMotors;

        friend class Motor;
    };

} // multivesc

#endif //MULTI_VESC_MANAGER_HH
