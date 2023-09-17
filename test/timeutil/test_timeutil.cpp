#include "code/util/TimeUtil.h"
#include <iostream>
#include <thread>
int main()
{
    hxk::TimeUtil now;
    std::cout << now.microSeconds() << now.getDay() << std::endl;
    char temp[32];
    size_t end = now.formatTime(temp);
    temp[end] ='\0';
    std::cout << now.formatTime() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << now.microSeconds() << now.getDay() << std::endl;
    std::cout << now.formatTime() << std::endl;
}