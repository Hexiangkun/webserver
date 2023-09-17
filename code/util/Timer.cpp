#include "Timer.h"


namespace hxk
{

namespace internal
{
thread_local unsigned int Timer::s_timerIdGen_ = 0;
Timer::Timer(const TimePonit& tp, int _count)
        :m_ptrId(std::make_shared<std::pair<TimePonit, unsigned int>>(tp, ++Timer::s_timerIdGen_)),
        m_count(_count)
{
    
}

Timer::Timer(Timer&& timer)
{
    _Move(std::move(timer));
}

Timer& Timer::operator=(Timer&& timer)
{
    if(this != &timer) {
        _Move(std::move(timer));
    }
    return *this;
}

void Timer::_Move(Timer&& timer)
{
    this->m_ptrId = std::move(timer.m_ptrId);
    this->m_func = std::move(timer.m_func);
    this->m_interval = std::move(timer.m_interval);
    this->m_count = timer.m_count;
}

void Timer::SetCallback(std::function<void()> cb)
{
    m_func = std::move(cb);
}

void Timer::OnTimer()
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

TimerPtrId Timer::GetPtrId() const
{
    return m_ptrId;
}

unsigned int Timer::GetUniqueId() const
{
    return m_ptrId->second;
}

int Timer::GetTickCount() const
{
    return m_count;
}

void Timer::SetTickCount(int _count)
{
    m_count = _count;
}

void Timer::SetInterval(DurationMs interval)
{
    m_interval = interval;
}

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

//触发所有到期定时器
void TimerManager::Tick()
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
        it->second.OnTimer();//触发时间落后于当前时间，触发事件，更新触发时间
        
        Timer timer(std::move(it->second));
        
        it = m_timers.erase(it);

        if(timer.GetTickCount() != 0) {
            const auto timepoint = timer.GetPtrId()->first;
            auto it_new = m_timers.insert(std::make_pair(timepoint, std::move(timer)));
            if(it_new == m_timers.end() || it_new->first < it->first) {
                it = it_new;
            }
        }
        else{

        }
    }
}

bool TimerManager::Cancel(TimerPtrId ptrId)
{
    auto begin = m_timers.lower_bound(ptrId->first);
    if(begin == m_timers.end()) {
        return false;
    }

    auto end = m_timers.upper_bound(ptrId->first);

    for(auto it = begin; it != end; it++) {
        if(it->second.GetUniqueId() == ptrId->second) {
            it->second.SetTickCount(0);
            return true;
        }
    }
    return false;
}

DurationMs TimerManager::GetNearestTimer() const
{
    if(m_timers.empty()) {
        return DurationMs::max();
    }

    const auto& timer = m_timers.begin()->second;
    auto now = std::chrono::steady_clock::now();

    if(now > timer.GetPtrId()->first) {
        return DurationMs::min();
    }
    else {
        return std::chrono::duration_cast<DurationMs>(timer.GetPtrId()->first - now);
    }
}



}

}