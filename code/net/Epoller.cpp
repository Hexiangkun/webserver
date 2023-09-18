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

        if(events & EventType::Read) {
            ev.events |= EPOLLIN;
        }
        if(events & EventType::Write) {
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

        if(events & EventType::Read) {
            ev.events |= EPOLLIN;
        }  
        if(events & EventType::Write) {
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
    Errif(m_epfd == -1, "Create epoll error!");
    //Debug
    std::cout << "Epoller" << m_epfd << std::endl;
}

Epoller::~Epoller()
{
    if(m_epfd != -1) {
        ::close(m_epfd);
    }
}

int Epoller::GetFD()
{
    return m_epfd;
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
    // ev.events = ch->GetListenEvents();
    if(ch->GetListenEvents() & EventType::Read) {
        ev.events |= EPOLLIN | EPOLLPRI;
    }
    if(ch->GetListenEvents() & EventType::Write) {
        ev.events |= EPOLLOUT;
    }
    if(ch->GetListenEvents() & EventType::ET) {
        ev.events |= EPOLLET;
    }
    if(!ch->GetInEpoll()){  //不在m_epfd的红黑树中
        Errif(::epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fd error!");
        ch->SetInEpoll(true);
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

std::vector<Channel*> Epoller::Poll(std::size_t maxEvents, int timeoutMs)
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
        uint32_t events = m_activeEvents[i].events;
        if(events & EPOLLIN) {
            ch->SetReadyEvent(EventType::Read);
        }
        if(events & EPOLLOUT) {
            ch->SetReadyEvent(EventType::Write);
        }
        if(events & EPOLLET){
            ch->SetReadyEvent(EventType::ET);
        }
        // ch->SetReadyEvent(m_activeEvents[i].events);
        activeChannels.emplace_back(ch);
    }
    return activeChannels;
}


}