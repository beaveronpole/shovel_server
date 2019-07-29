#ifndef SHOVEL_RINGQUEUE_H
#define SHOVEL_RINGQUEUE_H

#include <cstdint>
#include <atomic>
#include <vector>
#include <cstring>
#include <algorithm>
#include <functional>
#include <limits>
#include <unistd.h>
#include <memory>
#include <iostream>

#include "QueueBase.h"


class RingQueue : public QueueBase {

public:
    explicit RingQueue();
    explicit RingQueue(std::uint32_t count, std::uint32_t cell_size = std::numeric_limits<uint16_t>::max());

    RingQueue(const RingQueue& other) = delete;
    RingQueue& operator= ( const RingQueue&) = delete;
    RingQueue(const RingQueue&& other) = delete;
    RingQueue& operator= ( const RingQueue&&) = delete;

    uint16_t put(const uint8_t *buf, uint16_t size) override;
    uint16_t get(std::uint8_t *buf, uint16_t size) override;
    uint16_t peek(std::uint8_t *buf, uint16_t size) override;

    ~RingQueue() override;

protected:
    uint32_t m_index_in;
    uint32_t m_index_out;

    inline void reset_status() {m_status = QUEUEBASE_STATUS::STATUS_QUEUEBASE_OK;}

    struct bag{
        inline explicit bag(std::uint32_t input_init_cell_size = std::numeric_limits<uint16_t>::max()):
                             has_data(false),
                             data_size(0),
                             init_cell_size(input_init_cell_size){
                data = std::make_unique<std::vector<uint8_t> >(init_cell_size);
                data->resize(init_cell_size);
        }
        inline uint32_t put(const uint8_t* in_data, uint32_t put_data_size){
            if (data->size() < put_data_size)
                data->resize(put_data_size);
            memcpy(this->data->data(), in_data, put_data_size);
            this->has_data = true;
            this->data_size = put_data_size;
            return put_data_size;
        }
        inline uint32_t get(uint8_t* out_data, uint32_t size){
            if (!this->has_data) return 0;
            uint32_t read_size = std::min(size, this->data_size);
            if (size < this->data_size && size > 0){
                std::cout << "ask for get from data bag size = " << size << " less then " << this->data_size << " some data may be lost"<<std::endl;
            }
            memcpy(out_data, this->data->data(), read_size);
            this->has_data = false;
            this->data_size = 0;
            return read_size;
        }
        inline int peek(uint8_t* buf, uint32_t size){
            if (!this->has_data) return 0;
            uint32_t read_size = std::min(size, this->data_size);
            if (size < this->data_size){
                std::cout << "ask for peek from data bag size = " << size << " less then " << this->data_size << " some data may be lost" << std::endl;
            }
            memcpy(buf, this->data->data(), read_size);
            return read_size;
        }
        std::unique_ptr< std::vector<uint8_t> > data;
        std::uint32_t init_cell_size;
        std::uint32_t data_size;
        bool has_data;
        inline ~bag(){
        }
    };

    std::unique_ptr< std::vector< std::unique_ptr<bag> > >  m_ring_vector;
    std::atomic<uint32_t> m_total_count;
};


#endif //SHOVEL_RINGQUEUE_H
