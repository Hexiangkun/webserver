#include "Connection.h"
#include "Socket.h"
#include "Channel.h"

namespace hxk
{


Connection::Connection(std::shared_ptr<EventLoop>& loop, std::shared_ptr<Socket>& sock):m_eventLoop(loop),
        m_sock(sock), m_channel(nullptr)
{
    m_channel = new Channel(loop, m_sock->GetFd());
    std::function<void()> cb = std::bind(&Connection::HandleReadEvent, this, m_sock->GetFd());
    m_channel->SetCallbck(cb);
    m_channel->SetEnableReading();
}

Connection::~Connection()
{
    delete m_channel;
}

void Connection::SetDeleteConnCallback(std::function<void(std::shared_ptr<Socket>&)> cb)
{
    m_deleteConnCallback = cb;
}


void Connection::HandleReadEvent(int sockfd)
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
}