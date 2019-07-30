#include "instance_subprocess_1.h"

using namespace std;

InstanceSubprocess_1::InstanceSubprocess_1(std::string&& path_input, std::string&& path_output):
        AbstractSubprocess(move(path_input),
                           move(path_output),
                           "subprocess_1"){
    m_local_worker->set_parser(make_unique<ParserExample>());
    start();
}

void InstanceSubprocess_1::datagram_processor(shared_ptr<SizedArray<uint8_t, std::numeric_limits<uint16_t>::max()>> arr) {
    cout << "InstanceSubprocess_1 received " << arr->data_size();
    /// can process data here from arr
}


