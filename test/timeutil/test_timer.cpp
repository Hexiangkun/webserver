#include "code/util/Timer.h"
#include <iostream>
#include <thread>
using namespace hxk::internal;

void Test_ScheduleAtWithRepeat()
{
    TimerManager tm;
    auto fn = [](int num){
        std::cout << num << std::endl;
    };
    tm.ScheduleAtWithRepeat<1>(std::chrono::steady_clock::now(),
        std::chrono::milliseconds(10000),fn, 11);
    
    tm.ScheduleAtWithRepeat<2>(std::chrono::steady_clock::now(),
        std::chrono::milliseconds(1000),fn, 22);
    
    tm.Tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    tm.Tick();

}

void Test_ScheduleAfterWithRepeat()
{
    TimerManager tm;
    auto fn = [](int num){
        std::cout << num << std::endl;
    };
    tm.ScheduleAfterWithRepeat<2>(std::chrono::milliseconds(1000),fn, 22);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    tm.Tick();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    tm.Tick();
}

void Test_ScheduleAt()
{
    TimerManager tm;
    auto fn = [](int num){
        std::cout << num << std::endl;
    };
    tm.ScheduleAt(std::chrono::steady_clock::now() + std::chrono::milliseconds(1000),fn, 11);  
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    tm.Tick();  
}

void Test_ScheduleAfter()
{
    TimerManager tm;
    auto fn = [](int num){
        std::cout << num << std::endl;
    };
    tm.ScheduleAfter(std::chrono::milliseconds(1000),fn, 11);  
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    tm.Tick();
}

int main()
{
    Test_ScheduleAtWithRepeat();
    Test_ScheduleAfterWithRepeat();
    Test_ScheduleAt();
    Test_ScheduleAfter();

    return 0;
}