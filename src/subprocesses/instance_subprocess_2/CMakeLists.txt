cmake_minimum_required(VERSION 3.13)
project(instance_subprocess_2)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

string(COMPARE EQUAL "${MAIN_PROJECT_DIR}" "" result)
IF(result)
    SET(MAIN_PROJECT_DIR ../../)
ENDIF()

include_directories(
        ${MAIN_PROJECT_DIR}/
        ${MAIN_PROJECT_DIR}/containers
        ${MAIN_PROJECT_DIR}/connections
        ${MAIN_PROJECT_DIR}/connections/local
        ${MAIN_PROJECT_DIR}/connections/network
        ${MAIN_PROJECT_DIR}/connections/workers
        ${MAIN_PROJECT_DIR}/parsers
        ${MAIN_PROJECT_DIR}/subprocesses
        ${SHARE_PATH}
)

add_executable(instance_subprocess_2
        main.cpp
        ${MAIN_PROJECT_DIR}/connections/local/LocalSocket.cpp ${MAIN_PROJECT_DIR}/connections/local/LocalSocket.h
        ${MAIN_PROJECT_DIR}/containers/RingQueue.cpp ${MAIN_PROJECT_DIR}/containers/RingQueue.h
        ${MAIN_PROJECT_DIR}/containers/QueueBase.cpp ${MAIN_PROJECT_DIR}/containers/QueueBase.h
        ${MAIN_PROJECT_DIR}/containers/RingQueueBlockDecorators.cpp ${MAIN_PROJECT_DIR}/containers/RingQueueBlockDecorators.h
        ${MAIN_PROJECT_DIR}/connections/workers/ConnectionWorker.cpp ${MAIN_PROJECT_DIR}/connections/workers/ConnectionWorker.h
        ${MAIN_PROJECT_DIR}/parsers/ParserBase.cpp ${MAIN_PROJECT_DIR}/parsers/ParserBase.h
        ${MAIN_PROJECT_DIR}/parsers/ParserExample.cpp ${MAIN_PROJECT_DIR}/parsers/ParserExample.h
        ${MAIN_PROJECT_DIR}/connections/local/LocalSocket.cpp ${MAIN_PROJECT_DIR}/connections/local/LocalSocket.h
        ${MAIN_PROJECT_DIR}/connections/workers/LocalSocketWorker.cpp ${MAIN_PROJECT_DIR}/connections/workers/LocalSocketWorker.h
        ${MAIN_PROJECT_DIR}/containers/SizedArray.h ${MAIN_PROJECT_DIR}/containers/RingQueues.h
        instance_subprocess_2.cpp instance_subprocess_2.h
        ${MAIN_PROJECT_DIR}/subprocesses/abstract_subprocess/AbstractSubprocess.cpp ${MAIN_PROJECT_DIR}/subprocesses/abstract_subprocess/AbstractSubprocess.h)

target_link_libraries(
        instance_subprocess_2
        pthread)

