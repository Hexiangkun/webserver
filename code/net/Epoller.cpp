#include "Epoller.h"
#include "code/util/util.h"

namespace hxk
{

using namespace internal;

Epoller::Epoller()
{
    m_epfd = ::epoll_create(512);
    //Debug
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

    errif(::epoll_ctl(m_epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll add fd event error!");
}

void Epoller::ModFd(int sockfd, uint32_t event)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = event;
    errif(::epoll_ctl(m_epfd, EPOLL_CTL_MOD, sockfd, &ev) == -1, "epoll mod fd event error!");
}

void Epoller::DelFd(int sockfd)
{
    errif(::epoll_ctl(m_epfd, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll del fd event error!");
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


}