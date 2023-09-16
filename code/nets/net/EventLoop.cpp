#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"

#include <iostream>
namespace hxk
{
EventLoop::EventLoop(bool flag) 
{
    if(flag) {
        std::cout << true << std::endl;
        Create();
    }
    else {
        std::cout << false << std::endl;
    }
}

EventLoop::~EventLoop()
{

}

void EventLoop::Create()
{
    m_ep = std::make_shared<Epoller>();
}

void EventLoop::Loop() const
{
    std::cout << "Start loop" << std::endl;
    while(true) {
        std::vector<Channel*> chs = m_ep->poll(256);
        std::cout << chs.size() << std::endl;
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
}