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

    if(!manager.configure(config)) {
        std::cerr << "Failed to configure system" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(1s);

    while(!gTerminate) {
        std::this_thread::sleep_for(1s);
    }

    std::cout << "Exiting..." << std::endl;
    manager.stop();

    return 0;
}