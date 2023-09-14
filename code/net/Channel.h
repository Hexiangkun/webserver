#ifndef CHANNEL_H
#define CHANNEL_H

#include <memory>
#include <functional>

namespace hxk
{
class EventLoop;
//Channel必须说明它与哪个epoll和fd绑定
class Channel
{
public:

    Channel(std::shared_ptr<EventLoop>& loop, int sockfd);
    ~Channel();

    void SetREvent(uint32_t);   //设置接受到的事件
    void SetEvent(uint32_t);

    int GetFd();

    uint32_t GetEvents();
    uint32_t GetREvents();

    bool GetInEpoll();
    void SetInEpoll();

    void SetEnableReading();

    void HandleEvent();

    void SetCallbck(std::function<void()>);
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::function<void()> m_callback;
    int m_sockfd;
    uint32_t m_events;
    uint32_t m_revents;
    bool m_inEpoll;
};


}


#endif