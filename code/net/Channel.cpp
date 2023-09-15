#include "Channel.h"
#include "EventLoop.h"

namespace hxk
{

Channel::Channel(std::shared_ptr<EventLoop>& loop, int sockfd) :m_eventLoop(loop), 
            m_sockfd(sockfd), m_events(0), m_readyevents(0), 
            m_inEpoll(false), m_useThreadPool(true)
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

void Channel::SetEvent(uint32_t _ev)
{
    m_events = _ev;
}

int Channel::GetFd()
{
    return m_sockfd;
}

uint32_t Channel::GetEvents()
{
    return m_events;
}

uint32_t Channel::GetReadyEvents()
{
    return m_readyevents;
}

bool Channel::GetInEpoll()
{
    return m_inEpoll;
}

void Channel::SetInEpoll(bool _in)
{
    m_inEpoll = _in;
}

void Channel::SetEnableReading()
{
    m_events |= EPOLLIN | EPOLLPRI;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetUseET(bool _use)
{
    if(_use) {
        m_events |= EPOLLET;
    }
    else {
        m_events = m_events & ~EPOLLET;
    }
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableRead_ET()
{
    m_events |= EPOLLIN | EPOLLPRI | EPOLLET;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetReadCallbck(std::function<void()> cb)
{   
    m_readCallback = cb;
}

void Channel::SetWriteCallback(std::function<void()> cb)
{   
    m_writeCallback = cb;
}

void Channel::SetUseThreadPool(bool _use)
{
    m_useThreadPool = _use;
}



void Channel::HandleEvent()
{
    if(m_readyevents & (EPOLLIN | EPOLLPRI)) {
        if(m_useThreadPool) {
            m_eventLoop->AddFuncToThread(m_readCallback);
        }
        else{
            m_readCallback();
        }
    }
    else if(m_readyevents & (EPOLLOUT)) {
        if(m_useThreadPool) {
            m_eventLoop->AddFuncToThread(m_writeCallback);
        }
        else {
            m_writeCallback();
        }
    }
}

}