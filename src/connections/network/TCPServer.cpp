#include "TCPServer.h"

using namespace std;

TCPServer::TCPServer():
        m_master_socket(-1),
        m_status(TCPSERVER_OK),
        m_atomic_server_enable(true),
        m_atomic_server_finished(true)
{
}

int TCPServer::set_non_block(int s) {
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(s, F_GETFL, 0))) {
        flags = 0;
    }
    return fcntl(s, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}


void TCPServer::start(uint16_t listen_port, NetServerListenType listen_type) {
    m_atomic_server_enable = true;
    m_atomic_server_finished = false;
    auto status = start_listen(listen_port, listen_type);
    if (status < 0){
        m_atomic_server_finished = true;
        m_atomic_server_enable = false;
    }
}


int TCPServer::start_listen(uint16_t listen_port, NetServerListenType listen_type) {
    reset_status();
    if (m_master_socket>0){
        close(m_master_socket);
        this_thread::sleep_for(chrono::milliseconds(300));
    }
    m_master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_master_socket == -1){
        m_status = TCPSERVER_SOCKET_ERROR;
        cerr << "ERROR TCPServer error in constructor. socket returned -1" << endl;
        return -1;
    }

    int flag_reuse_addr = 1;
    int status_set_sock_opt = setsockopt(m_master_socket, SOL_SOCKET, SO_REUSEADDR, &flag_reuse_addr, sizeof(flag_reuse_addr));
    if (status_set_sock_opt == -1){
        m_status = TCPSERVER_SETSOCKOPT_ERROR;
        cerr << "ERROR TCPServer error in constructor. setsockopt SO_REUSEADDR return -1. MasterSocket = " << m_master_socket << endl;
        cerr << "ERRNO = " << errno <<", " << strerror(errno) << endl;
        return -2;
    }

    sockaddr_in sock_addr = {0};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(listen_port);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_stat = bind(m_master_socket, (struct sockaddr*)(&sock_addr), sizeof(sock_addr));
    if (bind_stat == -1){
        close(m_master_socket);
        m_master_socket = -1;
        m_status = TCPSERVER_BIND_ERROR;
        cerr << "ERROR TCPServer error in constructor. bind returned -1." << endl;
        return -3;
    }

    uint32_t rcv_buf_size = TCP_SERVER_RCVBUF_SIZE;
    uint32_t snd_buf_size = TCP_SERVER_SNDBUF_SIZE;
    setsockopt(m_master_socket, SOL_SOCKET, SO_RCVBUF, &rcv_buf_size, sizeof(rcv_buf_size) );
    setsockopt(m_master_socket, SOL_SOCKET, SO_SNDBUF, &snd_buf_size, sizeof(snd_buf_size) );

    int listen_status = listen(m_master_socket, MAX_PENDING_CONNECTIONS);
    if (listen_status == -1){
        m_status = TCPSERVER_LISTEN_ERROR;
        cerr << "ERROR TCPServer error in constructor. listen returned -1." << endl;
        close(m_master_socket);
        m_master_socket = -1;
        return -4;
    }

    /*start accept thread*/
    if (listen_type == NetServerListenType::TCPSERVER_BLOCKED) {
        auto t = thread(&TCPServer::_thread_blocked_accept, this);
        t.detach();
    }
    else if (listen_type == NetServerListenType::TCPSERVER_UNBLOCKED){
        auto t = thread(&TCPServer::_thread_unblocked_accept, this);
        t.detach();
    }
    else if (listen_type == NetServerListenType::TCPSERVER_TIMED){
        auto t = thread(&TCPServer::_thread_timed_accept, this);
        t.detach();
    }
    return 0;
}

void TCPServer::epoll_loop(const std::string &description, int32_t timeout) {
    constexpr uint32_t EPOLL_MAXEVENTS = 32;
    int EPoll = epoll_create1(0);
    epoll_event Event;
    Event.data.fd = m_master_socket;
    Event.events = EPOLLIN; // ready for reading

    epoll_ctl(EPoll, EPOLL_CTL_ADD, m_master_socket, &Event);

    uint32_t time_out_msec = static_cast<uint32_t>(timeout);
    while(m_atomic_server_enable){
        epoll_event Events[EPOLL_MAXEVENTS];
        int N = epoll_wait(EPoll, Events, EPOLL_MAXEVENTS, time_out_msec);
        //TODO check epoll wait errors
        if (!m_atomic_server_enable)
            break;
        for (uint32_t i = 0; i < N; i++){
            if (Events[i].data.fd == m_master_socket){
                int slave_socket = accept(m_master_socket,0,0);
                call_accepted_callback_function(slave_socket);
            }
            else{
                cerr << "Error TCPServer " << description << " epoll not on m_master_socket!\n";
            }
        }
    }
    m_atomic_server_finished = true;
}

