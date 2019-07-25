#include "TCPClient.h"

using namespace std;

TCPClient::TCPClient():
        m_status(TCPCLIENT_OK),
        m_port(0),
        m_ip("0.0.0.0"){
}

void TCPClient::connect(const string &ip, uint16_t port) {
    m_port = port;
    m_ip = ip;
    reset_status();
    int slave_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slave_socket < 0){
        m_status = TCPCLIENT_SOCKET_CONNECT_ERROR;
        return;
    }

    struct sockaddr_in sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(m_port);

    unsigned char sbuf[sizeof(struct in_addr)];
    int s = inet_pton(AF_INET, m_ip.c_str(), sbuf);
    sock_addr.sin_addr.s_addr =(*((unsigned int*)sbuf));
    if (s <= 0) {
        if (s == 0) {
            cerr<< "Not in presentation format IP string" << endl;
            m_status = TCPCLIENT_SOCKET_ERROR;
            return;
        }
        else {
            perror("inet_pton");
            m_status = TCPCLIENT_SOCKET_ERROR;
            return;
        }
    }

    uint32_t rcv_buf_size = TCP_CLIENT_RCVBUF_SIZE;
    uint32_t snd_buf_size = TCP_CLIENT_SNDBUF_SIZE;
    setsockopt(slave_socket, SOL_SOCKET, SO_RCVBUF, &rcv_buf_size, sizeof(rcv_buf_size) );
    setsockopt(slave_socket, SOL_SOCKET, SO_SNDBUF, &snd_buf_size, sizeof(snd_buf_size) );

    int constat = ::connect(slave_socket, (sockaddr*)(&sock_addr), sizeof(sock_addr));
    if (constat == 0){
        call_connect_callback_function(slave_socket);
    } else {
        m_status = TCPCLIENT_SOCKET_ERROR;
        cerr << "Error on socket connection to: " << m_ip << " port: " << m_port << " strerr: " << strerror(errno) << "("<<errno << ")" << endl;
    }
}


int TCPClient::set_client_socket_flags(int s) {

    vector< tuple<uint32_t, uint32_t, uint32_t> > flags{
            make_tuple(SOL_SOCKET, SO_KEEPALIVE,    1),
            make_tuple(SOL_TCP, TCP_KEEPIDLE,       2),
            make_tuple(SOL_TCP, TCP_KEEPCNT,        3),
            make_tuple(SOL_TCP, TCP_KEEPINTVL,      400),
            make_tuple(SOL_TCP, TCP_USER_TIMEOUT,   4000),
            make_tuple(SOL_TCP, TCP_NODELAY,        1),
            make_tuple(SOL_TCP, TCP_QUICKACK,       1)
    };

    for (auto t : flags){
        int stat = setsockopt(s, get<0>(t), get<1>(t), &(get<2>(t)), sizeof(get<2>(t)));
        if (stat < 0){
            m_status = TCPCLIENT_SETSOCKOPT_ERROR;
            cerr << "Error in TCPClient::set_client_socket_flags for socket " << s << " flag = " << get<0>(t) <<
                 " value = " << get<1>(t) <<" \n";
            return -1;
        }
    }

    timeval read_delay{TCP_CLIENT_RCV_TIMEOUT_MSEC / 1000, TCP_CLIENT_RCV_TIMEOUT_MSEC % 1000};
    int stat = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &read_delay, sizeof(read_delay));
    if (stat < 0){
        m_status = TCPCLIENT_SETSOCKOPT_ERROR;
        cerr << "Error in TCPClient::set_client_socket_flags for socket " << s << " flag = " << SOL_SOCKET <<
             " value = " << read_delay.tv_sec << "s + " << read_delay.tv_usec << "us" <<" \n";
        return -1;
    }

    timeval write_delay{TCP_CLIENT_SND_TIMEOUT_MSEC / 1000, TCP_CLIENT_SND_TIMEOUT_MSEC % 1000};
    stat = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &write_delay, sizeof(write_delay));
    if (stat < 0){
        m_status = TCPCLIENT_SETSOCKOPT_ERROR;
        cerr << "Error in TCPClient::set_client_socket_flags for socket " << s << " flag = " << SOL_SOCKET <<
             " value = " << write_delay.tv_sec << "s + " << write_delay.tv_usec << "us" <<" \n";
        return -1;
    }
    return 0;
}


void TCPClient::call_connect_callback_function(int s) {
    if (s <=0 ) {
        return;
    }
    int status = set_client_socket_flags(s);
    auto out_connected_socket = make_unique<TCPSocket>(s);
    m_accepted_callback(move(out_connected_socket));
}


TCPClient::~TCPClient() {

}


