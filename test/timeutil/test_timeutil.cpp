#include "code/util/TimeStamp.h"
#include <iostream>
#include <thread>
int main()
{
    hxk::TimeStamp now;
    std::cout << now.milliSeconds() << std::endl;
    std::cout << now.microSeconds() << std::endl;
    std::cout << now.formatTime() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << now.now().formatTime() << std::endl;
}