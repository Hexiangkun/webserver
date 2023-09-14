#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <memory>
#include <functional>

namespace hxk
{
class Epoller;
class Channel;
class ThreadPool;

class EventLoop //事件驱动类
{

public:
    EventLoop(/* args */);
    ~EventLoop();


    void Loop();

    void UpdateChannel(Channel* ch);

    void AddFuncToThread(std::function<void()>);
private:
    std::shared_ptr<Epoller> m_ep;
    std::shared_ptr<ThreadPool> m_threadPool;
    bool m_quit;
};


}

#endif