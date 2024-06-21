//
// Created by charles on 17/06/24.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "cxxopts.hpp"
#include "multivesc/ComsCan.hh"
#include "multivesc/Manager.hh"

bool gTerminate = false;

int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;

    std::string deviceName = "can0";
    bool verbose = true;
    try {
        cxxopts::Options options(argv[0], " - command line options");

        options
            .positional_help("[optional args]")
            .show_positional_help();

        options.add_options()
            ("h,help", "Print help")
            ("d,device", "CAN device name", cxxopts::value<std::string>(deviceName)->default_value("can0"))
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

    std::cout << "deviceName: " << deviceName << std::endl;
    std::cout << "verbose: " << verbose << std::endl;

    multivesc::Manager manager;

    if(!manager.openCan(deviceName)) {
        std::cout << "Failed to open CAN device" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(1s);

    auto motor = manager.getMotor(0x58);

    std::cout << "Setting RPM to 5000" << std::endl;

    motor->setRPM(5000);

    while(!gTerminate) {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}