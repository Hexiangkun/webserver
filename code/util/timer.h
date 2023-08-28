#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <map>
#include <memory>
#include <iostream>
#include <functional>
#include "noncopyable.h"

namespace hxk
{

using DurationMs = std::chrono::milliseconds;           //时间间隔
using TimePonit = std::chrono::steady_clock::time_point;    //时间点
using TimerPtrId = std::shared_ptr<std::pair<TimePonit, unsigned int>>; //一个定时器的唯一标识

constexpr int kForever = -1;    //无限次触发

inline std::ostream& operator<<(std::ostream& os, const TimerPtrId& d)
{
    os << "[TimerPtrId:" << (void*)d.get() << "]";
    return os;
}

namespace internal
{
    class TimerManager;
    class Timer
    {
        friend class TimerManager;
    public:
        explicit Timer(const TimePonit& tp);
        Timer(Timer&& timer);
        Timer& operator=(Timer&&);

        Timer(const Timer&) = delete;
        void operator=(const Timer&) = delete;

        //设置定时器触发时执行的函数和参数
        template<typename F, typename... Args>
        void setCallback(F&& f, Args&&... args);    

        //定时器触发执行的函数
        void onTimer();

        //获取定时器唯一标识
        TimerPtrId ptrId() const;

        //获取定时器唯一ID
        unsigned int uniqueId() const;

    private:
        void move(Timer&& timer);
    
    private:
        TimerPtrId m_ptrId;
        std::function<void()> m_func;
        DurationMs m_interval;
        int m_count;
    };

    class TimerManager final : public noncopyable
    {
        friend class Timer;
    public:
        TimerManager();
        ~TimerManager();

        //触发所有到期定时器
        void update();

        /// @brief 在指定的时间点开始触发定时器，重复触发一定次数，每次触发间隔为指定时间，
        /// @tparam Duration 触发间隔
        /// @tparam RepeatCount 重复次数
        /// @param triggerTime 指定时间点
        /// @param period 触发间隔
        /// @return 将F(Args...)绑定到定时器上并返回一个TimePtrId
        template<int RepeatCount, typename Duration, typename F, typename... Args>
        TimerPtrId ScheduleAtWithRepeat(const TimePonit& triggerTime, const Duration& period, F&& f, Args&&... args);

        /// @brief 在指定的时间间隔后开始触发定时器，重复触发一定次数，每次触发间隔为指定时间
        /// @tparam Duration 触发间隔
        /// @tparam RepeatCount 重复次数
        /// @return 将F(Args...)绑定到定时器上并返回一个TimePtrId
        template<int RepeatCount, typename Duration, typename F, typename... Args>
        TimerPtrId ScheduleAfterWithRepeat(const Duration& period, F&& f, Args&&... args);

        /// @brief 在指定的时间点触发定时器，只触发一次
        /// @param triggerTime 指定时间点
        /// @return 将F(Args...)绑定到定时器上并返回一个TimePtrId
        template<typename F, typename... Args>
        TimerPtrId ScheduleAt(const TimePonit& triggerTime, F&& f, Args&&... args);
        
        /// @brief 在指定时间间隔后触发定时器，只触发一次
        /// @tparam Duration 指定间隔
        /// @param duration 时间间隔
        /// @return 将F(Args...)绑定到定时器上并返回一个TimePtrId
        template<typename Duration, typename F, typename... Args>
        TimerPtrId ScheduleAfter(const Duration& duration, F&& f, Args&&... args);

        //取消指定定时器
        bool cancel(TimerPtrId ptrId);

        //返回最近一个要触发的定时器还有多长时间
        DurationMs nearestTimer() const;

    private:
        std::multimap<TimePonit, Timer> m_timers;

        static unsigned int s_timerIdGen_;
    };

    template<int RepeatCount, typename Duration, typename F, typename... Args>
    TimerPtrId TimerManager::ScheduleAtWithRepeat(const TimePonit& triggerTime, const Duration& period, F&& f, Args&&... args)
    {
        static_assert(RepeatCount != 0, "RepeatCount can't be zero!");

        Timer timer(triggerTime);
        timer.m_interval = std::max(DurationMs(1), std::chrono::duration_cast<DurationMs>(period));
        timer.m_count = RepeatCount;
        timer.setCallback(std::forward<F>(f), std::forward<Args>(args)...);

        TimerPtrId ptrId = timer.ptrId();
        m_timers.insert(std::make_pair(triggerTime, std::move(timer)));
        return ptrId;
    }

    template<int RepeatCount, typename Duration, typename F, typename... Args>
    TimerPtrId TimerManager::ScheduleAfterWithRepeat(const Duration& period, F&& f, Args&&... args)
    {
        const auto now = std::chrono::steady_clock::now();
        return ScheduleAtWithRepeat<RepeatCount>(now + period, period, std::forward<F>(f), std::forward<Args>(args)...);
    }

    template<typename F, typename... Args>
    TimerPtrId TimerManager::ScheduleAt(const TimePonit& triggerTime, F&& f, Args&&... args)
    {
        return ScheduleAtWithRepeat<1>(triggerTime,DurationMs(0), std::forward<F>(f), std::forward<Args>(args)...);
    }

    template<typename Duration, typename F, typename... Args>
    TimerPtrId TimerManager::ScheduleAfter(const Duration& period, F&& f, Args&&... args)
    {
        const auto now = std::chrono::steady_clock::now();
        return ScheduleAt(now + period, std::forward<F>(f), std::forward<Args>(args)...);
    }

    template<typename F, typename... Args>
    void Timer::setCallback(F&& f, Args&&... args)
    {
        m_func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    }
}


}

#endif