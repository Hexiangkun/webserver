#include "Epoller.h"
#include "Common.h"
#include "Channel.h"
#include <iostream>
namespace hxk
{



namespace internal
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
}

using namespace internal;
Epoller::Epoller()
{
    m_epfd = ::epoll_create(512);
    //Debug
    std::cout << "Epoller" << m_epfd << std::endl;
}

Epoller::~Epoller()
{
    if(m_epfd != -1) {
        ::close(m_epfd);
    }
}


bool Epoller::Register(int fd, int events, void* userPtr) 
{
    if(EpollCtl::AddSocket(m_epfd, fd, events, userPtr)) {
        return true;
    }
    return (errno == EEXIST) && Modify(fd, events, userPtr);
}

bool Epoller::Modify(int fd, int events, void* userPtr) 
{
    if(events == 0) {
        return Cancel(fd, 0);
    }

    if(EpollCtl::ModSocket(m_epfd, fd, events, userPtr)) {
        return true;
    }

    return errno == ENOENT && Register(fd, events, userPtr);
}
bool Epoller::Cancel(int fd, int events) 
{
    return EpollCtl::DelSocket(m_epfd, fd);
}

void Epoller::AddFd(int sockfd, uint32_t event)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = event;

    Errif(::epoll_ctl(m_epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll add fd event error!");
}

void Epoller::ModFd(int sockfd, uint32_t event)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = event;
    Errif(::epoll_ctl(m_epfd, EPOLL_CTL_MOD, sockfd, &ev) == -1, "epoll mod fd event error!");
}

void Epoller::DelFd(int sockfd)
{
    Errif(::epoll_ctl(m_epfd, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll del fd event error!");
}

int Epoller::Poll(std::size_t maxEvents, int timeoutMs) 
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

std::vector<epoll_event> Epoller::GetActiveEvents()
{
    return m_activeEvents;
}

void Epoller::UpdateChannel(Channel* ch)
{
    int fd = ch->GetFd();

    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = ch;
    ev.events = ch->GetListenEvents();
    
    if(!ch->GetInEpoll()){  //不在m_epfd的红黑树中
        Errif(::epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fd error!");
        ch->SetInEpoll();
    }
    else {
        Errif(::epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll mode fd error!");
    }
}


void Epoller::DeleteChannel(Channel* ch)
{
    int sockfd = ch->GetFd();
    Errif(::epoll_ctl(m_epfd, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll del error");
    ch->SetInEpoll(false);
}

std::vector<Channel*> Epoller::poll(std::size_t maxEvents, int timeoutMs)
{
    if(maxEvents == 0) {
        return {};
    }
    while(m_activeEvents.size() < maxEvents) {
        m_activeEvents.resize(2 * m_activeEvents.size() + 1);
    }
    std::vector<Channel*> activeChannels;
    int nfds = ::epoll_wait(m_epfd, &m_activeEvents[0], maxEvents, timeoutMs);
    Errif(nfds == -1, "epoll wait error");

    for(int i = 0; i < nfds; i++) {
        Channel* ch = (Channel*)m_activeEvents[i].data.ptr;
        ch->SetReadyEvent(m_activeEvents[i].events);
        activeChannels.emplace_back(ch);
    }
    return activeChannels;
}


}