#ifndef CHANNEL_H
#define CHANNEL_H

#include <memory>
#include <functional>
#include <sys/epoll.h>
#include <unistd.h>

namespace hxk
{
class EventLoop;
//Channel必须说明它与哪个epoll和fd绑定
class Channel
{
public:

    Channel(std::shared_ptr<EventLoop>& loop, int sockfd);
    ~Channel();

    void SetReadyEvent(uint32_t);   //设置接受到的事件
    void SetEvent(uint32_t);

    int GetFd();

    uint32_t GetEvents();
    uint32_t GetReadyEvents();

    bool GetInEpoll();
    void SetInEpoll(bool _in = true);

    void SetEnableReading();
    void SetUseET(bool _use = true);
    void SetEnableRead_ET();

    void SetReadCallbck(std::function<void()>);
    void SetWriteCallback(std::function<void()>);

    void HandleEvent();
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    int m_sockfd;

    uint32_t m_events;
    uint32_t m_readyevents;

    bool m_inEpoll;

};


}


#endif