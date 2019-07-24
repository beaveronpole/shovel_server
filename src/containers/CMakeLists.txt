cmake_minimum_required(VERSION 3.14)
project(containers)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(containers
        QueueBase.h
        RingQueue.cpp
        RingQueue.h
        RingQueueBlockDecorators.cpp
        RingQueueBlockDecorators.h
        RingQueues.h
        SizedArray.h QueueBase.cpp main.cpp)
