#include "RingQueue.h"

RingQueue::RingQueue():
        RingQueue(20,  std::numeric_limits<uint16_t>::max())
{
}

RingQueue::RingQueue(uint32_t count, uint32_t cell_size):
        m_index_in(0),
        m_index_out(0),
        m_total_count(count)
        {
    reset_status();
    m_ring_vector = std::make_unique<std::vector< std::unique_ptr<bag> > >();
    for (std::uint32_t i = 0; i < m_total_count; ++i){
        m_ring_vector->emplace_back( std::make_unique<bag>(cell_size) );
    }
    m_total_count = count;
}

uint16_t RingQueue::put(const uint8_t *buf, uint16_t size) {
    reset_status();
    if (m_size < m_total_count ){
        uint32_t put_size = m_ring_vector->at(m_index_in)->put(buf, size);
        m_index_in++;
        if (m_index_in == m_total_count)
            m_index_in = 0;
        m_size++;
        return static_cast<uint16_t>(put_size);
    }
    m_status = QUEUEBASE_STATUS::STATUS_QUEUEBASE_FULL;
    return 0;
}

uint16_t RingQueue::get(std::uint8_t *buf, uint16_t size) {
    reset_status();
    if (m_size){
        int32_t got_size = m_ring_vector->at(m_index_out)->get(buf, size);
        m_index_out++;
        if (m_index_out == m_total_count)
            m_index_out = 0;
        m_size--;
        return static_cast<uint16_t>(got_size);
    }
    m_status = QUEUEBASE_STATUS::STATUS_QUEUEBASE_EMPTY;
    return 0;
}

uint16_t RingQueue::peek(std::uint8_t *buf, uint16_t size) {
    reset_status();
    if (m_size){
        int32_t got_size = m_ring_vector->at(m_index_out)->peek(buf, size);
        return static_cast<uint16_t>(got_size);
    }
    m_status = QUEUEBASE_STATUS::STATUS_QUEUEBASE_EMPTY;
    return 0;
}

RingQueue::~RingQueue() {
    m_ring_vector->clear();
}



