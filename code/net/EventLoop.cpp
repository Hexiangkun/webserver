#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"

namespace hxk
{
    
EventLoop::EventLoop(): m_ep(std::make_shared<Epoller>())
{

}

EventLoop::~EventLoop()
{

}

void EventLoop::Loop() const
{
    while(true) {
        std::vector<Channel*> chs = m_ep->Poll(1024);
        for(auto it = chs.begin(); it != chs.end(); ++it){
            (*it)->HandleEvent();
        }
    }
}


void EventLoop::UpdateChannel(Channel* ch) const
{
    m_ep->UpdateChannel(ch);
}

void EventLoop::DeleteChannel(Channel* ch) const
{
    m_ep->DeleteChannel(ch);
}

void EventLoop::ScheduleLater(std::chrono::milliseconds, std::function<void()> f)
{

}

void EventLoop::Schedule(std::function<void()> f)
{
    
}

}