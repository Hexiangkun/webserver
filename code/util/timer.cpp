#include "timer.h"


namespace hxk
{

namespace internal
{



TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

void TimerManager::update()
{
    if(m_timers.empty()) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();

    for(auto it = m_timers.begin(); it != m_timers.end(); )
    {
        if(it->first > now) {
            return;
        }
        it->second.onTimer();//触发时间落后于当前时间，触发事件
        
        Timer timer(std::move(it->second));
        
        it = m_timers.erase(it);

        if(timer.m_count != 0) {
            const auto timepoint = timer.ptrId()->first;
            auto it_new = m_timers.insert(std::make_pair(timepoint, std::move(timer)));
            if(it_new == m_timers.end() || it_new->first < it->first) {
                it = it_new;
            }
        }
    }
}

bool TimerManager::cancel(TimerPtrId ptrId)
{
    auto begin = m_timers.lower_bound(ptrId->first);
    if(begin == m_timers.end()) {
        return false;
    }

    auto end = m_timers.upper_bound(ptrId->first);

    for(auto it = begin; it != end; it++) {
        if(it->second.uniqueId() == ptrId->second) {
            it->second.m_count = 0;
            return true;
        }
    }
    return false;
}

DurationMs TimerManager::nearestTimer() const
{
    if(m_timers.empty()) {
        return DurationMs::max();
    }

    const auto& timer = m_timers.begin()->second;
    auto now = std::chrono::steady_clock::now();

    if(now > timer.ptrId()->first) {
        return DurationMs::min();
    }
    else {
        return std::chrono::duration_cast<DurationMs>(timer.ptrId()->first - now);
    }
}

Timer::Timer(const TimePonit& tp)
        :m_ptrId(std::make_shared<std::pair<TimePonit, unsigned int>>(tp, ++TimerManager::s_timerIdGen_)),
        m_count(kForever)
{
    
}

Timer::Timer(Timer&& timer)
{
    move(std::move(timer));
}

Timer& Timer::operator=(Timer&& timer)
{
    if(this != &timer) {
        move(std::move(timer));
    }
    return *this;
}

void Timer::move(Timer&& timer)
{
    this->m_ptrId = std::move(timer.m_ptrId);
    this->m_func = std::move(timer.m_func);
    this->m_interval = std::move(timer.m_interval);
    this->m_count = timer.m_count;
}


void Timer::onTimer()
{
    if(!m_func || m_count == 0) {
        return;
    }
    if(m_count == kForever || m_count-- > 0) {
        m_func();
        m_ptrId->first += m_interval;   //更新下一个触发时间点
    }
    else {
        m_count = 0;
    }
}

TimerPtrId Timer::ptrId() const
{
    return m_ptrId;
}

unsigned int Timer::uniqueId() const
{
    return m_ptrId->second;
}

}

}