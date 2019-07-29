#include "LocalSocketWorker.h"

LocalSocketWorker::LocalSocketWorker(std::shared_ptr<QueueBase> queue_for_reading_data_from_source,
                                     std::shared_ptr<QueueBase> queue_for_writing_data_to_source,
                                     std::unique_ptr<LocalSocket> local_socket):
                            ConnectionWorker(queue_for_reading_data_from_source, queue_for_writing_data_to_source),
                                                                    m_local_socket(move(local_socket)){
}

ssize_t LocalSocketWorker::read_data_from_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) {
    if (m_local_socket != nullptr){
        auto status = m_local_socket->read(buf);
        if (status < 0){
            return -1;
        }else {
            return status;
        }
    } else {
        return -1;
    }
}

ssize_t LocalSocketWorker::write_data_to_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) {
    if (m_local_socket != nullptr){
        auto status = m_local_socket->write(buf);
        if (status < 0){
            return -1;/// on write data socket can be not initialized yet
        }else {
            return status;
        }
    }else {
        return -1;
    }
}

LocalSocketWorker::~LocalSocketWorker() {

}
