#ifndef EPOLLER_H
#define EPOLLER_H

#include "poller.h"
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace hxk::internal;

namespace hxk
{

namespace EpollCtl
{
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


class Epoller : public Poller
{
public:
    Epoller()
    {
        m_epfd = ::epoll_create(512);
        //Debug
    }

    ~Epoller()
    {
        if(m_epfd != -1) {
            ::close(m_epfd);
        }
    }

    Epoller(const Epoller&) = delete;
    void operator=(const Epoller&) = delete;

    bool Register(int fd, int events, void* userPtr) override
    {
        if(EpollCtl::AddSocket(m_epfd, fd, events, userPtr)) {
            return true;
        }
        return (errno == EEXIST) && Modify(fd, events, userPtr);
    }

    bool Modify(int fd, int events, void* userPtr) override
    {
        if(events == 0) {
            return Cancel(fd, 0);
        }

        if(EpollCtl::ModSocket(m_epfd, fd, events, userPtr)) {
            return true;
        }

        return errno == ENOENT && Register(fd, events, userPtr);
    }
    bool Cancel(int fd, int events) override
    {
        return EpollCtl::DelSocket(m_epfd, fd);
    }

    void AddFd(int sockfd, uint32_t event)
    {
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.fd = sockfd;
        ev.events = event;

        errif(::epoll_ctl(m_epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll add fd event error!");
    }

    void ModFd(int sockfd, uint32_t event)
    {
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.fd = sockfd;
        ev.events = event;
        errif(::epoll_ctl(m_epfd, EPOLL_CTL_MOD, sockfd, &ev) == -1, "epoll mod fd event error!");
    }

    void DelFd(int sockfd)
    {
        errif(::epoll_ctl(m_epfd, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll del fd event error!");
    }

    int Poll(std::size_t maxEvents, int timeoutMs) override
    {
        if(maxEvents == 0) {
            return 0;
        }
        while(m_activeEvents.size() < maxEvents) {
            m_activeEvents.resize(2 * m_activeEvents.size() + 1);
        }

        int nFired = TEMP_FAILURE_RETRY(::epoll_wait(m_epfd, &m_activeEvents[0], maxEvents, timeoutMs));

        if(nFired == -1 && errno != EINTR && errno != EWOULDBLOCK) {
            return -1;
        }

        auto& firedEvents = m_firedEvents;
        if(nFired > 0) {
            std::vector<FiredEvent>().swap(firedEvents);
            firedEvents.resize(nFired);
        }

        for(int i = 0; i < nFired; ++i) {
            FiredEvent& fired = firedEvents[i];
            fired.events = 0;
            fired.userData = m_activeEvents[i].data.ptr;

            if(m_activeEvents[i].events & EPOLLIN) {
                fired.events |= eET_Read;
            }
            if(m_activeEvents[i].events & EPOLLOUT) {
                fired.events |+ eET_Write;
            }

            if(m_activeEvents[i].events & (EPOLLERR | EPOLLHUP)) {
                fired.events |= eET_Error;
            }
        }
        return nFired;
    }

    std::vector<epoll_event> GetActiveEvents()
    {
        return m_activeEvents;
    }

private:
    std::vector<epoll_event> m_activeEvents;
};
}

#endif
