#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <memory>


namespace hxk
{

namespace internal
{

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
    using _ptr = std::shared_ptr<Poller>;
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


namespace EpollCtl
{
    bool ModSocket(int epfd, int sockfd, uint32_t events, void* ptr);

    bool AddSocket(int epfd, int sockfd, uint32_t events, void* ptr);

    bool DelSocket(int epfd, int sockfd);
}


}


class Channel;
class Epoller 
{
public:
    using _ptr = std::shared_ptr<Epoller>;
    Epoller();
    ~Epoller();

    Epoller(const Epoller&) = delete;
    void operator=(const Epoller&) = delete;

    bool Register(int fd, int events, void* userPtr);
    bool Modify(int fd, int events, void* userPtr);
    bool Cancel(int fd, int events) ;

    void AddFd(int sockfd, uint32_t event);
    void ModFd(int sockfd, uint32_t event);
    void DelFd(int sockfd);

    std::vector<epoll_event> GetActiveEvents();

    int GetFD();

    void UpdateChannel(Channel* channel);
    void DeleteChannel(Channel* channel);
    std::vector<Channel*> Poll(std::size_t maxEvents, int timeoutMs = -1);

private:
    std::vector<epoll_event> m_activeEvents;
    int m_epfd;
};
}

#endif
