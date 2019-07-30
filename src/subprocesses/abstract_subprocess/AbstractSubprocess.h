#ifndef SHOVEL_ABSTRACTSUBPROCESS_H
#define SHOVEL_ABSTRACTSUBPROCESS_H

#include <string>
#include <memory>
#include <chrono>

#include "../../containers/RingQueues.h"
#include "../../connections/workers/LocalSocketWorker.h"


class AbstractSubprocess {
public:
    const uint32_t QUEUE_COUNT_BAGS = 128;

    AbstractSubprocess(std::string &&path_input,
                       std::string &&path_output,
                       std::string &&subprocess_name);
    void start();

    AbstractSubprocess(const AbstractSubprocess& other) = delete;
    AbstractSubprocess& operator= (const AbstractSubprocess&) = delete;
    AbstractSubprocess(const AbstractSubprocess&& other) = delete;
    AbstractSubprocess& operator= (const AbstractSubprocess&&) = delete;

protected:
    std::unique_ptr<LocalSocketWorker> m_local_worker;
    void loop();
    virtual void datagram_processor(std::shared_ptr<SizedArray<uint8_t, std::numeric_limits<uint16_t>::max()> >) = 0;

    std::string m_subprocess_name;
};


#endif //SHOVEL_ABSTRACTSUBPROCESS_H
