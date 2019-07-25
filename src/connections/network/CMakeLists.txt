cmake_minimum_required(VERSION 3.14)
project(network)

set(CMAKE_CXX_STANDARD 14)

SET(CONTAINERS_PATH "./../../")

include_directories(.)

add_executable(network
        TCPClient.cpp
        TCPClient.h
        TCPServer.cpp
        TCPServer.h
        TCPSocket.cpp
        TCPSocket.h
        ${CONTAINERS_PATH}/containers/QueueBase.cpp
        ${CONTAINERS_PATH}/containers/RingQueue.cpp
        ${CONTAINERS_PATH}/containers/RingQueueBlockDecorators.cpp
        main.cpp)

target_link_libraries(network pthread)
