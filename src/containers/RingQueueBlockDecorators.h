#ifndef SHOVEL_RINGQUEUEBLOCKDECORATORS_H
#define SHOVEL_RINGQUEUEBLOCKDECORATORS_H

#include <mutex>
#include <condition_variable>
#include <memory>

#include "QueueBase.h"

using namespace std;

class RingQueueBlockBaseDecorator: public QueueBase {
public:
    explicit RingQueueBlockBaseDecorator(std::shared_ptr<QueueBase> inner);
    /**if inner == QueueBase:....
    //if inner == RingQueueBlockBaseDecorator...
        create mutex and conds
        upward mutex and conds*/


    uint16_t put(const uint8_t *buf, uint16_t size) override;
    uint16_t get(std::uint8_t *buf, uint16_t size) override;
    uint16_t peek(std::uint8_t *buf, uint16_t size) override;

    uint32_t capacity() override;

    uint32_t size() override;

    QUEUEBASE_STATUS get_status() override;


    inline void set_syncs(shared_ptr<mutex> mutex,
                          shared_ptr<condition_variable> full_cond,
                          shared_ptr<condition_variable> empty_cond){
        m_mutex = mutex;
        m_condition_empty = empty_cond;
        m_condition_full = full_cond;
    };
    ~RingQueueBlockBaseDecorator() override;

    RingQueueBlockBaseDecorator(const RingQueueBlockBaseDecorator& other) = delete;
    RingQueueBlockBaseDecorator& operator= ( const RingQueueBlockBaseDecorator&) = delete;
    RingQueueBlockBaseDecorator(const RingQueueBlockBaseDecorator&& other) = delete;
    RingQueueBlockBaseDecorator& operator= ( const RingQueueBlockBaseDecorator&&) = delete;

protected:
    std::shared_ptr<QueueBase> m_wrappee;

    std::shared_ptr<std::mutex> m_mutex;
    std::shared_ptr<std::condition_variable> m_condition_empty;
    std::shared_ptr<std::condition_variable> m_condition_full;

    virtual void wait_function_put(std::unique_lock<std::mutex> &);
    virtual void wait_function_get(std::unique_lock<std::mutex> &);
    virtual void wait_function_put(std::unique_lock<std::mutex> &, uint32_t wait_msecs);
    virtual void wait_function_get(std::unique_lock<std::mutex> &, uint32_t wait_msecs);
};
///-------------------------

class RQ_BlockWriteDecorator: public RingQueueBlockBaseDecorator
{
public:
    explicit RQ_BlockWriteDecorator(std::shared_ptr<QueueBase> inner);
    uint16_t put(const uint8_t *buf, uint16_t size) override;
};
///-------------------------

class RQ_BlockReadDecorator: public RingQueueBlockBaseDecorator
{
public:
    explicit RQ_BlockReadDecorator(std::shared_ptr<QueueBase> inner);
    uint16_t get(std::uint8_t *buf, uint16_t size) override;
    uint16_t peek(std::uint8_t *buf, uint16_t size) override;
};
///-------------------------

class RQ_BlockWriteTimedDecorator: public RingQueueBlockBaseDecorator
{
public:
    RQ_BlockWriteTimedDecorator(std::shared_ptr<QueueBase> inner, uint32_t timeout_ms);
    uint16_t put(const uint8_t *buf, uint16_t size) override;

protected:
    uint32_t m_wait_time_ms;
};
///-------------------------

class RQ_BlockReadTimedDecorator: public RingQueueBlockBaseDecorator
{
public:
    RQ_BlockReadTimedDecorator(std::shared_ptr<QueueBase> inner, uint32_t timeout_ms);
    uint16_t get(std::uint8_t *buf, uint16_t size) override;
    uint16_t peek(std::uint8_t *buf, uint16_t size) override;

protected:
    uint32_t m_wait_time_ms;
};
///-------------------------

class RQ_NoBlockWriteDecorator: public RingQueueBlockBaseDecorator
{
public:
    RQ_NoBlockWriteDecorator(std::shared_ptr<QueueBase> inner);
    uint16_t put(const uint8_t *buf, uint16_t size) override;

protected:
    uint32_t m_wait_time_ms;
};
///-------------------------

class RQ_NoBlockReadDecorator: public RingQueueBlockBaseDecorator
{
public:
    explicit RQ_NoBlockReadDecorator(std::shared_ptr<QueueBase> inner);
    uint16_t get(std::uint8_t *buf, uint16_t size) override;
};
///-------------------------




#endif //SHOVEL_RINGQUEUEBLOCKDECORATORS_H
