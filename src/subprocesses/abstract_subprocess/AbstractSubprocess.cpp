#include "AbstractSubprocess.h"

using namespace std;

AbstractSubprocess::AbstractSubprocess(string &&path_input,
                                       string &&path_output,
                                       string &&log_filename,
                                       string &&subscriber_name):
        m_subprocess_name(subscriber_name){
    cout << "Start subprocess " << m_subprocess_name << " pid = " << getpid();

    auto local_socket = make_unique<LocalSocket>( move(path_output), move(path_input) );

    auto q_for_reading_from_source =
            std::make_shared<RQ_NoBlockReadDecorator>(
                    std::make_shared<RQ_BlockWriteDecorator>(
                            (
                                    (
                                            std::make_shared<RingQueue>(QUEUE_COUNT_BAGS, numeric_limits<uint16_t >::max()
                                            )
                                    )
                            )
                    )
            )
    ;

    auto q_for_writing_to_source =
            std::make_shared<RQ_NoBlockReadDecorator>(
                    std::make_shared<RQ_BlockWriteTimedDecorator>(
                            (
                                    (
                                            std::make_shared<RingQueue>(QUEUE_COUNT_BAGS, numeric_limits<uint16_t >::max()
                                            )
                                    )
                            ), 1000
                    )
            )
    ;


    m_local_worker = make_unique<LocalSocketWorker>(q_for_reading_from_source, q_for_writing_to_source, move(local_socket) );
    m_local_worker->start();
}

void AbstractSubprocess::start() {
    loop();
}

void AbstractSubprocess::loop() {
    constexpr std::chrono::milliseconds MAX_LOOP_ITERATION_DURATION(20);
    auto arr = make_shared< SizedArray<uint8_t, numeric_limits<uint16_t>::max()> >();
    while(true){
        if (getppid() < 2){
            exit(0);
        }
        // start code here

        if (m_local_worker->get_queue_for_reading_data_from_source()->size()) {

            uint16_t got_size = m_local_worker->get_queue_for_reading_data_from_source()->get(arr->data(), arr->size());
            while (got_size > 0) {
                arr->resize(got_size);
                if (got_size > 0) {
                    datagram_processor(arr);
                }
                got_size = m_local_worker->get_queue_for_reading_data_from_source()->get(arr->data(), arr->size());
            }

        }
        else{
            this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // end code here
    }
}