void TCPServer::_thread_blocked_accept() {
    epoll_loop("_thread_blocked_accept");
}

void TCPServer::_thread_timed_accept() {
    epoll_loop("_thread_timed_accept", 200);
}

void TCPServer::_thread_unblocked_accept() {
    set_non_block(m_master_socket);
    while(m_atomic_server_enable) {
        if (m_master_socket <= 0) {
            cerr << "ERROR TCPServer _thread_unblocked_accept in function connection_accept, master_socket <= 0.\n";
            continue;
        }

        sockaddr_storage their_addr = {0};
        socklen_t len;
        int slave_socket = accept(m_master_socket, (sockaddr *) &their_addr, &len);
        if (errno == EAGAIN or errno == EWOULDBLOCK){
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }
        if (slave_socket == -1) {
            cerr << "ERROR TCPServer _thread_unblocked_accept in function connection_accept, slave_socket == -1.\n";
            break;
        }
        call_accepted_callback_function(slave_socket);
    }
    m_atomic_server_finished = true;
}

int TCPServer::set_client_socket_flags(int s) {
    vector< tuple<uint32_t, uint32_t, uint32_t> > flags{
            make_tuple(SOL_SOCKET, SO_KEEPALIVE,    1), //1
            make_tuple(SOL_TCP, TCP_KEEPIDLE,       2), //2
            make_tuple(SOL_TCP, TCP_KEEPCNT,        3), //3
            make_tuple(SOL_TCP, TCP_KEEPINTVL,      4), //4
            make_tuple(SOL_TCP, TCP_USER_TIMEOUT, 4000), //400
            make_tuple(SOL_TCP, TCP_NODELAY,        1), //1
            make_tuple(SOL_TCP, TCP_QUICKACK,       1) //1
    };
    for (auto t : flags){
        int stat = setsockopt(s, get<0>(t), get<1>(t), &(get<2>(t)), sizeof(get<2>(t)));
        if (stat < 0){
            m_status = TCPSERVER_SETSOCKOPT_ERROR;
            cerr << "Error in TCPServer::set_client_socket_flags for socket " << s << " flag = " << get<0>(t) <<
                 " value = " << get<1>(t) <<" \n";
            return -1;
        }
    }

    timeval read_delay{TCP_SERVER_RCV_TIMEOUT_MSEC / 1000, TCP_SERVER_RCV_TIMEOUT_MSEC % 1000};
    int stat = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &read_delay, sizeof(read_delay));
    if (stat < 0){
        m_status = TCPSERVER_SETSOCKOPT_ERROR;
        cerr << "Error in TCPServer::set_client_socket_flags for socket " << s << " flag = " << SOL_SOCKET <<
             " value = " << read_delay.tv_sec << "s + " << read_delay.tv_usec << "us" <<" \n";
        return -1;
    }

    timeval write_delay{TCP_SERVER_SND_TIMEOUT_MSEC / 1000, TCP_SERVER_SND_TIMEOUT_MSEC % 1000};
    stat = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &read_delay, sizeof(read_delay));
    if (stat < 0){
        m_status = TCPSERVER_SETSOCKOPT_ERROR;
        cerr << "Error in TCPServer::set_client_socket_flags for socket " << s << " flag = " << SOL_SOCKET <<
             " value = " << read_delay.tv_sec << "s + " << read_delay.tv_usec << "us" <<" \n";
        return -1;
    }

    return 0;
}

void TCPServer::call_accepted_callback_function(int s) {
    if (s <=0 ) {
        return;
    }
    int status = set_client_socket_flags(s);
    auto out_accepted_socket = make_unique<TCPSocket>(s);
    m_accepted_callback(move(out_accepted_socket));
}

std::string TCPServer::get_status_string(NetServerStatus status) {
    if (status == TCPSERVER_OK){
        return std::string("TCPServer status OK");
    } else if (status == TCPSERVER_SOCKET_ERROR){
        return std::string("TCPServer Socket error");
    } else if (status == TCPSERVER_SETSOCKOPT_ERROR){
        return std::string("TCPServer Setsockopt error");
    } else if (status == TCPSERVER_BIND_ERROR){
        return std::string("TCPServer bind error");
    } else if (status == TCPSERVER_LISTEN_ERROR){
        return std::string("TCPServer listen error");
    } else if (status == TCPSERVER_ACCEPT_ERROR){
        return std::string("TCPServer accept error");
    } else if (status == TCPSERVER_SLAVESOCKET_ERROR) {
        return std::string("TCPServer slavesocket error");
    }
    return std::string("Unknown TCPServer status");
}

TCPServer::~TCPServer() {
    m_atomic_server_enable = false;
    if (m_master_socket >= 0){
        close(m_master_socket);
    }
    m_master_socket = -1;
    while(!m_atomic_server_finished){
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

