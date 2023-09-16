#include "Connection.h"
#include "Socket.h"
#include "Channel.h"

namespace hxk
{


Connection::Connection(std::shared_ptr<EventLoop> loop, std::shared_ptr<Socket> sock)
        :m_eventLoop(loop), m_clnt_sock(sock),
        m_read_buffer(std::make_shared<Buffer>()),
        m_write_buffer(std::make_shared<Buffer>()),
        m_channel(nullptr)
{
    if(m_eventLoop != nullptr) {
        m_channel = new Channel(loop, m_clnt_sock->GetFd());
        m_channel->SetEnableRead_ET();
    }

    m_state = CONNSTATE::Connected;

    std::function<void()> cb = std::bind(&Connection::HandleReadEvent, this, m_clnt_sock->GetFd());
    m_channel->SetReadCallbck(cb);
}

Connection::~Connection()
{
    delete m_channel;
}

void Connection::HandleReadEvent(int sockfd)
{
    if(m_state != CONNSTATE::Connected) {
        perror("Connection is not connected, can not read");
    }
    assert(m_state == CONNSTATE::Connected && "Connection state is disconnected!");
    m_read_buffer->clear();

    if(m_clnt_sock->IsNonBlocking()) {
        return ReadNonBlocking();
    }
    else {
        return ReadBlocking();
    }
    
    // char buf[1024];
    // while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    //     bzero(buf, sizeof(buf));
    //     ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    //     if(bytes_read > 0){
    //         m_buffer->pushData(buf, bytes_read);

    //     } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
    //         printf("continue reading");
    //         continue;
    //     } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
    //         printf("finish reading once\n");
    //         printf("message from client fd %d: %s\n", sockfd, m_buffer->to_string().c_str());
    //         HandleWriteEvent(sockfd);
    //         m_buffer->clear();
    //         break;
    //     } else if(bytes_read == 0){  //EOF，客户端断开连接
    //         printf("EOF, client fd %d disconnected\n", sockfd);
    //         // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
    //         m_deleteConnCallback(sockfd);
    //         break;
    //     }
    //     else {
    //         printf("Connection reset by peer\n");
    //         m_deleteConnCallback(sockfd);          //会有bug，注释后单线程无bug
    //         break;
    //     }
    // }
}

void Connection::HandleWriteEvent(int sockfd)
{
    if(m_state != CONNSTATE::Connected) {
        perror("Connection is not connected, can not read");
    }
    if(m_clnt_sock->IsNonBlocking()) {
        WriteNonBlocking();
    }
    else {
        WriteBlocking();
    }
    m_write_buffer->clear();
    return;
    // int  data_size = m_buffer->readableSize(); 
    // char buf[data_size];
    // strcpy(buf, m_buffer->to_string().c_str());
    // int data_left = data_size; 
    // while (data_left > 0) 
    // { 
    //     ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
    //     if (bytes_write == -1 && errno == EAGAIN) { 
    //         break;
    //     }
    //     data_left -= bytes_write; 
    // }
}

void Connection::ReadNonBlocking()
{
    int sockfd = m_clnt_sock->GetFd();
    char buf[1024];
    while (true)
    {
        bzero(buf, sizeof(buf));

        ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0) {
            m_read_buffer->pushData(buf, bytes_read);
        }
        else if(bytes_read == -1 && errno == EINTR) {
            printf("continue reading\n");
            continue;
        }
        else if(bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, m_read_buffer->to_string().c_str());
    
            m_write_buffer->pushData(m_read_buffer->readAddr(), m_read_buffer->readableSize());
            WriteNonBlocking();
            break;
        }
        else if(bytes_read == 0) {
            printf("read EOF, client fd %d disconnected\n", sockfd);
            m_state = CONNSTATE::Closed;
            Close();    //关闭连接
            break;
        }
        else {
            printf("Other error on client fd %d\n", sockfd);
            m_state = CONNSTATE::Closed;
            Close();
            break;
        }
    }
}

void Connection::WriteNonBlocking()
{
    int sockfd = m_clnt_sock->GetFd();
    int data_size = m_write_buffer->readableSize();
    char buf[data_size];
    memcpy(buf, m_write_buffer->readAddr(), data_size);
    int data_left = data_size;
    while (data_left > 0)
    {
        ssize_t bytes_write = ::write(sockfd, buf + data_size - data_left, data_left);
        if(bytes_write == -1 && errno == EINTR) {
            printf("continue writing\n");
            continue;
        }
        else if(bytes_write == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        }
        else if(bytes_write == -1) {
            printf("Other error on client fd %d\n", sockfd);
            m_state = CONNSTATE::Closed;
            break;
            
        }
        data_left -= bytes_write;
    }
}

void Connection::ReadBlocking()
{
    int sockfd = m_clnt_sock->GetFd();
    char buf[1024];
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if(bytes_read > 0) {
        m_read_buffer->pushData(buf, bytes_read);
    }
    else if(bytes_read == 0) {
        printf("read EOF, client fd %d disconnected\n", sockfd);
        m_state = CONNSTATE::Closed;
    }
    else if(bytes_read == -1) {
        printf("Other error on client fd %d\n", sockfd);
        m_state = CONNSTATE::Closed;
    }
}

void Connection::WriteBlocking()
{
    int sockfd = m_clnt_sock->GetFd();
    ssize_t bytes_write = ::write(sockfd, m_write_buffer->readAddr(), m_write_buffer->readableSize());
    if(bytes_write == -1) {
        printf("Other error on client fd %d\n", sockfd);
        m_state = CONNSTATE::Closed;
    }
}

void Connection::Business()
{
    HandleReadEvent(m_clnt_sock->GetFd());
    if(m_onRecvCallback){
        m_onRecvCallback(this);
    }
}

Connection::CONNSTATE Connection::GetState() const
{
    return m_state;
}

void Connection::Close()
{
    m_deleteConnCallback(m_clnt_sock->GetFd());
}


void Connection::SetDeleteConnCallback(const std::function<void(int)>& cb)
{
    m_deleteConnCallback = std::move(cb);
}

void Connection::SetOnRecvCallback(const std::function<void(Connection*)>& cb)
{
    m_onRecvCallback = std::move(cb);
    std::function<void()> bus = std::bind(&Connection::Business, this);
    m_channel->SetReadCallbck(bus);
}

std::shared_ptr<Buffer> Connection::GetReadBuffer() const
{
    return m_read_buffer;
}

std::shared_ptr<Buffer> Connection::GetWriteBuffer() const
{
    return m_write_buffer;
}

std::shared_ptr<Socket> Connection::GetSocket() const
{
    return m_clnt_sock;
}

void Connection::SetWriteBufferContent(const char* str)
{
    m_write_buffer->pushData(str, strlen(str));
}

}