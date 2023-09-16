#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <map>
#include <vector>
#include <functional>
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
    Application();
    ~Application();

    void Start();

    void HandleNewConnection(std::shared_ptr<Socket>&);
    void HandleDeleteConnection(int);

    void SetOnConnect(const std::function<void(Connection*)>&);
    void SetOnRecv(const std::function<void(Connection*)>&);
private:
    std::shared_ptr<EventLoop> m_mainReactor;
    std::shared_ptr<Acceptor> m_acceptor;
    std::map<int, std::shared_ptr<Connection>> m_connections;
    std::vector<std::shared_ptr<EventLoop>> m_subReactor;
    std::shared_ptr<ThreadPool> m_threadpool;

    std::function<void(Connection*)> m_onConnect;
    std::function<void(Connection*)> m_onRecv;
};



}


#endif