#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "code/threadpool/threadpool.h"

namespace hxk
{
EventLoop::EventLoop() :m_ep(std::make_shared<Epoller>()), m_quit(false),
                        m_threadPool(std::make_shared<ThreadPool>())
{
    
}

EventLoop::~EventLoop()
{

}

void EventLoop::Loop()
{
    while(!m_quit) {
        std::vector<Channel*> chs = m_ep->poll(1024);
        for(auto it = chs.begin(); it != chs.end(); ++it){
            (*it)->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel* ch)
{
    m_ep->UpdateChannel(ch);
}

void EventLoop::AddFuncToThread(std::function<void()> func)
{
    m_threadPool->execute(func);
}
}