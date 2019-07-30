
#ifndef SHOVEL_INSTANCE_SUBPROCESS_1_H
#define SHOVEL_INSTANCE_SUBPROCESS_1_H

#include <string>
#include <memory>
#include <chrono>

#include "../abstract_subprocess/AbstractSubprocess.h"
#include "../../parsers/ParserExample.h"

class InstanceSubprocess_1 : public AbstractSubprocess{
public:
    explicit InstanceSubprocess_1(std::string&& path_input, std::string&& path_output);


    InstanceSubprocess_1(const InstanceSubprocess_1& other) = delete;
    InstanceSubprocess_1& operator= ( const InstanceSubprocess_1&) = delete;
    InstanceSubprocess_1(const InstanceSubprocess_1&& other) = delete;
    InstanceSubprocess_1& operator= ( const InstanceSubprocess_1&&) = delete;

protected:
    void datagram_processor(std::shared_ptr<SizedArray<uint8_t, std::numeric_limits<uint16_t>::max()>> ptr) override;
};


#endif //SHOVEL_INSTANCE_SUBPROCESS_1_H
