#ifndef SHOVEL_NETWORKER_H
#define SHOVEL_NETWORKER_H

///
/// Thing for net socket that can:
/// 1. read data from real socket
/// 2. parse data with some parser aka strategy
/// 3. put parsed data to the output smth
///

#include "../../connections/workers/ConnectionWorker.h"
#include "../../connections/network/TCPSocket.h"

class TCPSocketWorker : public ConnectionWorker{
public:

    explicit TCPSocketWorker(std::shared_ptr<QueueBase> queue_for_reading_data_from_source,
                             std::shared_ptr<QueueBase> queue_for_writing_data_to_source,
                             std::unique_ptr<TCPSocket> net_socket);
    inline const TCPSocket& get_socket(){return *m_net_socket;}

    ~TCPSocketWorker() override;

    TCPSocketWorker(const TCPSocketWorker& other) = delete;
    TCPSocketWorker& operator= (const TCPSocketWorker&) = delete;
    TCPSocketWorker(const TCPSocketWorker&& other) = delete;
    TCPSocketWorker& operator= (const TCPSocketWorker&&) = delete;

protected:
    ssize_t read_data_from_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) override;
    ssize_t write_data_to_source(std::shared_ptr<SizedArray<uint8_t, connection_worker_arr_size> > buf) override;


private:
    std::unique_ptr<TCPSocket> m_net_socket;
};


#endif //SHOVEL_NETWORKER_H
