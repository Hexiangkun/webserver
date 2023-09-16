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
    void SetListenEvent(uint32_t);

    int GetFd() const;

    uint32_t GetListenEvents() const;
    uint32_t GetReadyEvents() const;

    bool GetInEpoll() const;
    void SetInEpoll(bool _in = true);

    void SetEnableRead();
    void SetEnableWrite();
    void SetEnableET(bool _use = true);
    void SetEnableRead_ET();

    void SetReadCallbck(const std::function<void()>& cb);
    void SetWriteCallback(const std::function<void()>& cb);

    void HandleEvent();
private:
    std::shared_ptr<EventLoop> m_eventLoop; //事件驱动
    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    int m_sockfd;   //当前channel管理的文件描述符

    uint32_t m_listen_events;   //监听的事件
    uint32_t m_ready_events;    //触发的事件

    bool m_inEpoll; //是否在epoll的红黑树中

};


}


#endif