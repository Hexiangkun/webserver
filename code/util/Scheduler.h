#ifndef SCHEDULER_H
#define SCHEFULER_H

#include <chrono>
#include <functional>

namespace hxk
{
class Scheduler
{
public:
    virtual void ScheduleLater(std::chrono::milliseconds durations, std::function<void()> f) = 0;
    virtual void Schedule(std::function<void()> f) = 0;
    virtual ~Scheduler() {}
};


}

#endif