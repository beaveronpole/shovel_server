#ifndef SHOVEL_CONNECTIONWORKER_H
#define SHOVEL_CONNECTIONWORKER_H

#include <thread>
#include <memory>
#include <functional>
#include <atomic>
#include <future>
#include <chrono>
#include <iostream>
#include <array>
#include <atomic>

#include "../../containers/QueueBase.h"
#include "../../parsers/ParserBase.h"
#include "../../containers/SizedArray.h"


class ConnectionWorker;

using WorkerFunction = std::function<void(QueueBase*)>;
using WorkerPostMortem = std::function<void(ConnectionWorker*)>;

constexpr uint16_t connection_worker_arr_size = std::numeric_limits<uint16_t>::max();

class ConnectionWorker {

public:
    ConnectionWorker(std::shared_ptr<QueueBase> queue_for_reading_data_from_source,
                     std::shared_ptr<QueueBase> queue_for_writing_data_to_source);

    void set_parser(std::unique_ptr<ParserBase>&& parser);
    inline void set_postmortem_callback(WorkerPostMortem cb){ m_postmortem_callback = cb;}

    inline std::shared_ptr<QueueBase> get_queue_for_reading_data_from_source() {return m_queue_for_reading_data_from_source;}
    inline std::shared_ptr<QueueBase> get_queue_for_writing_data_to_source() {return m_queue_for_writing_data_to_source;}
    inline void set_queue_for_writing_data_to_source(std::shared_ptr<QueueBase> pg){ m_queue_for_writing_data_to_source = pg;}

    void start();
    virtual void stop();
    inline bool is_stopped() {return m_stopped;}
    virtual ~ConnectionWorker();
    inline uint64_t id() {return m_worker_id;}

    ConnectionWorker(const ConnectionWorker& other) = delete;
    ConnectionWorker& operator= ( const ConnectionWorker&) = delete;
    ConnectionWorker(const ConnectionWorker&& other) = delete;
    ConnectionWorker& operator= ( const ConnectionWorker&&) = delete;

    struct connection_statistics{
        inline void add_sent(uint64_t count){
            total_sent_bytes+=count;
            chunk_sent_bytes+=count;
        }
        inline void add_recv(uint64_t count){
            total_recv_bytes+=count;
            chunk_recv_bytes+=count;
        }
        uint64_t total_sent_bytes = 0;
        uint64_t total_recv_bytes = 0;
        uint64_t chunk_sent_bytes = 0;
        uint64_t chunk_recv_bytes = 0;
        uint64_t last_check_time_msec = 0;
    };

    inline connection_statistics get_connection_statistics(uint64_t req_time){
        auto out = m_connection_statisctics;
        m_connection_statisctics.last_check_time_msec = req_time;
        m_connection_statisctics.chunk_sent_bytes = 0;
        m_connection_statisctics.chunk_recv_bytes = 0;
        return out;
    }

protected:
    virtual void run();

    std::shared_ptr<QueueBase> m_queue_for_reading_data_from_source = nullptr;
    std::shared_ptr<QueueBase> m_queue_for_writing_data_to_source = nullptr;

    virtual ssize_t read_data_from_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf);
    virtual ssize_t write_data_to_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf);
    
    ///  Parsing strategy
    std::unique_ptr<ParserBase> m_parser = nullptr;
    WorkerPostMortem m_postmortem_callback = [](ConnectionWorker* a){
        std::cerr<<"Posmorten callback is not set in ConnectionWorker! It causes memory leak.\n";
    }; ///callback that will call after both threads stops
    std::atomic_bool m_read_thread_alive;
    std::atomic_bool m_write_thread_alive;
    std::atomic_bool m_stop_asked;
    std::atomic_bool m_stopped;

private:
    void thread_read_from_source();//read from source, put 2 Parser, and then put 2 queue
    void thread_write_to_source();//get from queue and write 2 source
    void thread_guard(); /// waits until read and write thread finished
    std::thread t_guard;
    std::recursive_mutex m_mutex_mortem;

    static std::atomic<std::uint64_t> m_total_worker_id_counter;
    uint64_t m_worker_id;
    std::vector< std::vector<uint8_t> > m_dg_vector_for_parser;
    connection_statistics m_connection_statisctics;
};


#endif //SHOVEL_CONNECTIONWORKER_H
