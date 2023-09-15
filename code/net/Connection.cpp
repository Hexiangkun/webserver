#include "Connection.h"
#include "Socket.h"
#include "Channel.h"

namespace hxk
{


Connection::Connection(std::shared_ptr<EventLoop>& loop, std::shared_ptr<Socket>& sock):m_eventLoop(loop),
        m_sock(sock), m_buffer(std::make_shared<Buffer>()),m_channel(nullptr)
{
    m_channel = new Channel(loop, m_sock->GetFd());
    m_channel->SetEnableRead_ET();
    std::function<void()> cb = std::bind(&Connection::HandleReadEvent, this, m_sock->GetFd());
    m_channel->SetReadCallbck(cb);
    m_channel->SetUseThreadPool();
}

Connection::~Connection()
{
    delete m_channel;
}

void Connection::SetDeleteConnCallback(std::function<void(int)> cb)
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
            HandleWriteEvent(sockfd);
            m_buffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            m_deleteConnCallback(sockfd);
            break;
        }
        else {
            printf("Connection reset by peer\n");
            m_deleteConnCallback(sockfd);          //会有bug，注释后单线程无bug
            break;
        }
    }
}

void Connection::HandleWriteEvent(int sockfd)
{
    int  data_size = m_buffer->readableSize(); 
    char buf[data_size];
    strcpy(buf, m_buffer->to_string().c_str());
    int data_left = data_size; 
    while (data_left > 0) 
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }
}
}