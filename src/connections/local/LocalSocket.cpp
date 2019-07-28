#include "LocalSocket.h"

using namespace std;

LocalSocket::LocalSocket(std::string &&path_input, std::string &&path_output):
        m_fd(0),
        m_path_for_reading(""),
        m_path_for_writing(""){
    init_socket(move(path_input), move(path_output));
    m_buf_for_reading = std::make_unique<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> >();
    m_buf_for_writing = std::make_unique<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> >();
}

void LocalSocket::init_socket(std::string &&path_input, std::string &&path_output) {
    m_path_for_reading = path_input;
    m_path_for_writing = path_output;
    auto unlink_status = unlink(m_path_for_reading.c_str());
    if ( (m_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) <0 ) {
        m_fd = 0;
        cerr << "Error in opening Local Socket socket on path: " << m_path_for_reading << endl;
        return;
    }

    timeval sock_recv_timer = {0, 20};
    setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &sock_recv_timer, sizeof(sock_recv_timer));

    timeval sock_send_timer = {0, 20};
    setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, &sock_send_timer, sizeof(sock_send_timer));

    sockaddr_un sock_addr{0};
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, m_path_for_reading.c_str());

    auto size = offsetof(sockaddr_un, sun_path) + strlen(sock_addr.sun_path);
    if ( bind(m_fd, (sockaddr*)&sock_addr, size) < 0 ) {
        cerr << "Error in bind Local Socket on path: " << m_path_for_reading << endl;
        close(m_fd);
        m_fd = 0;
    }
    cout << "Init local socket fd = " << m_fd << endl;
}

ssize_t LocalSocket::read(std::shared_ptr< SizedArray<uint8_t, LOCALSOCKET_ARRAY_SIZE> > buf) {
    buf->clear();
    if (m_fd == 0)
        return -1;
    sockaddr_un sock_addr_from{0};
    socklen_t fromlen = sizeof(sock_addr_from);
    if (buf->size() < m_buf_for_reading->size()){
        cerr << "Local Socket buf for read has capacity less than UINT32_MAX. path in: " << m_path_for_reading << endl;
    }
    auto recv_size = recvfrom(m_fd, m_buf_for_reading->data(), m_buf_for_reading->size(), 0, (sockaddr*)&sock_addr_from, &fromlen);
    if ( (recv_size < 0) and (errno == EAGAIN || errno == EWOULDBLOCK)){
        //! it is not a error
        return 0;
    }
    if (recv_size < 0){
        cerr << "Local Socket read error. path in: " << m_path_for_reading << " errno = " << errno << " errstr: " << strerror(errno);
        close(m_fd);
        m_fd = 0;
        return recv_size;
    }
    copy(m_buf_for_reading->begin(), m_buf_for_reading->begin() + recv_size, buf->begin());
    buf->resize((std::size_t)recv_size);
    return recv_size;
}

ssize_t LocalSocket::write(std::shared_ptr< SizedArray<uint8_t, LOCALSOCKET_ARRAY_SIZE> > buf) {
    if (m_fd == 0) {
        cerr << "File descriptor is zero" << endl;
        return -1;
    }
    sockaddr_un out{0};
    out.sun_family = AF_UNIX;
    strcpy(out.sun_path, m_path_for_writing.c_str());
    auto send_status = sendto(m_fd, buf->data(), buf->data_size(), 0, (sockaddr*)&out, sizeof(out));
    if (send_status < 0){
        cerr  << "send_status = " << send_status << " error = " << strerror(errno) << " " <<errno;
    }
    if (send_status < 0 and (errno == ENOENT or errno == ECONNREFUSED or errno == EAGAIN)) {
        // TODO error handling
        send_status = 0;
    }else{
        buf->clear();
    }
    return send_status;
}

LocalSocket::~LocalSocket() {
    close(m_fd);
    unlink(m_path_for_reading.c_str());
}
