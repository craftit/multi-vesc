//
// Created by charles on 17/06/24.
//

#include <iostream>
#include "multivesc/Manager.hh"
#include "multivesc/ComsCan.hh"

namespace multivesc
{

    Manager::Manager(std::shared_ptr<ComsInterface> coms)
     : mPrimaryComs(std::move(coms))
    {

    }

    Manager::~Manager()
    {
        stop();
    }

    bool Manager::openCan(const std::string& port)
    {
        mPrimaryComs = std::make_shared<ComsCan>(port);
        if(!mPrimaryComs->open()) {
            return false;
        }
        start();
        return true;
    }


    bool Manager::start()
    {
        if(mUpdateThread.joinable())
        {
            return false;
        }
        mUpdateThread = std::thread(&Manager::runUpdate, this);
        return true;
    }


    bool Manager::stop()
    {
        mTerminate = true;
        if(mUpdateThread.joinable())
        {
            mUpdateThread.join();
            return true;
        }
        return false;
    }

    void Manager::runUpdate()
    {
        while(!mTerminate)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::lock_guard lock(mMutex);
            for(auto &motor : mMotors)
            {
                if(!motor)
                {
                    continue;
                }
                motor->update();
            }
        }

    }

    std::shared_ptr<Motor> Manager::getMotor(uint8_t id)
    {
        std::lock_guard lock(mMutex);
        if(id >= mMotors.size())
        {
            std::cerr << "Motor id out of range" << std::endl;
            return {};
        }
        if(!mMotors[id])
        {
            if(!mPrimaryComs)
            {
                std::cerr << "No primary coms set" << std::endl;
                return {};
            }
            mMotors[id] = std::make_shared<Motor>(mPrimaryComs, id);
        }
        return mMotors[id];
    }


} // multivesc