#include "Connection.h"
#include "Socket.h"
#include "Channel.h"

#include "Common.h"
namespace hxk
{


Connection::Connection(std::shared_ptr<EventLoop> loop, int sockfd)
        :m_eventLoop(loop), m_clnt_sock(std::make_shared<Socket>(sockfd)),
        m_state(STATE::Connected),
        m_read_buffer(std::make_shared<Buffer>()),
        m_write_buffer(std::make_shared<Buffer>())
{
    if(m_eventLoop != nullptr) {
        m_channel = std::make_shared<Channel>(m_eventLoop, sockfd);
        m_channel->SetEnableRead_ET();
    }
}

Connection::~Connection()
{

}

void Connection::HandleReadEvent()
{

    ERRIF(m_state != STATE::Connected, "connection state is disconnected!");
    m_read_buffer->clear();
    
    if(m_clnt_sock->IsNonBlocking()) {
        ReadNonBlocking();
    }
    else {
        ReadBlocking();
    }
}

void Connection::HandleWriteEvent()
{
    ERRIF(m_state != STATE::Connected, "connection state is disconnected!");
    
    if(m_clnt_sock->IsNonBlocking()) {
        ReadNonBlocking();
    }
    else {
        ReadBlocking();
    }
    m_write_buffer->clear();
}

void Connection::HandleSendEvent(std::string msg)
{
    SetWriteBuffer(msg.c_str());
    HandleWriteEvent();
}

void Connection::SetDeleteConnCallback(const std::function<void(int)>& cb)
{
    m_deleteConnCallback = std::move(cb);
}

void Connection::SetOnRecvCallback(const std::function<void(Connection*)>& cb)
{
    m_recvCallback = std::move(cb);

    std::function<void()> bus = std::bind(&Connection::Business, this);
    m_channel->SetReadCallbck(bus);
}

Connection::STATE Connection::GetState() const
{
    return m_state;
}

void Connection::Close()
{
    m_deleteConnCallback(m_clnt_sock->GetFd());
}

void Connection::SetWriteBuffer(const char* str)
{
    m_write_buffer->clear();
    m_write_buffer->pushData(str, strlen(str));
}

std::shared_ptr<Buffer>& Connection::GetReadBuffer() 
{
    return m_read_buffer;
}

std::shared_ptr<Buffer>& Connection::GetWriteBuffer() 
{
    return m_write_buffer;
}

std::shared_ptr<Socket>& Connection::GetSocket()
{
    return m_clnt_sock;
}

void Connection::OnConnect(std::function<void()> func)
{

}

void Connection::OnMessage(std::function<void()> func)
{

}

void Connection::Business()
{
    HandleReadEvent();
    m_recvCallback(this);
}
void Connection::ReadNonBlocking()
{
    int sockfd = m_clnt_sock->GetFd();

    char buf[1024];
    while(true) {
        bzero(buf, sizeof(buf));
        int read_bytes = ::read(sockfd, buf, sizeof(buf));

        if(read_bytes > 0) {
            m_read_buffer->pushData(buf, read_bytes);
        }
        else if(read_bytes == -1 && errno == EINTR) {   //程序正常中断
            continue;
        }
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {    //非阻塞io，这个条件表示读取完毕
            break;
        }
        else if(read_bytes == 0) {  //EOF,客户端断开连接
            printf("read EOF, client fd %d disconnected\n", sockfd);
            m_state = STATE::Closed;
            Close();
            break;
        }
        else {
            printf("Other error on client fd %d\n", sockfd);
            m_state = STATE::Closed;
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

    while(data_left > 0) {
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if(bytes_write == -1 && errno == EINTR) {
            continue;
        }
        else if(bytes_write == -1 && errno == EAGAIN) {
            break;
        }
        else if(bytes_write == -1) {
            printf("Other error on client fd %d\n", sockfd);
            m_state = STATE::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}

// only use for client
void Connection::ReadBlocking()
{
    int sockfd = m_clnt_sock->GetFd();

    size_t data_size = m_clnt_sock->RecvTcpBufSize();

    char buf[1024];
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));

    if(bytes_read > 0) {
        m_read_buffer->pushData(buf, bytes_read);
    }
    else if(bytes_read == 0) {
        printf("read EOF, blocking client fd %d disconnected\n", sockfd);
        m_state = STATE::Closed;
    }
    else if(bytes_read == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = STATE::Closed;        
    }
}

void Connection::WriteBlocking()
{
    int sockfd = m_clnt_sock->GetFd();
    ssize_t bytes_write = write(sockfd, m_write_buffer->readAddr(), m_write_buffer->readableSize());

    if(bytes_write == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = STATE::Closed;
    }
}



}