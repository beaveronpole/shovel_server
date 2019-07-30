#include <iostream>
#include <chrono>
#include <unistd.h>

#include "instance_subprocess_2.h"

using namespace std;

void signal_SIGINT_handler(int value)
{
    cout << "Process recv SIGINT!" << endl;
}

int main(int argc, char* argv[]){
    cout << "Start InstanceSubprocess_2 process! pid = " << getpid() << endl;
    if (argc < 2){
        cerr << "Error in instance_subprocess_1 arguments count. count = " << argc;
        exit(1);
    }
    InstanceSubprocess_2 subprocess_2( (std::string(argv[2])), (std::string(argv[1])) );
    subprocess_2.start();
    cout << "Finish InstanceSubprocess_2 process!!!" << endl;
    return 0;
}

