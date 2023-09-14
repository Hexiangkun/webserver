#include "Application.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "EventLoop.h"

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

void Application::HandleReadEvent(int sockfd)
{
    char buf[1024];
    while(true) {
        bzero(buf, sizeof(buf));
        ssize_t bytes = read(sockfd, buf, sizeof(buf));

        if(bytes > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, bytes);
        }
        else if(bytes == -1 && errno == EINTR) {    //客户端正常中断，继续读取
            printf("continue reading");
            continue;
        }
        else if(bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if(bytes == 0) {
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

void Application::HandleNewConnection(std::shared_ptr<Socket>& serv_sock)
{
    // InetAddress* clnt_addr = new InetAddress();
    std::shared_ptr<InetAddress> clnt_addr = std::make_shared<InetAddress>();
    Socket* clnt_sock = new Socket(serv_sock->Accept(clnt_addr));

    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp().c_str(), clnt_addr->GetPort());
    clnt_sock->SetNonBlocking();

    Channel* clnt_channel = new Channel(m_eventLoop, clnt_sock->GetFd());
    std::function<void()> cb = std::bind(&Application::HandleReadEvent,this, clnt_sock->GetFd());
    clnt_channel->SetCallbck(cb);
    clnt_channel->SetEnableReading();
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