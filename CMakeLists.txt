
cmake_minimum_required(VERSION 3.13)
project(MutliVesc)

add_library(mutlivesc STATIC
        src/ComsInterface.cc include/multivesc/ComsInterface.hh
        src/Manager.cc include/multivesc/Manager.hh
        src/ComsSerial.cc include/multivesc/ComsSerial.hh
        src/Motor.cc include/multivesc/Motor.hh
        src/ComsCan.cc include/multivesc/ComsCan.hh
)

target_compile_features(mutlivesc PUBLIC cxx_std_17)

target_include_directories(mutlivesc PUBLIC include)

add_executable(vesc_run
        src/run_vesc.cc
)

target_link_libraries(vesc_run mutlivesc)