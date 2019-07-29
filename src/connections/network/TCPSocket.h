#ifndef SHOVEL_NETSOCKET_H
#define SHOVEL_NETSOCKET_H

#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <system_error>
#include <memory>
#include <limits>

#include "../../containers/SizedArray.h"

constexpr uint16_t NETSOCKET_ARRAY_SIZE = std::numeric_limits<uint16_t>::max();

class TCPSocket {
public:
    explicit TCPSocket(int descr = 0);
    enum NetSocketStatus: uint32_t {TCPSOCKET_OK = 0,
            TCPSOCKET_DESCRIPTOR_ERROR,
            TCPSOCKET_PROTOCOL_ERROR,
            TCPSOCKET_CLOSED,
            TCPSCOKET_CLOSED_BY_PEER
            };

    uint16_t get_port() const;
    uint32_t get_IP() const;
    std::string get_IP_str() const;

    /// it will clear the vector before starting reading and putting data to it
    ssize_t read(std::shared_ptr<SizedArray<uint8_t, NETSOCKET_ARRAY_SIZE> > buf);

    /// it will clear the vector after successful data sending
    ssize_t write(std::shared_ptr<SizedArray<uint8_t, NETSOCKET_ARRAY_SIZE> > buf);
    inline NetSocketStatus status() const {return NetSocketStatus((uint32_t)m_status);}
    inline int get_socket_descriptor(){return m_descr;}

    virtual ~TCPSocket();

    TCPSocket(const TCPSocket& other) = delete;
    TCPSocket& operator= (const TCPSocket&) = delete;
    TCPSocket(const TCPSocket&& other) = delete;
    TCPSocket& operator= (const TCPSocket&&) = delete;
    static std::string get_status_string(NetSocketStatus status);

private:
    int m_descr = -1;
    sockaddr_in m_addrinfo;
    mutable std::atomic<uint32_t> m_status;
    uint16_t m_port;
    uint32_t m_ip;
    std::unique_ptr<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> > m_read_buf;
    std::unique_ptr<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> > m_write_buf;
    void set_descriptor(int descr);
    inline void reset_status() const {m_status = TCPSOCKET_OK;}
    void close_descriptor();
};


#endif //SHOVEL_NETSOCKET_H
