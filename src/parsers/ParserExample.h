#ifndef SHOVEL_PARSEREXAMPLE_H
#define SHOVEL_PARSEREXAMPLE_H

#include <cstddef>


#include "ParserBase.h"


class ParserExample : public ParserBase {
public:
    struct data_head{
        inline data_head(){
            memset(this, 0, sizeof(data_head));
            main_word = MAIN_WORD;
        }
        uint32_t main_word;

        uint16_t msg_type;
        uint16_t msg_ver;

        uint16_t size;
        uint16_t reserv;
    };

private:
    const static uint32_t MAIN_WORD = 0xffffffff;
    uint32_t _MAIN_WORD = MAIN_WORD;

    enum PARSER_EXAMPLE_STATE{
        STATE_NODATA,
        STATE_RECEIVING_HEADER,
        STATE_RECEIVING_DATA,
        STATE_RECEIVING_WAIT_NEW_DATA,
        STATE_RECEIVING_COMPLETE,
        STATE_ERROR
    };

public:
    ParserExample();
    uint32_t put(const uint8_t *buf, uint32_t data_size, std::vector< std::vector<uint8_t> >& dg_vector) override;
    static std::vector<uint8_t> prepare_data_for_sending(const uint8_t *buf, size_t data_size);
    ~ParserExample() override;

    ParserExample(const ParserExample& other) = delete;
    ParserExample& operator= (const ParserExample&) = delete;
    ParserExample(const ParserExample&& other) = delete;
    ParserExample& operator= (const ParserExample&&) = delete;


private:
    PARSER_EXAMPLE_STATE m_state = STATE_NODATA;

    void _receiving_header();
    void _receiving_data();
    void _receiving_complete(std::vector< std::vector<uint8_t> >&);

    /** this function get data from parse buffer and parsing it through tmp buffer, put the result in output*/
    uint32_t _parse_data(std::vector<std::vector<uint8_t> > &dg_vector);

    std::vector<uint8_t> m_tmp_dg_buf;///this buffer contains not finished datagrams
};


#endif //SHOVEL_PARSEREXAMPLE_H
