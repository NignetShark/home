cmake_minimum_required(VERSION 3.17)
project(home)

set(CMAKE_CXX_STANDARD 14)

add_executable(yatu
        src/test.cpp
        src/device.cpp
        ../lib/src/aes/aes.c
        ../lib/include/crc32/crc32.hpp
        ../lib/src/crc32/crc32.cpp src/exception/device_exception.cpp include/exception/device_exception.hpp src/light_bulb.cpp include/light_bulb.hpp)

target_include_directories(yatu PRIVATE include ../lib/include)