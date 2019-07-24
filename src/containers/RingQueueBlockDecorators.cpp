#include "RingQueueBlockDecorators.h"


RingQueueBlockBaseDecorator::RingQueueBlockBaseDecorator(std::shared_ptr<QueueBase> inner) {
    m_wrappee = inner;
    auto inner_decorator = dynamic_pointer_cast<RingQueueBlockBaseDecorator>(inner);
    if (m_mutex == nullptr){ /// if our object doesnt have "parent"- we create mutex
        m_mutex = make_shared<mutex>();
    }
    if (m_condition_full == nullptr){ /// if our object doesnt have "parent"- we create condition variable for full
        m_condition_full = make_shared<condition_variable>();
    }
    if (m_condition_empty == nullptr){ /// if our object doesnt have "parent"- we create condition variable for empty
        m_condition_empty = make_shared<condition_variable>();
    }
    if (inner_decorator != nullptr){ /// check if our child is DECORATOR
        /// if out child is DECORATOR too- we set it SYNC things
        /// so? we drill down SYNCs things if our child is DECORATOR
        inner_decorator->set_syncs(m_mutex, m_condition_full, m_condition_empty);
    } else { //another RingQueue
        //maybe do something here
    }
}

uint16_t RingQueueBlockBaseDecorator::put(const uint8_t *buf, uint16_t size) {
    return m_wrappee->put(buf, size);
}

uint16_t RingQueueBlockBaseDecorator::get(std::uint8_t *buf, uint16_t size) {
    return m_wrappee->get(buf, size);
}

uint16_t RingQueueBlockBaseDecorator::peek(std::uint8_t *buf, uint16_t size) {
    return m_wrappee->peek(buf, size);
}

void RingQueueBlockBaseDecorator::wait_function_put(std::unique_lock<std::mutex> &var_lock) {
    while (m_wrappee->size() >= m_wrappee->capacity())
        m_condition_full->wait(var_lock);
}

void RingQueueBlockBaseDecorator::wait_function_get(std::unique_lock<std::mutex> &var_lock) {
    while (m_wrappee->size() <= 0)
        m_condition_empty->wait(var_lock);
}

void RingQueueBlockBaseDecorator::wait_function_put(std::unique_lock<std::mutex> &var_lock, uint32_t wait_msecs) {
    if (m_wrappee->size() >= m_wrappee->capacity())
        m_condition_full->wait_for(var_lock, std::chrono::milliseconds(wait_msecs));
}

void RingQueueBlockBaseDecorator::wait_function_get(std::unique_lock<std::mutex> &var_lock, uint32_t wait_msecs) {
    if (m_wrappee->size() <= 0)
        m_condition_empty->wait_for(var_lock, std::chrono::milliseconds(wait_msecs));
}

RingQueueBlockBaseDecorator::~RingQueueBlockBaseDecorator() {

}

uint32_t RingQueueBlockBaseDecorator::capacity() {
    return m_wrappee->capacity();
}

uint32_t RingQueueBlockBaseDecorator::size() {
    return m_wrappee->size();
}

QueueBase::QUEUEBASE_STATUS RingQueueBlockBaseDecorator::get_status() {
    return m_wrappee->get_status();
}

//-------------------------------------------------------------------------

RQ_BlockWriteDecorator::RQ_BlockWriteDecorator(std::shared_ptr<QueueBase> inner) :
        RingQueueBlockBaseDecorator(inner) {

}

uint16_t RQ_BlockWriteDecorator::put(const uint8_t *buf, uint16_t size) {
    std::unique_lock<std::mutex> cond_lock(*m_mutex);
    wait_function_put(cond_lock);
    auto res = m_wrappee->put(buf, size);
    m_condition_empty->notify_one();
    return res;
}

//------------------------------------------------------------------------


RQ_BlockReadDecorator::RQ_BlockReadDecorator(std::shared_ptr<QueueBase> inner) :
        RingQueueBlockBaseDecorator(inner) {

}

uint16_t RQ_BlockReadDecorator::get(std::uint8_t *buf, uint16_t size) {
    std::unique_lock<std::mutex> cond_lock(*m_mutex);
    wait_function_get(cond_lock);
    auto res = m_wrappee->get(buf, size);
    m_condition_full->notify_one();
    return res;
}

uint16_t RQ_BlockReadDecorator::peek(std::uint8_t *buf, uint16_t size) {
    std::unique_lock<std::mutex> cond_lock(*m_mutex);
    wait_function_get(cond_lock);
    auto res = m_wrappee->peek(buf, size);
    return res;
}

//-------------------------------------------------------------------------



RQ_BlockWriteTimedDecorator::RQ_BlockWriteTimedDecorator(std::shared_ptr<QueueBase> inner,
        uint32_t timeout_ms):
            RingQueueBlockBaseDecorator(inner),
            m_wait_time_ms(timeout_ms) {

}

uint16_t RQ_BlockWriteTimedDecorator::put(const uint8_t *buf, uint16_t size) {
    std::unique_lock<std::mutex> cond_lock(*m_mutex);
    wait_function_put(cond_lock, m_wait_time_ms);
    auto res = m_wrappee->put(buf, size);
    m_condition_empty->notify_one();
    return res;
}

//-------------------------------------------------------------------------
RQ_BlockReadTimedDecorator::RQ_BlockReadTimedDecorator(std::shared_ptr<QueueBase> inner, uint32_t timeout_ms):
        RingQueueBlockBaseDecorator(inner),
        m_wait_time_ms(timeout_ms) {

}

uint16_t RQ_BlockReadTimedDecorator::get(std::uint8_t *buf, uint16_t size) {
    std::unique_lock<std::mutex> cond_lock(*m_mutex);
    wait_function_get(cond_lock, m_wait_time_ms);
    auto res = m_wrappee->get(buf, size);
    m_condition_full->notify_one();
    return res;
}

uint16_t RQ_BlockReadTimedDecorator::peek(std::uint8_t *buf, uint16_t size) {
    std::unique_lock<std::mutex> cond_lock(*m_mutex);
    wait_function_get(cond_lock, m_wait_time_ms);
    auto res = m_wrappee->peek(buf, size);
    return res;
}

//-------------------------------------------------------------------------

RQ_NoBlockWriteDecorator::RQ_NoBlockWriteDecorator(std::shared_ptr<QueueBase> inner):
        RingQueueBlockBaseDecorator(inner) {

}

uint16_t RQ_NoBlockWriteDecorator::put(const uint8_t *buf, uint16_t size) {
    auto res = m_wrappee->put(buf, size);
    m_condition_empty->notify_one();
    return res;
}

//------------------------------------------------------------------------

RQ_NoBlockReadDecorator::RQ_NoBlockReadDecorator(std::shared_ptr<QueueBase> inner) :
        RingQueueBlockBaseDecorator(inner) {

}

uint16_t RQ_NoBlockReadDecorator::get(std::uint8_t *buf, uint16_t size) {
    auto res = m_wrappee->get(buf, size);
    m_condition_full->notify_one();
    return res;
}
