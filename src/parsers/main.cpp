#include "ParserExample.h"

using namespace std;

int main(int argc, char** argv){
    ParserExample parser;

    //prepare data for parsing
    vector<uint8_t> data;
    vector<uint8_t> garbage;
    for (int i = 0; i < 10; i++){
        garbage.push_back(i);
    }

    ParserExample::data_head head;
    head.main_word = 0xffffffff;
    head.msg_ver = 4;
    head.size = 56;
    head.msg_type = 33;

    data.resize(head.size);
    memcpy(data.data(), &head, sizeof(head));

    ParserExample::data_head* pt_head = (ParserExample::data_head*)data.data();

    cout << "On input" << endl;
    cout << "main_word = " << pt_head->main_word << endl;
    cout << "head->size = " << pt_head->size << endl;
    cout << "head->type = " << pt_head->msg_type << endl;
    cout << "head->ver = " << pt_head->msg_ver << endl;

    //add some garbage in front of data
    data.insert(data.begin(), garbage.begin(), garbage.end());

// start parsing
    std::vector< std::vector<uint8_t> > for_parsed;
    auto found_dg_count = parser.put(data.data(), data.size(), for_parsed);
    cout << "--------------------------"<<endl;
    cout << "found dg count = " << found_dg_count << endl;
    cout << "after parsing" << endl;
    for (auto& dg : for_parsed){
        ParserExample::data_head* dg_head = (ParserExample::data_head*)(dg.data());
        cout << "dg word " << dg_head->main_word << endl;
        cout << "dg size " << dg_head->size << endl;
        cout << "dg type " << dg_head->msg_type << endl;
        cout << "dg version " << dg_head->msg_ver << endl;
    }

    return 0;
}

