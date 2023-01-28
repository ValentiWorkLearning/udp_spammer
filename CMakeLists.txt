cmake_minimum_required(VERSION 3.22)
project (udp_server_test C CXX ASM)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CROSS_BUILD_FOR_ARM ON)
if(CROSS_BUILD_FOR_ARM)
    set(CMAKE_CROSSCOMPILING TRUE)
endif()

add_executable(udp_server main.c)
