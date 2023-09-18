#include "Channel.h"
#include "EventLoop.h"
#include "Common.h"

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


void Channel::SetReadyEvent(int _ev)
{
    // m_readyevents = _ev;
    if(_ev & EventType::Read) {
        m_readyevents |= EventType::Read;
    }
    if(_ev & EventType::Write) {
        m_readyevents |= EventType::Write;
    }

    if(_ev & EventType::ET) {
        m_readyevents |= EventType::ET;
    }
}

void Channel::SetListenEvent(int _ev)
{
    // m_listenevents = _ev;
    if(_ev & EventType::Read) {
        m_listenevents |= EventType::Read;
    }
    if(_ev & EventType::Write) {
        m_listenevents |= EventType::Write;
    }

    if(_ev & EventType::ET) {
        m_listenevents |= EventType::ET;
    }
}

int Channel::GetFd() const
{
    return m_sockfd;
}

int Channel::GetListenEvents() const
{
    return m_listenevents;
}

int Channel::GetReadyEvents() const
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
    // m_listenevents |= EPOLLIN | EPOLLPRI;
    m_listenevents |= EventType::Read;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableWrite()
{
    m_listenevents |= EventType::Write;
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableET(bool _use)
{
    if(_use) {
        // m_listenevents |= EPOLLET;
        m_listenevents |= EventType::ET;
    }
    else {
        // m_listenevents = m_listenevents & ~EPOLLET;
        m_listenevents = m_listenevents & ~EventType::ET;
    }
    m_eventLoop->UpdateChannel(this);
}

void Channel::SetEnableRead_ET()
{
    // m_listenevents |= EPOLLIN | EPOLLPRI | EPOLLET;
    m_listenevents |= EventType::Read | EventType::ET;
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
    if(m_readyevents & EventType::Read) {
        m_readCallback();
    }
    if(m_readyevents & EventType::Write) {
        m_writeCallback();
    }
    // if(m_readyevents & (EPOLLIN | EPOLLPRI)) {
    //     m_readCallback();
    // }
    // else if(m_readyevents & (EPOLLOUT)) {
    //     m_writeCallback();
    // }
}

}