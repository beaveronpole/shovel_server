cmake_minimum_required(VERSION 3.14)
project(parsers)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(parsers
        ParserBase.cpp
        ParserBase.h
        ParserExample.cpp
        ParserExample.h
        main.cpp)
