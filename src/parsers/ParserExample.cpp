#include "ParserExample.h"

ParserExample::ParserExample() {
    m_tmp_dg_buf.reserve(std::numeric_limits<uint16_t>::max());
    m_tmp_dg_buf.clear();
}

uint32_t ParserExample::put(const uint8_t *buf, uint32_t data_size, std::vector< std::vector<uint8_t> >& dg_vector) {
    /// clear dg vector for found datagrams
    for (auto& sub_vector: dg_vector) {
        sub_vector.clear();
    }
    dg_vector.clear();
    this->m_parsing_buffer.insert(m_parsing_buffer.end(), buf, buf + data_size);
    _parse_data(dg_vector);
    return dg_vector.size();
}

uint32_t ParserExample::_parse_data(std::vector<std::vector<uint8_t> > &dg_vector) {
    while(!m_parsing_buffer.empty()) {
        if (m_state == STATE_NODATA) {
            if (m_parsing_buffer.size() >= sizeof(data_head) ) {
                m_tmp_dg_buf.reserve(std::numeric_limits<uint16_t>::max());
                m_state = STATE_RECEIVING_HEADER;
            }
            else
                break;
        } else if (m_state == STATE_RECEIVING_HEADER) {
            _receiving_header();
        } else if (m_state == STATE_RECEIVING_DATA) {
            _receiving_data();
        } else if (m_state == STATE_RECEIVING_WAIT_NEW_DATA) { /// if we waited data and finished loop
            m_state = STATE_RECEIVING_DATA;
            break;
        } else if (m_state == STATE_RECEIVING_COMPLETE) {
            _receiving_complete(dg_vector);
        } else if (m_state == STATE_ERROR) {
//            std::cerr << "ParserBase Example ERROR!" << std::endl;
            m_tmp_dg_buf.clear();
            m_parsing_buffer.pop_front();
            m_state = STATE_NODATA;
        }
    }
    return dg_vector.size();
}

void ParserExample::_receiving_header() {
    /// check first byte of head
    auto _parsing_buffer_begin = m_parsing_buffer.begin();
    if ( (*(_parsing_buffer_begin) != (*((uint8_t*)&_MAIN_WORD)))  ){
        m_state = STATE_ERROR;
        return;
    }

    m_tmp_dg_buf.resize(sizeof(data_head));
    std::copy(_parsing_buffer_begin, _parsing_buffer_begin+sizeof(data_head), m_tmp_dg_buf.data());
    auto head = (data_head*)m_tmp_dg_buf.data();
    if (head->main_word != MAIN_WORD || head->size < sizeof(head)){
        m_state = STATE_ERROR;/// tmp buffer will clear automatically
    }
    else{
        m_state = STATE_RECEIVING_DATA;
    }
}

void ParserExample::_receiving_data() {

    /// get data from deque since  (_parsing_buffer.begin()+_tmp_kg_buf.size())
    auto head = (data_head*)m_tmp_dg_buf.data();
    if (m_parsing_buffer.size() < head->size){
        m_state = STATE_RECEIVING_WAIT_NEW_DATA;
    }
    else{
        m_tmp_dg_buf.insert(m_tmp_dg_buf.end(), m_parsing_buffer.begin() + m_tmp_dg_buf.size(), m_parsing_buffer.begin() + head->size);
        m_state = STATE_RECEIVING_COMPLETE;
    }
}

void ParserExample::_receiving_complete(std::vector< std::vector<uint8_t> >& out) {
    m_parsing_buffer.erase(m_parsing_buffer.begin(), m_parsing_buffer.begin() + m_tmp_dg_buf.size());
    out.push_back(m_tmp_dg_buf );
    m_tmp_dg_buf.clear();
    m_state = STATE_NODATA;
}

std::vector<uint8_t> ParserExample::prepare_data_for_sending(const uint8_t *buf, size_t data_size) {
    data_head head;
    std::vector<uint8_t> out;
    if (data_size > std::numeric_limits<uint16_t>::max()-sizeof(data_head))
        return std::move(out);
    out.reserve(sizeof(head) + data_size);
    head.main_word = MAIN_WORD;
    head.size = sizeof(head) + data_size;
    head.msg_type = 1;
    head.msg_ver = 1;

    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
            std::chrono::system_clock::now().time_since_epoch()
    );

    out.resize(sizeof(head));
    out.insert(out.end(), buf, buf+data_size);
    std::copy((uint8_t*)&head, (uint8_t*)&head+sizeof(head), out.begin());
    return std::move(out);
}

ParserExample::~ParserExample() {

}


