#ifndef SHOVEL_PARSER_H
#define SHOVEL_PARSER_H

#include <stdint-gcc.h>
#include <deque>
#include <iostream>
#include <cstring>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>

/*
    Parsing strategy base class
*/
class ParserBase {

public:
    explicit ParserBase() = default;
    /// parses binary data from buf with size size, and cut found datagrams to for_parsed vector
    /// return datagram count in vector
    virtual uint32_t put(const uint8_t *buf, uint32_t data_size, std::vector< std::vector<uint8_t> >& for_parsed);

    virtual ~ParserBase() = default;

    ParserBase(const ParserBase& other) = delete;
    ParserBase& operator= (const ParserBase&) = delete;
    ParserBase(const ParserBase&& other) = delete;
    ParserBase& operator= (const ParserBase&&) = delete;

protected:
    /// it is a storage of not parsed data
    std::deque<uint8_t> m_parsing_buffer;
};


#endif //SHOVEL_PARSER_H
