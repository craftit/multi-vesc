//
// Created by charles on 17/06/24.
//

#include <iostream>
#include "cxxopts.hpp"
#include "multivesc/ComsCan.hh"

int main(int argc, char *argv[])
{
    std::string deviceName = "can0";
    bool verbose = false;
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



    return 0;
}