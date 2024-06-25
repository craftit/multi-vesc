
# Mutli-Vesc

API for driving multiple VESC motor controllers over CAN on linux.  It is assumed that the motors are configure with the
vesc tool.   Though the code currently only supports CAN, serial support could be added in the future.

This code is for use with controllers based on the VESC project: https://vesc-project.com/

It has been tested with [FLIPSKY Mini FSESC6.7](https://flipsky.net/collections/v6-series/products/flipsky-mini-fsesc6-7-pro-70a-base-on-vesc6-6-with-aluminum-anodized-heat-sink)

# Installation

Building on Ubuntu 24.04:

    sudo apt install nlohmann-json3-dev cmake ninja-build build-essential can-utils pybind11-dev python3-dev pybind11-json-dev
    mkdir build
    cd build
    cmake .. 
    make

# Protocol

The VESC [CAN protocol](https://github.com/vedderb/bldc/blob/master/documentation/comm_can.md) is used to communicate 
with the VESC controllers.

# License

This code is licensed under the MIT license unless otherwise noted.

The follow files have their own license:

    include/multivesc/crc.hh    

# Configuring CAN

This has been tested with [DSD TECH USB to CAN bus adapter](https://www.amazon.co.uk/dp/B0BQ5G3KLR) 

The VESC default can bus speed is 500k bit rate.

If there are no other can devices in the system it will appear as 'can0',  this interface can be configured with 
the following commands:

    ip link set can0 type can bitrate 500000
    ip link set up can0


# Running the code

The code can be run with the following command:

    ./multi_vesc -c ./config/example.json -d can0

Following is an example of the json  config file should be in the following format:

    {
        "buses": {
            "can0": {
              "type": "can",
              "device": "can0"
            }
        },
        "motors": {
            "motor1": {
              "id": 88,
              "bus":"can0",
              "enable": true,
              "controlMode": "rpm",
              "rpm": 10000,
              "numPoles": 14,
              "maxRPMAcceleration": 1000,
              "minRPM": 2000,
              "startDelay": 0.2
            },
            "motor2": {
                "id": 122
                "bus":"can0",
                "enable": false,
            }
        }
    }

For each motor the following parameters can be set:

* id: The CAN id of the motor
* enable: If the motor is enabled
* controlMode: The control mode of the motor
* One of the following types of drive values, depending on the 'controlMode':
  * rpm: The target RPM of the motor
  * current: The target current of the motor in Amps
  * duty: The target duty cycle of the motor. 0.0 to 1.0
* maxRPMAcceleration: The maximum RPM acceleration of the motor
* minRPM: The minimum RPM of the motor
* startDelay: The delay in seconds before the motor starts

