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
    EventLoop(/* args */);
    ~EventLoop();

    void Loop();

    void UpdateChannel(Channel* ch);
private:
    std::shared_ptr<Epoller> m_ep;
    bool m_quit;
};


}

#endif