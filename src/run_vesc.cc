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

bool configureMotor(multivesc::Manager &manager, const std::string& motorName, const json& motorConfig)
{
    int motorId = motorConfig["id"];
    std::cout << "Configuring motor '" << motorName << "'  (" << motorId << ")  to ";
    auto motor = manager.getMotor(motorId);
    if(motor == nullptr) {
        std::cout << "Failed to get motor" << std::endl;
        return false;
    }
    motor->setName(motorName);
    // Setup maximum acceleration
    if(motorConfig.contains("maxRPMAcceleration")) {
        float maxRPMAcceleration = motorConfig["maxRPMAcceleration"];
        motor->setMaxRPMAcceleration(maxRPMAcceleration);
        std::cout << " MaxRPMAcceleration=" << maxRPMAcceleration << " ";
    }
    // Setup minimum motor RPM
    if(motorConfig.contains("minRPM")) {
        float minRPM = motorConfig["minRPM"];
        motor->setMinRPM(minRPM);
        std::cout << " MinRPM=" << minRPM << " ";
    }
    if(!motorConfig.value("enable", true)) {
        motor->setRPM(0.0f);
        std::cout << " Disabled ";
        return true;
    }
    // Get the delay before we should start the motor
    if(motorConfig.contains("startDelay")) {
        float startDelay = motorConfig.value("startDelay",0.1f);
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(startDelay*1000)));
    }

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
            return false;
        }
        float current = motorConfig["current"];
        motor->setCurrent(current);
        std::cout << " Current=" << current << " ";
        controlSet = true;
    }
    if(motorConfig.contains("duty")) {
        if(controlSet) {
            std::cout << "Both RPM and duty set for motor " << motorName << std::endl;
            return false;
        }
        float duty = motorConfig["duty"];
        std::cout << " Duty=" << duty << " ";
        motor->setDuty(duty);
        controlSet = true;
    }
    std::cout << " " << std::endl;

    return true;
}


int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;

    signal(SIGINT, signalHandler);

    std::string deviceName = "can0";
    std::string configFileName = "config.json";
    bool verbose = false;
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

    manager.setVerbose(verbose);

    if(!manager.openCan(deviceName)) {
        std::cout << "Failed to open CAN device" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(1s);

    // Add motors and apply settings from config file
    for(auto& item : config["motors"].items())
    {
        const std::string& motorName = item.key();
        if(!configureMotor(manager, motorName, item.value())) {
            std::cout << "Failed to configure motor " << motorName << std::endl;
            return 1;
        }
    }

    while(!gTerminate) {
        std::this_thread::sleep_for(1s);
    }

    std::cout << "Exiting..." << std::endl;
    manager.stop();

    return 0;
}