#include "ParserBase.h"

using namespace std;

uint32_t ParserBase::put(const uint8_t *buf, uint32_t data_size, std::vector< std::vector<uint8_t> >& dg_vector) {
    cerr << "You should reimplement method put in your ParserBase." << endl;
    return dg_vector.size();
}

