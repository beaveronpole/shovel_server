#ifndef SHOVEL_LOCALSOCKET_H
#define SHOVEL_LOCALSOCKET_H

#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <limits>

#include "../../containers/SizedArray.h"

constexpr uint16_t LOCALSOCKET_ARRAY_SIZE = std::numeric_limits<uint16_t>::max();

class LocalSocket {
public:
    explicit LocalSocket(std::string &&path_input, std::string &&path_output);
    void init_socket(std::string &&path_input, std::string &&path_output);

    /// it will clear the vector before starting reading and putting data to it
    ssize_t read(std::shared_ptr< SizedArray<uint8_t, LOCALSOCKET_ARRAY_SIZE> > buf);

    /// it will clear the vector after successful data sending
    ssize_t write(std::shared_ptr< SizedArray<uint8_t, LOCALSOCKET_ARRAY_SIZE> > buf);

    LocalSocket(const LocalSocket& other) = delete;
    LocalSocket& operator= ( const LocalSocket&) = delete;
    LocalSocket(const LocalSocket&& other) = delete;
    LocalSocket& operator= ( const LocalSocket&&) = delete;

    virtual ~LocalSocket();

private:
    int m_fd;
    std::string m_path_for_reading;
    std::string m_path_for_writing;

    std::unique_ptr<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> > m_buf_for_reading;
    std::unique_ptr<std::array<uint8_t, std::numeric_limits<uint16_t>::max()> > m_buf_for_writing;
};


#endif //SHOVEL_LOCALSOCKET_H
