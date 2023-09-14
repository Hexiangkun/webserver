#include "Application.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"

#include <iostream>

namespace hxk
{

Application::Application(std::shared_ptr<EventLoop>& loop) : m_eventLoop(loop)
{
    Socket *serv_sock = new Socket();
    InetAddress* serv_addr = new InetAddress("127.0.0.1", 8888);

    serv_sock->Bind(serv_addr);
    serv_sock->Listen();
    serv_sock->SetNonBlocking();    //设置套接字非阻塞

    Channel* serv_channel = new Channel(m_eventLoop, serv_sock->GetFd());
    
    std::function<void()> cb = std::bind(&Application::HandleNewConnection, this, serv_sock);
    serv_channel->SetCallbck(cb);
    serv_channel->SetEnableReading();   //设置ET模式和IN事件

    std::cout << "Application" << std::endl;
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

void Application::HandleNewConnection(Socket* serv_addr)
{
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(serv_addr->Accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp().data(), clnt_addr->GetPort());
    clnt_sock->SetNonBlocking();

    Channel* clnt_channel = new Channel(m_eventLoop, clnt_sock->GetFd());
    std::function<void()> cb = std::bind(&Application::HandleReadEvent, this, clnt_sock->GetFd());
    clnt_channel->SetCallbck(cb);
    clnt_channel->SetEnableReading();
}

}