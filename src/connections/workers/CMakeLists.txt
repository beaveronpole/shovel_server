cmake_minimum_required(VERSION 3.14)
project(workers)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(workers
        ConnectionWorker.cpp
        ConnectionWorker.h
        LocalSocketWorker.cpp
        LocalSocketWorker.h
        TCPSocketWorker.cpp
        TCPSocketWorker.h
        ../../connections/local/LocalSocket.h ../../connections/local/LocalSocket.cpp
        ../../connections/network/TCPSocket.h ../../connections/network/TCPSocket.cpp
        ../../containers/QueueBase.h ../../containers/QueueBase.cpp
        main.cpp)

target_link_libraries(workers
        pthread)
