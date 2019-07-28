cmake_minimum_required(VERSION 3.14)
project(local)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(local
        LocalSocket.cpp
        LocalSocket.h
        main.cpp)
