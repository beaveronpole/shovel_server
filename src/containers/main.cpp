#include <iostream>

#include "RingQueues.h"

int main(int argc, char** argv){
    RingQueue a(10, 60);
    uint8_t buf[10]={1,2,3,4};
    a.put(buf, sizeof(buf));
    uint8_t buf2[10] = {0,0,0,0,0,0,0};
    auto got_size = a.get(buf2, sizeof(buf2));
    std::cout << "Got size = " << got_size << std::endl;
    for (auto& d: buf2){
        cout << (uint32_t)d << " ,";
    }
}