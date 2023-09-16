#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <memory>
#include <functional>

namespace hxk
{
class Epoller;
class Channel;


class EventLoop //事件驱动类
{

public:
    EventLoop(bool flag = true);
    ~EventLoop();

    void Create();
    void Loop() const;

    void UpdateChannel(Channel* ch) const;
    void DeleteChannel(Channel* ch) const;
private:
    std::shared_ptr<Epoller> m_ep;      //epoll
};


}

#endif