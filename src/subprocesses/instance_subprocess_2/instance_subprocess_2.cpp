#include "instance_subprocess_2.h"

using namespace std;

InstanceSubprocess_2::InstanceSubprocess_2(std::string&& path_input, std::string&& path_output):
        AbstractSubprocess(move(path_input),
                           move(path_output),
                           "subprocess_2"){
    m_local_worker->set_parser(make_unique<ParserExample>());
    start();
}

void InstanceSubprocess_2::datagram_processor(shared_ptr<SizedArray<uint8_t, std::numeric_limits<uint16_t>::max()>> arr) {
    cout << "InstanceSubprocess_2 received " << arr->data_size();
    /// can process data here from arr
}


