#ifndef SHOVEL_SIZEDARRAY_H
#define SHOVEL_SIZEDARRAY_H

#include <array>

/// simple implementation of STD array, that can hold a real data size.

template <class T,  std::size_t Nm>
class SizedArray : public std::array<T, Nm> {
public:
    virtual ~SizedArray() {

    }

    std::size_t data_size(){
        return m_data_size;
    }

    void resize(std::size_t _size){
        if (_size < Nm)
            m_data_size = _size;
        else
            m_data_size = Nm;
    }

    void clear(){
        m_data_size = 0;
    }

private:
    std::size_t m_data_size = 0;
};


#endif //SHOVEL_SIZEDARRAY_H
