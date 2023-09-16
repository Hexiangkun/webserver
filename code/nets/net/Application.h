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

    void HandleNewConnection(int);
    void HandleDeleteConnection(int);

    void SetOnConnCallback(const std::function<void(Connection*)>&);
    void SetOnRecvCallback(const std::function<void(Connection*)>&);
private:
    std::shared_ptr<EventLoop> m_mainReactor;       
    std::shared_ptr<Acceptor> m_acceptor;
    std::map<int, std::shared_ptr<Connection>> m_connections;
    std::vector<std::shared_ptr<EventLoop>> m_subReactor;
    std::shared_ptr<ThreadPool> m_threadpool;

    std::function<void(Connection*)> m_onConn_Callback;
    std::function<void(Connection*)> m_onRecv_Callback;
};



}


#endif