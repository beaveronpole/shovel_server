cmake_minimum_required(VERSION 3.14)
project(abstract_subprocess)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(abstract_subprocess
        AbstractSubprocess.cpp
        AbstractSubprocess.h
        ../../connections/workers/ConnectionWorker.h ../../connections/workers/ConnectionWorker.cpp
        ../../connections/workers/LocalSocketWorker.h ../../connections/workers/LocalSocketWorker.cpp
        ../../connections/local/LocalSocket.h ../../connections/local/LocalSocket.cpp
        ../../containers/QueueBase.h ../../containers/QueueBase.cpp
        ../../containers/RingQueue.h ../../containers/RingQueue.cpp
        ../../containers/RingQueueBlockDecorators.h ../../containers/RingQueueBlockDecorators.cpp

        main.cpp)

target_link_libraries(abstract_subprocess pthread)
