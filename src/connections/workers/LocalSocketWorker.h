#ifndef SHOVEL_LOCALWORKER_H
#define SHOVEL_LOCALWORKER_H

#include "ConnectionWorker.h"
#include "../../connections/local/LocalSocket.h"

/// Thing for local UNIX socket that can:
///1. read data from real local UNIX socket in one thread
///2. parse data with some parser aka strategy
///3. put parsed data to the output smth in other thread


class LocalSocketWorker : public ConnectionWorker {
public:
    explicit LocalSocketWorker(std::shared_ptr<QueueBase> queue_for_reading_data_from_source,
                               std::shared_ptr<QueueBase> queue_for_writing_data_to_source,
                               std::unique_ptr<LocalSocket> local_socket);

    ~LocalSocketWorker() override ;

    LocalSocketWorker(const LocalSocketWorker& other) = delete;
    LocalSocketWorker& operator= (const LocalSocketWorker&) = delete;
    LocalSocketWorker(const LocalSocketWorker&& other) = delete;
    LocalSocketWorker& operator= (const LocalSocketWorker&&) = delete;

protected:
    ssize_t read_data_from_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) override;
    ssize_t write_data_to_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) override;

private:
    std::unique_ptr<LocalSocket> m_local_socket;
};


#endif //SHOVEL_LOCALWORKER_H
