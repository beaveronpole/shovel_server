
#ifndef SHOVEL_INSTANCE_SUBPROCESS_1_H
#define SHOVEL_INSTANCE_SUBPROCESS_1_H

#include <string>
#include <memory>
#include <chrono>

#include "../abstract_subprocess/AbstractSubprocess.h"
#include "../../parsers/ParserExample.h"

class InstanceSubprocess_2 : public AbstractSubprocess{
public:
    explicit InstanceSubprocess_2(std::string&& path_input, std::string&& path_output);


    InstanceSubprocess_2(const InstanceSubprocess_2& other) = delete;
    InstanceSubprocess_2& operator= ( const InstanceSubprocess_2&) = delete;
    InstanceSubprocess_2(const InstanceSubprocess_2&& other) = delete;
    InstanceSubprocess_2& operator= ( const InstanceSubprocess_2&&) = delete;

protected:
    void datagram_processor(std::shared_ptr<SizedArray<uint8_t, std::numeric_limits<uint16_t>::max()>> ptr) override;
};


#endif //SHOVEL_INSTANCE_SUBPROCESS_1_H
