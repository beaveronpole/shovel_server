#include "TCPSocketWorker.h"

using namespace std;


TCPSocketWorker::TCPSocketWorker(shared_ptr<QueueBase> queue_for_reading_data_from_source,
                                 shared_ptr<QueueBase> queue_for_writing_data_to_source,
                                 unique_ptr<TCPSocket> net_socket) : ConnectionWorker(
        queue_for_reading_data_from_source, queue_for_writing_data_to_source),
                                                         m_net_socket(move(net_socket)){

}

ssize_t TCPSocketWorker::read_data_from_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) {
    if (m_net_socket != nullptr){
        static uint32_t read_count = 0;
        auto status = m_net_socket->read(buf);
        if (status < 0){
            return -1;
        }else {
            read_count+=status;
            return status;
        }
    } else {
        return -1;
    }
}

ssize_t TCPSocketWorker::write_data_to_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) {
    if (m_net_socket != nullptr){
        auto status = m_net_socket->write(buf);

        if (status < 0){
            cerr << "Error on writing to net! : " << m_net_socket->get_status_string(m_net_socket->status()) << endl;
            return -2;
        }else {
            return status;
        }
    } else {
        return -1;
    }
}

TCPSocketWorker::~TCPSocketWorker() {

}

