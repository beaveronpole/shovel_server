#include "TCPSocket.h"

using namespace std;

TCPSocket::TCPSocket(int descr):
    m_status(TCPSOCKET_OK),
    m_port(0),
    m_ip(0)
    {
        reset_status();
        set_descriptor(descr);
        m_read_buf = std::make_unique<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> >();
        m_write_buf = std::make_unique<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> >();
}

uint16_t TCPSocket::get_port() const {
    //TODO check is descriptor is correct unix net socket
    return m_addrinfo.sin_port;
}

uint32_t TCPSocket::get_IP() const{
    //TODO check is descriptor is correct unix net socket
    return *(uint32_t*)(&(m_addrinfo.sin_addr));
}

ssize_t TCPSocket::read(std::shared_ptr<SizedArray<uint8_t, NETSOCKET_ARRAY_SIZE> > buf) {
    buf->clear(); /// delete all data in array first
    buf->resize(0);
    if (m_status != TCPSOCKET_OK) {
        return -1;
    }
    reset_status(); //set status TCPSOCKET_OK
    ssize_t status = 0;
    if (m_descr > 0){
        status = recv(m_descr, m_read_buf->data(), m_read_buf->size(), MSG_NOSIGNAL);
        if (status < 0 and (errno == EAGAIN or errno == EINTR or errno == EWOULDBLOCK) ){
            return 0;
        }
        cout << "Net Socket smth wrong. Read size = " << status << " IP: " << get_IP_str() << " port: " << get_port();
        if (status < 0){
            m_status = TCPSOCKET_CLOSED;
            close_descriptor();
            return -1;
        }
        if (status == 0){
            m_status = TCPSCOKET_CLOSED_BY_PEER;
            close_descriptor();
            return -1;
        }
        copy(m_read_buf->begin(), m_read_buf->begin()+status, buf->begin());
        buf->resize((uint64_t)status);
        return status;
    }
    m_status = TCPSOCKET_DESCRIPTOR_ERROR;
    return -1;
}

ssize_t TCPSocket::write(std::shared_ptr<SizedArray<uint8_t, NETSOCKET_ARRAY_SIZE> > buf) {
    if (m_status != TCPSOCKET_OK)
        return -1;
    reset_status();
    ssize_t sent_size = 0;
    if (m_descr > 0){
        while(sent_size < buf->data_size()){
            auto status = send(m_descr, buf->data()+sent_size, buf->data_size()-sent_size, MSG_NOSIGNAL);
            if (status < 0 and (errno == EAGAIN or errno == ENOBUFS)) {
                return 0;
            }
            if (status < 0 ) {
                cerr << "TCPSocket ERROR " << strerror(errno) << " errno = " << errno << endl;
                m_status = TCPSOCKET_CLOSED;
                close_descriptor();
                return -1;
            }
            static uint32_t written_count = 0;
            written_count+=status;
            sent_size += status;
        }
        buf->clear();
        return sent_size;
    }
    m_status = TCPSOCKET_DESCRIPTOR_ERROR;
    return -1;
}

void TCPSocket::set_descriptor(int descr) {
    reset_status();
    memset(&m_addrinfo, 0, sizeof(m_addrinfo));
    m_descr = descr;
    if (m_descr > 0) {
        socklen_t addr_size = sizeof(m_addrinfo);
        int res = getpeername(m_descr, (struct sockaddr *) &m_addrinfo, &addr_size);
        if (res != 0){
            cerr << "Error in TCPSocket constructor getpeername for descriptor: " << m_descr << endl;
            m_status = TCPSOCKET_DESCRIPTOR_ERROR;
            m_descr = -1;
        }
    }
}

void TCPSocket::close_descriptor() {
    reset_status();
    if (m_descr > 0) {
        close(m_descr);
        m_descr = -1;
    }
    m_status = TCPSOCKET_CLOSED;
}

std::string TCPSocket::get_IP_str() const{
    return std::string(inet_ntoa(m_addrinfo.sin_addr), INET_ADDRSTRLEN);
}

std::string TCPSocket::get_status_string(TCPSocket::NetSocketStatus status) {
    if (status == TCPSOCKET_OK){
        return std::string("TCPSocket status OK");
    } else if (status == TCPSOCKET_DESCRIPTOR_ERROR){
        return std::string("TCPSocket descriptor error");
    } else if (status == TCPSOCKET_PROTOCOL_ERROR){
        return std::string("TCPSocket protocol error");
    } else if (status == TCPSOCKET_CLOSED){
        return std::string("TCPSocket closed");
    } else if (status == TCPSCOKET_CLOSED_BY_PEER){
        return std::string("TCPSocket closed by peer");
    }
    return std::string("Unknown TCPServer status");
}

TCPSocket::~TCPSocket() {
    //TODO check is descriptor is correct unix net socket
    if (m_descr){
        shutdown(m_descr, SHUT_RDWR);
        close_descriptor();
    }
}

