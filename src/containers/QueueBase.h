#ifndef SHOVEL_QUEUEBASE_H
#define SHOVEL_QUEUEBASE_H

#include <cstdint>
#include <atomic>

class QueueBase  {

public:

    QueueBase();
    enum class QUEUEBASE_STATUS : uint32_t {
        STATUS_QUEUEBASE_OK,
        STATUS_QUEUEBASE_FULL,
        STATUS_QUEUEBASE_EMPTY
    };

    /// simple put data to queue.
    virtual uint16_t put(const uint8_t *buf, uint16_t size) = 0;

    /// if size in get returns 0 it means error
    virtual uint16_t get(std::uint8_t *buf, uint16_t size) = 0;

    /// peek let us look at data in PioG without taking it
    virtual uint16_t peek(std::uint8_t *buf, uint16_t size) = 0;

    virtual QUEUEBASE_STATUS get_status();
    virtual uint32_t size();
    virtual uint32_t capacity();
    virtual ~QueueBase();

protected:
    std::atomic_uint32_t m_total_count;
    std::atomic_uint32_t m_size;
    QUEUEBASE_STATUS m_status;

};


#endif //SHOVEL_QUEUEBASE_H
