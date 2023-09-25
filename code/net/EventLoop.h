#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <memory>
#include <functional>
#include "code/util/Scheduler.h"
#include "code/util/Timer.h"
#include "code/future/Future.h"

namespace hxk
{
class Epoller;
class Channel;

class EventLoop  : public Scheduler //事件驱动类
{

public:
    EventLoop();
    ~EventLoop();

    void Loop() const;

    void UpdateChannel(Channel* ch) const;
    void DeleteChannel(Channel* ch) const;

    void ScheduleLater(std::chrono::milliseconds, std::function<void()>) override;
    void Schedule(std::function<void()>) override;

    template<int RepeatCount, typename Duration, typename F, typename... Args>
    TimerPtrId ScheduleAtWithRepeat(const TimePonit&, const Duration& , F&&, Args&&...);

    template<int RepeatCount, typename Duration, typename F, typename... Args>
    TimerPtrId ScheduleAfterWithRepeat(const Duration& , F&&, Args&&...);

    template<typename F, typename... Args>
    TimerPtrId ScheduleAt(const TimePonit&, F&&, Args&&...);

    template<typename Duration, typename F, typename... Args>
    TimerPtrId ScheduleAfter(const Duration&, F&&, Args&&...);

    template<typename F, typename... Args,
            typename = typename std::enable_if<!std::is_void<typename std::result_of<F(Args...)>::type>::value, void>::type,
            typename Dummy = void>
    auto Execute(F&&, Args&&...) -> Future<typename std::result_of<F(Args...)>::type>;

    template<typename F, typename... Args,
            typename = typename std::enable_if<std::is_void<typename std::result_of<F(Args...)>::type>::value, void>::type>
    auto Execute(F&&, Args&&...) -> Future<void>;

private:
    std::shared_ptr<Epoller> m_ep;
};

template<int RepeatCount, typename Duration, typename F, typename... Args>
TimerPtrId EventLoop::ScheduleAtWithRepeat(const TimePonit&, const Duration& , F&&, Args&&...)
{
    
}



}

#endif