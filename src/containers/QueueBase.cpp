#include "QueueBase.h"

QueueBase::QueueBase() :
    m_total_count(0),
    m_size(0),
    m_status(QUEUEBASE_STATUS::STATUS_QUEUEBASE_OK){
}

QueueBase::QUEUEBASE_STATUS QueueBase::get_status() {
    return m_status;
}

uint32_t QueueBase::size()
{
    return m_size;
}

uint32_t QueueBase::capacity(){
    return m_total_count;
}

QueueBase::~QueueBase()
{

}
