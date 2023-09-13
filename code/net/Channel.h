#ifndef CHANNEL_H
#define CHANNEL_H

#include <memory>


namespace hxk
{
class Epoller;
//Channel必须说明它与哪个epoll和fd绑定
class Channel
{
public:
    using Ptr_ = std::shared_ptr<Channel>;
    Channel(std::shared_ptr<Epoller> ep, int sockfd);
    ~Channel();

    void SetREvent(uint32_t);   //设置接受到的事件
    void SetEvent(uint32_t);

    int GetFd();

    uint32_t GetEvents();
    uint32_t GetREvents();

    bool GetInEpoll();
    void SetInEpoll();

    void SetEnableReading();

private:
    std::shared_ptr<Epoller> m_epPtr;
    int m_sockfd;
    uint32_t m_events;
    uint32_t m_revents;
    bool m_inEpoll;
};


}


#endif