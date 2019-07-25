#ifndef SHOVEL_SERVERLISTEN_H
#define SHOVEL_SERVERLISTEN_H

#include <iostream>
#include <algorithm>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <string>
#include <vector>
#include <tuple>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <cerrno>

#include "TCPSocket.h"

constexpr uint32_t MAX_PENDING_CONNECTIONS = 32;
constexpr uint32_t TCP_SERVER_RCVBUF_SIZE = 65536;
constexpr uint32_t TCP_SERVER_SNDBUF_SIZE = 65536;
constexpr uint32_t TCP_SERVER_RCV_TIMEOUT_MSEC = 1000;
constexpr uint32_t TCP_SERVER_SND_TIMEOUT_MSEC = 1000;


class TCPServer {
public:
    enum NetServerStatus {
        TCPSERVER_OK,
        TCPSERVER_SOCKET_ERROR,
        TCPSERVER_SETSOCKOPT_ERROR,
        TCPSERVER_BIND_ERROR,
        TCPSERVER_LISTEN_ERROR,
        TCPSERVER_ACCEPT_ERROR,
        TCPSERVER_SLAVESOCKET_ERROR
    };

    static std::string get_status_string(NetServerStatus status);

    enum class NetServerListenType {
        TCPSERVER_BLOCKED,
        TCPSERVER_UNBLOCKED,
        TCPSERVER_TIMED,
    };

    using AccetedSocketCallBack = std::function<void(std::unique_ptr<TCPSocket>)>;
    explicit TCPServer();
    void start(uint16_t listen_port, NetServerListenType listen_type);
    inline void stop() { m_atomic_server_enable = false; }
    inline NetServerStatus status() const { return m_status; }
    inline int get_descriptor() { return m_master_socket; }
    inline void set_accepted_callback(AccetedSocketCallBack cb) { m_accepted_callback = cb; }
    virtual ~TCPServer();

    TCPServer(const TCPServer &other) = delete;
    TCPServer &operator=(const TCPServer &) = delete;
    TCPServer(const TCPServer &&other) = delete;
    TCPServer &operator=(const TCPServer &&) = delete;

protected:
    AccetedSocketCallBack m_accepted_callback = [](std::unique_ptr<TCPSocket>) {
        std::cerr << "You should set accepted_callback function in TCPServer\n";
    };
    int start_listen(uint16_t port, NetServerListenType listen_type);
    int set_client_socket_flags(int s);

private:
    int m_master_socket;
    NetServerStatus m_status;
    int set_non_block(int s);
    void _thread_blocked_accept();///attention! this thead cannot be stopped by set m_atomic_server_enable to  false. It has to return from accept function first
    void _thread_timed_accept();
    void _thread_unblocked_accept();
    void epoll_loop(const std::string &description, int32_t timeout = -1);
    inline void reset_status() { m_status = TCPSERVER_OK; }
    std::atomic_bool m_atomic_server_enable;
    std::atomic_bool m_atomic_server_finished;
    void call_accepted_callback_function(int s);
};


#endif //SHOVEL_SERVERLISTEN_H
