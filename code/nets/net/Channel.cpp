#include "Channel.h"
#include "EventLoop.h"

namespace hxk
{

Channel::Channel(std::shared_ptr<EventLoop>& loop, int sockfd) 
            :m_eventLoop(loop), m_sockfd(sockfd), 
            m_listen_events(0), m_ready_events(0), 
            m_inEpoll(false)
{

}

Channel::~Channel()
{
    m_eventLoop->DeleteChannel(this);
}

void Channel::SetReadyEvent(uint32_t _ev)
{
    m_ready_events = _ev;
}

void Channel::SetListenEvent(uint32_t _ev)
{
    m_listen_events = _ev;
}

int Channel::GetFd() const
{
    return m_sockfd;
}

uint32_t Channel::GetListenEvents() const
{
    return m_listen_events;
}

uint32_t Channel::GetReadyEvents() const
{
    return m_ready_events;
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
    m_listen_events |= EPOLLIN | EPOLLPRI;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableWrite()
{
    
}

void Channel::SetEnableET(bool _use)
{
    if(_use) {
        m_listen_events |= EPOLLET;
    }
    else {
        m_listen_events = m_listen_events & ~EPOLLET;
    }
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableRead_ET()
{
    m_listen_events |= EPOLLIN | EPOLLPRI | EPOLLET;
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
    if(m_ready_events & (EPOLLIN | EPOLLPRI)) {
        m_readCallback();
    }
    else if(m_ready_events & (EPOLLOUT)) {
        m_writeCallback();
    }
}

}