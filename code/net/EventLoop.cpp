#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include <iostream>
namespace hxk
{
EventLoop::EventLoop() :m_ep(std::make_shared<Epoller>()), m_quit(false)
{
    
}

EventLoop::~EventLoop()
{

}

void EventLoop::Loop()
{
    while(!m_quit) {
        std::vector<Channel*> chs = m_ep->poll(1024);
        std::cout << chs.size() << std::endl;
        for(auto it = chs.begin(); it != chs.end(); ++it){
            (*it)->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel* ch)
{
    m_ep->UpdateChannel(ch);
}
}