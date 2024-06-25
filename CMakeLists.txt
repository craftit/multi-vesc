
cmake_minimum_required(VERSION 3.13)
project(multivesc)

find_package(nlohmann_json REQUIRED)
find_package(pybind11 CONFIG)

add_library(multivesc STATIC
        src/BusInterface.cc include/multivesc/BusInterface.hh
        src/Manager.cc include/multivesc/Manager.hh
        src/BusSerial.cc include/multivesc/BusSerial.hh
        src/Motor.cc include/multivesc/Motor.hh
        src/BusCan.cc include/multivesc/BusCan.hh
)

# Make code relocatable
set_target_properties(multivesc PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(multivesc PUBLIC cxx_std_17)
target_include_directories(multivesc PUBLIC include)
target_link_libraries(multivesc PUBLIC nlohmann_json::nlohmann_json)

# Build the executable
add_executable(vesc_run
        src/run_vesc.cc
)
target_link_libraries(vesc_run PUBLIC multivesc )


pybind11_add_module(pymultivesc src/python.cc)
target_link_libraries(pymultivesc PRIVATE multivesc)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(pymultivesc
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
