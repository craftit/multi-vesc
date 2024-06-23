//
// Created by charles on 17/06/24.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <fstream>
#include <csignal>
#include "cxxopts.hpp"
#include "multivesc/Manager.hh"

using json = nlohmann::json;

bool gTerminate = false;

void signalHandler(int signum)
{
    gTerminate = true;
}


int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;

    signal(SIGINT, signalHandler);

    std::string deviceName = "can0";
    std::string configFileName = "config.json";
    bool verbose = true;
    try {
        cxxopts::Options options(argv[0], " - command line options");

        options
            .positional_help("[optional args]")
            .show_positional_help();

        options.add_options()
            ("h,help", "Print help")
            ("d,device", "CAN device name", cxxopts::value<std::string>(deviceName)->default_value("can0"))
            ("c,config", "Config file", cxxopts::value<std::string>(configFileName)->default_value("config.json"))
            ("v,verbose", "Verbose output", cxxopts::value<bool>(verbose))
            ;

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

    } catch (const std::exception& e)
    {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }

    // Open config file
    std::ifstream configFile(configFileName);
    if(!configFile.is_open()) {
        std::cerr << "Failed to open config file" << std::endl;
        return 1;
    }

    // Load config file, throw exception if invalid but allow comments
    json config = json::parse(configFile, nullptr, true, true);

    std::cout << "deviceName: " << deviceName << std::endl;
    std::cout << "verbose: " << verbose << std::endl;

    multivesc::Manager manager;

    if(!manager.openCan(deviceName)) {
        std::cout << "Failed to open CAN device" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(1s);

    // Add motors and apply settings from config file
    for(auto& item : config["motors"].items())
    {
        const std::string& motorName = item.key();
        int motorId = item.value()["id"];
        std::cout << "Configuring motor '" << motorName << "'  (" << motorId << ")  to ";
        auto motorConfig = item.value();
        auto motor = manager.getMotor(motorId);
        if(motor == nullptr) {
            std::cout << "Failed to get motor" << std::endl;
            return 1;
        }
        motor->setName(motorName);
        bool controlSet = false;
        if(motorConfig.contains("rpm")) {
            float rpm = motorConfig["rpm"];
            motor->setRPM(rpm);
            controlSet = true;
            std::cout <<" RPM=" << rpm << " ";
        }
        if(motorConfig.contains("current")) {
            if(controlSet) {
                std::cout << "Both RPM and current set for motor " << motorName << std::endl;
                return 1;
            }
            float current = motorConfig["current"];
            motor->setCurrent(current);
            std::cout << " Current=" << current << " ";
            controlSet = true;
        }
        if(motorConfig.contains("duty")) {
            if(controlSet) {
                std::cout << "Both RPM and duty set for motor " << motorName << std::endl;
                return 1;
            }
            float duty = motorConfig["duty"];
            std::cout << " Duty=" << duty << " ";
            motor->setDuty(duty);
            controlSet = true;
        }
        std::cout << " " << std::endl;
    }

    while(!gTerminate) {
        std::this_thread::sleep_for(1s);
    }

    std::cout << "Exiting..." << std::endl;
    manager.stop();

    return 0;
}