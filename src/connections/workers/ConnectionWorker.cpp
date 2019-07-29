#include "ConnectionWorker.h"

using namespace std;

std::atomic<std::uint64_t> ConnectionWorker::m_total_worker_id_counter(0);

ConnectionWorker::ConnectionWorker(shared_ptr<QueueBase> queue_for_reading_data_from_source,
                                   shared_ptr<QueueBase> queue_for_writing_data_to_source):
        m_queue_for_reading_data_from_source(std::move(queue_for_reading_data_from_source)),
        m_queue_for_writing_data_to_source(std::move(queue_for_writing_data_to_source)),
        m_read_thread_alive(false),
        m_write_thread_alive(false),
        m_stop_asked(false),
        m_stopped(false)
{
    m_worker_id = m_total_worker_id_counter.fetch_add(1, memory_order_relaxed);
}


void ConnectionWorker::start() {
    run();
}


void ConnectionWorker::run() {
    m_read_thread_alive = true;
    m_write_thread_alive = true;
    m_stop_asked = false;
    t_guard = thread(&ConnectionWorker::thread_guard, this);
}


void ConnectionWorker::thread_read_from_source() {
    auto read_data_arr = make_shared<SizedArray<uint8_t, connection_worker_arr_size> >();

    static uint32_t read_bytes = 0;

    while(m_queue_for_reading_data_from_source && m_write_thread_alive && !m_stop_asked) {
        auto read_status = read_data_from_source(read_data_arr);
        if ((read_status < 0) || (m_write_thread_alive == false) || (m_stop_asked == true)) {
            cout << "Error in reading in ConnectionWorker read_status = " << read_status << endl;
            //TODO error on reading
            break;
        }
        if (read_status == 0){ /// we dont want put zero size data to queue.
            continue;
        }
        m_connection_statisctics.add_recv(read_status);
        if (m_parser){
            uint32_t res = m_parser->put(read_data_arr->data(),
                                         (uint32_t)read_data_arr->data_size(), m_dg_vector_for_parser);
            for (auto kg : m_dg_vector_for_parser){
                uint16_t put_status = 0;
                while(put_status <= 0) { ///if out Queue unblocked by timer or is unblocking- we dont want to lose our data
                    put_status = m_queue_for_reading_data_from_source->put(kg.data(), (uint16_t) kg.size());
                    if (put_status < 0 ){
                        cerr << "Read from source error " << endl;
                    }
                    if (put_status == 0) {
                        cout << "Read from source. queue full" << endl;
                        this_thread::sleep_for(chrono::milliseconds(10));
                    }
                    if (!m_write_thread_alive || m_stop_asked){
                        break;
                    }
                }
                if (!m_write_thread_alive || m_stop_asked){
                    break;
                }
                //TODO check put status
            }
        }else{
            m_queue_for_reading_data_from_source->put(read_data_arr->data(), (uint16_t)read_data_arr->data_size());
        }
    }
    m_read_thread_alive = false;
}

void ConnectionWorker::thread_write_to_source() {
    auto write_data_arr = make_shared<SizedArray<uint8_t, connection_worker_arr_size> >();
    while(m_queue_for_writing_data_to_source && m_read_thread_alive && !m_stop_asked) {
        //TODO set capacity to uint16 max
        uint16_t get_status = m_queue_for_writing_data_to_source->peek(write_data_arr->data(), write_data_arr->size());
        write_data_arr->resize(get_status);
        //TODO if queue blocked and socket closed- do smth
        if ( (m_read_thread_alive == false) || (m_stop_asked == true) ) {
            break;
        }
        if (get_status == 0){
            // TODO think about not wait, but block
            this_thread::sleep_for(chrono::milliseconds(1));
        }
        if (get_status > 0){
            ssize_t write_status = 0;
            while(write_status == 0) {
                write_status = write_data_to_source(write_data_arr);
                this_thread::yield();
            }
            if (write_status < 0){
                //TODO check write status
                cerr << "write status = " << write_status << endl;
                break;
            }
            m_connection_statisctics.add_sent(write_status);
            /// apply data sending from QUEUE (take zero data size for just tell QUEUE that we dont need anymore the last element without copying it)
            m_queue_for_writing_data_to_source->get(write_data_arr->data(), 0);
        }
    }
    m_write_thread_alive = false;
}

void ConnectionWorker::thread_guard() {
    thread t_read = thread(&ConnectionWorker::thread_read_from_source, this);
    thread t_write = thread(&ConnectionWorker::thread_write_to_source, this);
    t_read.join();
    t_write.join();
    m_stopped = true;
    m_postmortem_callback(this);
    /// here the object must be dead
}

ConnectionWorker::~ConnectionWorker() {
    std::unique_lock<recursive_mutex> l(m_mutex_mortem); /// need for correct delete object.
    stop();
    if (t_guard.joinable()) {
        if (t_guard.get_id() != this_thread::get_id()) {
            t_guard.join();
        } else {
            t_guard.detach();
        }
    }
}

ssize_t ConnectionWorker::read_data_from_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) {
    buf->clear();
    cerr << "Error. ConnectionWorker in function for thread thread_read_from_source. You should reimplement this function in your class"
         << endl;
    this_thread::sleep_for(chrono::seconds(1));
    return 0;
}

ssize_t ConnectionWorker::write_data_to_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) {
    buf->clear();
    cerr << "Error. ConnectionWorker in function for thread thread_write_to_source. You should reimplement this function in your class"
         << endl;
    this_thread::sleep_for(chrono::seconds(1));
    return 0;
}


void ConnectionWorker::stop() {
    m_stop_asked = true;
}

void ConnectionWorker::set_parser(std::unique_ptr<ParserBase>&& parser) {
    m_parser = std::move(parser);
}


