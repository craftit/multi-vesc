//
// Created by charles on 17/06/24.
//

#include <iostream>
#include "multivesc/Manager.hh"
#include "multivesc/BusCan.hh"
#include "multivesc/BusSerial.hh"

namespace multivesc
{
    using namespace std::chrono_literals;

    Manager::Manager(json config)
    {
        if(!configure(config)) {
            throw std::runtime_error("Failed to configure manager");
        }
    }

    Manager::~Manager()
    {
        stop();
    }

    bool Manager::configure(json config)
    {
        for(auto& item : config["buses"].items())
        {
            auto entry = item.value();
            auto busType = entry["type"].get<std::string>();
            std::shared_ptr<BusInterface> bus;
            if(busType == "can")
            {
                bus = std::make_shared<BusCan>(entry);
            } else if(busType == "serial")
            {
                bus = std::make_shared<BusSerial>(entry);
            }
            else
            {
                std::cout << "Unknown bus type " << busType << std::endl;
                return false;
            }
            mBusMap[item.key()] = bus;
        }

        start();

        // Configure motors
        for(auto& item : config["motors"].items())
        {
            const std::string& motorName = item.key();
            auto motor = std::make_shared<Motor>(motorName);
            motor->configure(*this, item.value());
        }

        return true;
    }

    bool Manager::openCan(const std::string& port)
    {
        auto bus = std::make_shared<BusCan>(port);
        bus->setVerbose(mVerbose);
        if(!bus->open()) {
            return false;
        }
        mBusMap["can"] = bus;
        start();
        return true;
    }

    std::shared_ptr<BusInterface> Manager::getBus(const std::string &name)
    {
        std::lock_guard lock(mMutex);
        auto iter = mBusMap.find(name);
        if(iter == mBusMap.end())
        {
            std::cerr << "Bus " << name << " not found" << std::endl;
            return {};
        }
        return iter->second;
    }


    bool Manager::start()
    {
        if(mUpdateThread.joinable())
        {
            return false;
        }
        // Start all buses
        {
            std::lock_guard lock(mMutex);
            for(auto& bus : mBusMap)
            {
                bus.second->open();
            }
        }
        mUpdateThread = std::thread(&Manager::runUpdate, this);
        return true;
    }


    bool Manager::stop()
    {
        mTerminate = true;
        // Shutdown all buses
        {
            std::lock_guard lock(mMutex);
            for(auto& bus : mBusMap)
            {
                bus.second->stop();
            }
        }

        // Close thread
        if(mUpdateThread.joinable())
        {
            mUpdateThread.join();
            return true;
        }
        return false;
    }

    std::vector<std::shared_ptr<Motor>> Manager::motors() const {
        std::lock_guard lock(mMutex);
        return mMotors;
    }

    bool Manager::register_motor(Motor &motor)
    {
        std::lock_guard lock(mMutex);
        // Check if the motor is already registered
        for(auto& m : mMotors)
        {
            if(m->name() == motor.name())
            {
                std::cerr << "Motor " << motor.name() << " already registered" << std::endl;
                return false;
            }
        }
        mMotors.push_back(motor.shared_from_this());
        return true;
    }

    void Manager::runUpdate()
    {
        while(!mTerminate)
        {
            std::this_thread::sleep_for(50ms);

            std::lock_guard lock(mMutex);
            for(auto& bus : mBusMap)
            {
                bus.second->update();
            }

        }

    }

    std::shared_ptr<Motor> Manager::getMotor(const std::string &name)
    {
        std::lock_guard lock(mMutex);
        for(auto &motor : mMotors)
        {
            if(motor && motor->name() == name)
            {
                return motor;
            }
        }
        return {};
    }
} // multivesc