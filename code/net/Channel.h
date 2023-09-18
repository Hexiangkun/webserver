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

    void SetReadyEvent(int);   //设置触发的事件
    void SetListenEvent(int);  //设置监听的事件

    int GetFd() const;

    int GetListenEvents() const;
    int GetReadyEvents() const;

    bool GetInEpoll() const;
    void SetInEpoll(bool _in = true);

    void SetEnableRead();
    void SetEnableWrite();

    void SetEnableET(bool _use = true);
    void SetEnableRead_ET();

    void SetReadCallbck(const std::function<void()>& );
    void SetWriteCallback(const std::function<void()>&);

    void HandleEvent();
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    int m_sockfd;

    int m_listenevents;
    int m_readyevents;

    bool m_inEpoll;

};


}


#endif