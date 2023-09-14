#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

namespace hxk
{
class EventLoop;
class Socket;
class Acceptor;

class Application
{

public:
    Application(std::shared_ptr<EventLoop>& loop);
    ~Application();

    void HandleReadEvent(int);
    // void HandleNewConnection(Socket *serv_sock);
    void HandleNewConnection(std::shared_ptr<Socket>&);
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Acceptor> m_acceptor;
};



}


#endif