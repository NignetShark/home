cmake_minimum_required(VERSION 3.17)
project(home)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(COMPILER_PATH /home/paul/.git/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin)

option(CROSS_COMPILE_BUILD "Use cross-compilation" ON)

if(CROSS_COMPILE_BUILD)
    SET(CMAKE_SYSTEM_NAME Linux)
    SET(CMAKE_C_COMPILER ${COMPILER_PATH}/arm-linux-gnueabihf-gcc)
    SET(CMAKE_CXX_COMPILER ${COMPILER_PATH}/arm-linux-gnueabihf-g++)
endif(CROSS_COMPILE_BUILD)

add_executable(alexa
        src/test.cpp
        src/alexa.cpp
        src/exceptions/alexa_exception.cpp
        include/exceptions/alexa_exception.hpp
        src/exceptions/alexa_policy_exception.cpp
        include/exceptions/alexa_policy_exception.hpp
        src/exceptions/alexa_processing_exception.cpp
        include/exceptions/alexa_processing_exception.hpp
        src/alexa_server.cpp include/alexa_server.hpp)

target_include_directories(alexa PRIVATE include ../lib/include)
target_link_libraries(alexa PRIVATE Threads::Threads)

if(CROSS_COMPILE_BUILD)
    add_custom_command(TARGET alexa POST_BUILD COMMAND scp $<TARGET_FILE:alexa> pi@192.168.1.12:~/)
endif(CROSS_COMPILE_BUILD)