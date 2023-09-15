#include "Application.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Connection.h"

#include <iostream>

namespace hxk
{

Application::Application(std::shared_ptr<EventLoop>& loop) : m_eventLoop(loop),
                            m_acceptor(std::make_shared<Acceptor>(loop))
{
    std::function<void(std::shared_ptr<Socket>&)> cb = std::bind(&Application::HandleNewConnection, this, std::placeholders::_1);
    m_acceptor->SetNewConnectionCallback(cb);
}

Application::~Application()
{

}



void Application::HandleNewConnection(std::shared_ptr<Socket>& clnt_sock)
{
    if(clnt_sock->GetFd() != -1){
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(m_eventLoop, clnt_sock);
        std::function<void(int)> cb = std::bind(&Application::DeleteConnection, this, std::placeholders::_1);
        conn->SetDeleteConnCallback(cb);
        m_connections[clnt_sock->GetFd()] = conn;
    }
    else{
        errif(true, "clnt sock error");
    }
}

void Application::DeleteConnection(int sockfd)
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



// void Application::HandleNewConnection(Socket* serv_addr)
// {
//     InetAddress* clnt_addr = new InetAddress();
//     Socket* clnt_sock = new Socket(serv_addr->Accept(clnt_addr));
//     printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp().data(), clnt_addr->GetPort());
//     clnt_sock->SetNonBlocking();

//     Channel* clnt_channel = new Channel(m_eventLoop, clnt_sock->GetFd());
//     std::function<void()> cb = std::bind(&Application::HandleReadEvent, this, clnt_sock->GetFd());
//     clnt_channel->SetCallbck(cb);
//     clnt_channel->SetEnableReading();
// }

}