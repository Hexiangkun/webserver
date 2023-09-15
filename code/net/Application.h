#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <map>
#include <vector>

namespace hxk
{
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Application
{

public:
    Application(std::shared_ptr<EventLoop>& loop);
    ~Application();


    void HandleNewConnection(std::shared_ptr<Socket>&);
    void DeleteConnection(int);
private:
    std::shared_ptr<EventLoop> m_mainReactor;
    std::shared_ptr<Acceptor> m_acceptor;
    std::map<int, std::shared_ptr<Connection>> m_connections;
    std::vector<std::shared_ptr<EventLoop>> m_subReactor;
    std::shared_ptr<ThreadPool> m_threadpool;
};



}


#endif