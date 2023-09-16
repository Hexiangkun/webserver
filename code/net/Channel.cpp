#include "Channel.h"
#include "EventLoop.h"

namespace hxk
{

Channel::Channel(std::shared_ptr<EventLoop>& loop, int sockfd) :m_eventLoop(loop), 
            m_sockfd(sockfd), m_listenevents(0), m_readyevents(0), 
            m_inEpoll(false)
{

}

Channel::~Channel()
{
    if(m_sockfd != -1){
        close(m_sockfd);
        m_sockfd = -1;
    }
}

void Channel::SetReadyEvent(uint32_t _ev)
{
    m_readyevents = _ev;
}

void Channel::SetListenEvent(uint32_t _ev)
{
    m_listenevents = _ev;
}

int Channel::GetFd() const
{
    return m_sockfd;
}

uint32_t Channel::GetListenEvents() const
{
    return m_listenevents;
}

uint32_t Channel::GetReadyEvents() const
{
    return m_readyevents;
}

bool Channel::GetInEpoll() const
{
    return m_inEpoll;
}

void Channel::SetInEpoll(bool _in)
{
    m_inEpoll = _in;
}

void Channel::SetEnableRead()
{
    m_listenevents |= EPOLLIN | EPOLLPRI;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableET(bool _use)
{
    if(_use) {
        m_listenevents |= EPOLLET;
    }
    else {
        m_listenevents = m_listenevents & ~EPOLLET;
    }
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableRead_ET()
{
    m_listenevents |= EPOLLIN | EPOLLPRI | EPOLLET;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetReadCallbck(const std::function<void()>& cb)
{   
    m_readCallback = std::move(cb);
}

void Channel::SetWriteCallback(const std::function<void()>& cb)
{   
    m_writeCallback = std::move(cb);
}


void Channel::HandleEvent()
{
    if(m_readyevents & (EPOLLIN | EPOLLPRI)) {
        m_readCallback();
    }
    else if(m_readyevents & (EPOLLOUT)) {
        m_writeCallback();
    }
}

}