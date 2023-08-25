#include "timeUtil.h"
#include <iostream>

int main()
{
    hxk::TimeUtil now;
    std::cout << now.microSeconds() << now.getDay() << std::endl;
    char temp[32];
    size_t end = now.formatTime(temp);
    temp[end] ='\0';
    std::cout << now.formatTime() << std::endl;
}