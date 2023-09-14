#include "Connection.h"
#include "Socket.h"
#include "Channel.h"

namespace hxk
{


Connection::Connection(std::shared_ptr<EventLoop>& loop, std::shared_ptr<Socket>& sock):m_eventLoop(loop),
        m_sock(sock), m_buffer(std::make_shared<Buffer>()),m_channel(nullptr)
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
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            m_buffer->pushData(buf, bytes_read);

        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, m_buffer->to_string().c_str());
            errif(write(sockfd, m_buffer->to_string().c_str(), m_buffer->to_string().size()) == -1, "socket write error");
            m_buffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            m_deleteConnCallback(m_sock);
            break;
        }
    }
}
}