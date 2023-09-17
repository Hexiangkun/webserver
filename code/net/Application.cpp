#include "Application.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Connection.h"
#include "code/threadpool/Threadpool.h"

#include <iostream>

namespace hxk
{

Application::Application() :m_mainReactor(std::make_shared<EventLoop>()),
                        m_acceptor(std::make_shared<Acceptor>(m_mainReactor)),
                        m_threadpool(std::make_shared<ThreadPool>(std::thread::hardware_concurrency()))
{
    std::function<void(std::shared_ptr<Socket>&)> cb = std::bind(&Application::HandleNewConnection, this, std::placeholders::_1);
    m_acceptor->SetNewConnectionCallback(cb);

    for(int i = 0; i < std::thread::hardware_concurrency(); i++) {
        m_subReactor.emplace_back(std::make_shared<EventLoop>());
    }

    for(int i = 0; i < std::thread::hardware_concurrency(); i++) {
        std::function<void()> sub_loop = std::bind(&EventLoop::Loop, m_subReactor[i]);
        m_threadpool->execute(std::move(sub_loop));
    }
}

Application::~Application()
{

}

void Application::Start()
{
    m_mainReactor->Loop();
}


void Application::HandleNewConnection(std::shared_ptr<Socket>& clnt_sock)
{
    if(clnt_sock->GetFd() != -1){
        int random = clnt_sock->GetFd() % m_subReactor.size();

        std::shared_ptr<Connection> conn = std::make_shared<Connection>(m_subReactor[random], clnt_sock);
        std::function<void(int)> cb = std::bind(&Application::HandleDeleteConnection, this, std::placeholders::_1);
        conn->SetDeleteConnCallback(cb);
        conn->SetOnRecvCallback(m_onRecv);
        m_connections[clnt_sock->GetFd()] = std::move(conn);

        if(m_onConnect) {
            m_onConnect(m_connections[clnt_sock->GetFd()].get());
        }
    }
    else{
        Errif(true, "clnt sock error");
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

void Application::SetOnConnect(const std::function<void(Connection*)>& cb)
{
    m_onConnect = std::move(cb);
}

void Application::SetOnRecv(const std::function<void(Connection*)>& cb)
{
    m_onRecv = std::move(cb);
}

}