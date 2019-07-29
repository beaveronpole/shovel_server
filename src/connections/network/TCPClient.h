#ifndef SHOVEL_CLIENTCONNECT_H
#define SHOVEL_CLIENTCONNECT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/tcp.h>
#include <memory>
#include <functional>
#include <vector>

#include "TCPSocket.h"

constexpr uint32_t TCP_CLIENT_RCVBUF_SIZE = 65536;
constexpr uint32_t TCP_CLIENT_SNDBUF_SIZE = 65536;
constexpr uint32_t TCP_CLIENT_RCV_TIMEOUT_MSEC = 1000;
constexpr uint32_t TCP_CLIENT_SND_TIMEOUT_MSEC = 1000;

class TCPClient {
public:
    enum NetClientStatus{
        TCPCLIENT_OK,
        TCPCLIENT_SOCKET_CONNECT_ERROR,
        TCPCLIENT_SOCKET_ERROR,
        TCPCLIENT_SETSOCKOPT_ERROR};

    using AccetedSocketCallBack = std::function< void(std::unique_ptr< TCPSocket >) >;
    explicit TCPClient();
    void connect(const std::string &ip, uint16_t port);
    inline void set_connected_callback(AccetedSocketCallBack cb) {m_accepted_callback = cb;}
    inline NetClientStatus status() {return m_status;}
    virtual ~TCPClient();

    TCPClient(const TCPClient& other) = delete;
    TCPClient& operator= (const TCPClient&) = delete;
    TCPClient(const TCPClient&& other) = delete;
    TCPClient& operator= (const TCPClient&&) = delete;

private:
    void call_connect_callback_function(int s);
    int set_client_socket_flags(int s);
    AccetedSocketCallBack m_accepted_callback = [](std::unique_ptr< TCPSocket >){
        std::cerr<< "You should set accepted_callback function in NetServer\n";
    };
    inline void reset_status() {m_status = TCPCLIENT_OK;}
    NetClientStatus m_status;
    std::string m_ip;
    uint16_t m_port;
};



#endif //SHOVEL_CLIENTCONNECT_H
