#include "Application.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Connection.h"
#include "Common.h"
#include "code/threadpool/threadpool.h"
#include <iostream>
namespace hxk
{

Application::Application() 
{
    m_mainReactor = std::make_shared<EventLoop>();
    m_acceptor = std::make_shared<Acceptor>(m_mainReactor);
    unsigned int size = std::thread::hardware_concurrency();
    m_threadpool = std::make_shared<ThreadPool>(size);

    std::function<void(int)> cb = std::bind(&Application::HandleNewConnection, this, std::placeholders::_1);
    m_acceptor->SetNewConnectionCallback(cb);

    for(int i = 0; i < size; i++) {
        m_subReactor.push_back(std::make_shared<EventLoop>(false));
    }


}

Application::~Application()
{
    
}

void Application::Start()
{
    std::cout << "Start" << std::endl;
    for(int i = 0; i < m_subReactor.size(); i++) {
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, m_subReactor[i].get());
        m_threadpool->execute(std::move(sub_loop));
    }
    std::cout << "Start" << std::endl;
    m_mainReactor->Loop();
}


void Application::HandleNewConnection(int sockfd)
{
    ERRIF(sockfd == -1, "new connection error");
    int random = sockfd % m_subReactor.size();

    std::shared_ptr<Connection> conn = std::make_shared<Connection>(m_subReactor[random], sockfd);
    std::function<void(int)> cb = std::bind(&Application::HandleDeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnCallback(cb);
    conn->SetOnRecvCallback(m_onRecv_Callback);

    m_connections[sockfd] = std::move(conn);

    if(m_onConn_Callback) {
        m_onConn_Callback(m_connections[sockfd].get());
    }
}

void Application::HandleDeleteConnection(int sockfd)
{
    if(sockfd != -1) {
        auto it = m_connections.find(sockfd);
        if(it != m_connections.end()) {
            std::shared_ptr<Connection> conn = m_connections[sockfd];
            m_connections.erase(sockfd);
            conn.reset();
        }
}
}

void Application::SetOnConnCallback(const std::function<void(Connection*)>& fn)
{
    m_onConn_Callback = std::move(fn);
}

void Application::SetOnRecvCallback(const std::function<void(Connection*)>& fn)
{
    m_onRecv_Callback = std::move(fn);
}

}