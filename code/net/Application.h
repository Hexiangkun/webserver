#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <map>

namespace hxk
{
class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Application
{

public:
    Application(std::shared_ptr<EventLoop>& loop);
    ~Application();
    // void HandleNewConnection(Socket *serv_sock);
    void HandleNewConnection(std::shared_ptr<Socket>&);
    void DeleteConnection(std::shared_ptr<Socket>&);
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Acceptor> m_acceptor;
    std::map<int, std::shared_ptr<Connection>> m_connections;
};



}


#endif