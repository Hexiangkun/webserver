#ifndef EPOLLER_H
#define EPOLLER_H

#include "poller.h"
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <string.h>


namespace hxk
{

namespace internal
{

enum EventType
{
    eET_None = 0,           //0
    eET_Read = 0x1 << 0,    //1
    eET_Write = 0x1 << 1,   //2
    eET_Error = 0x1 << 2    //4
};

struct FiredEvent   //触发事件类
{
    int events;         //事件类型
    void* userData;     //用户数据

    FiredEvent():events(0), userData(nullptr) {}
};

class Poller
{
protected:
    int m_epfd;
    std::vector<FiredEvent> m_firedEvents;

public:
    Poller() : m_epfd(-1) {}

    virtual ~Poller() {}

    virtual bool Register(int fd, int events, void* userPtr) = 0;
    virtual bool Modify(int fd, int events, void* userPtr) = 0;
    virtual bool Cancel(int fd, int events) = 0;

    virtual int Poll(std::size_t maxEvent, int timeoutMs) = 0;      //轮询事件

    const std::vector<FiredEvent>& GetFiredEvents() const
    {
        return m_firedEvents;
    }
};

}

namespace EpollCtl
{
    using namespace internal;
    bool ModSocket(int epfd, int sockfd, uint32_t events, void* ptr)
    {
        if(sockfd < 0) {
            return false;
        }

        epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.fd = sockfd;
        ev.data.ptr = ptr;
        ev.events = 0;

        if(events & eET_Read) {
            ev.events |= EPOLLIN;
        }
        if(events & eET_Write) {
            ev.events |= EPOLLOUT;
        }

        return 0 == epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
    }

    bool AddSocket(int epfd, int sockfd, uint32_t events, void* ptr)
    {
        if(sockfd < 0) {
            return false;
        }

        epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.fd = sockfd;
        ev.data.ptr = ptr;
        ev.events = 0;

        if(events & eET_Read) {
            ev.events |= EPOLLIN;
        }  
        if(events & eET_Write) {
            ev.events |= EPOLLOUT;
        }

        return 0 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    }

    bool DelSocket(int epfd, int sockfd)
    {
        if(sockfd < 0) {
            return false;
        }

        return 0 == epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, nullptr);
    }
}


class Epoller : public internal::Poller
{
public:
    Epoller();
    ~Epoller();

    Epoller(const Epoller&) = delete;
    void operator=(const Epoller&) = delete;

    bool Register(int fd, int events, void* userPtr) override;
    bool Modify(int fd, int events, void* userPtr) override;
    bool Cancel(int fd, int events) override;

    void AddFd(int sockfd, uint32_t event);
    void ModFd(int sockfd, uint32_t event);
    void DelFd(int sockfd);

    int Poll(std::size_t maxEvents, int timeoutMs) override;

    std::vector<epoll_event> GetActiveEvents();

private:
    std::vector<epoll_event> m_activeEvents;
};
}

#endif
