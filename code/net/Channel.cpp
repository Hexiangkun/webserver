#include "Channel.h"
#include "Epoller.h"
namespace hxk
{

Channel::Channel(std::shared_ptr<Epoller> ep, int sockfd) :m_epPtr(ep), m_sockfd(sockfd),
            m_events(0), m_revents(0), m_inEpoll(false)
{

}

Channel::~Channel()
{

}


void Channel::SetEnableReading()
{
    m_events = EPOLLIN | EPOLLET;
    m_epPtr->UpdateChannel(this);
}

int Channel::GetFd()
{
    return m_sockfd;
}

uint32_t Channel::GetEvents()
{
    return m_events;
}

uint32_t Channel::GetREvents()
{
    return m_revents;
}

bool Channel::GetInEpoll()
{
    return m_inEpoll;
}

void Channel::SetInEpoll()
{
    m_inEpoll = true;
}

void Channel::SetEvent(uint32_t _ev)
{
    m_events = _ev;
}

void Channel::SetREvent(uint32_t _ev)
{
    m_revents = _ev;
}

}