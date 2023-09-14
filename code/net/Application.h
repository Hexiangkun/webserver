#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

namespace hxk
{
class EventLoop;
class Socket;
class Application
{

public:
    Application(std::shared_ptr<EventLoop>& loop);
    ~Application();

    void HandleReadEvent(int);
    void HandleNewConnection(Socket *serv_sock);

private:
    std::shared_ptr<EventLoop> m_eventLoop;
};



}


#endif