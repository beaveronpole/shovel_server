#include <iostream>
#include <chrono>
#include <unistd.h>

#include "instance_subprocess_1.h"

using namespace std;

void signal_SIGINT_handler(int value)
{
    cout << "Process recv SIGINT!" << endl;
}

int main(int argc, char* argv[]){
    cout << "Start instance_subprocess_1 process! pid = " << getpid() << endl;
    if (argc < 2){
        cerr << "Error in instance_subprocess_1 arguments count. count = " << argc;
        exit(1);
    }
    InstanceSubprocess_1 subprocess_1( (std::string(argv[2])), (std::string(argv[1])) );
    subprocess_1.start();
    cout << "Finish instance_subprocess_1 process!!!" << endl;
    return 0;
}

