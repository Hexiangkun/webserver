#include "Socket.h"

namespace hxk
{

Socket::Socket(/* args */) : fd_(-1)
{
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    Errif(fd_ == -1, "Create socket error!");
}

Socket::Socket(int fd):fd_(fd) 
{
    Errif(fd_ == -1, "Create socket error!");
}

Socket::~Socket()
{
    if(fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

void Socket::Bind(std::shared_ptr<InetAddress>& addr)
{
    Errif(::bind(fd_, (struct sockaddr*)(&addr->GetAddr()), sizeof(addr->GetAddr())) == -1, "socket bind error!");
}

void Socket::Bind(const char* ip, uint16_t port)
{
    std::shared_ptr<InetAddress> addr = std::make_shared<InetAddress>(ip, port);
    Bind(addr);
}

void Socket::Listen()
{
    Errif(::listen(fd_, SOMAXCONN) == -1, "Listen error!");
}

void Socket::SetNonBlocking()
{
    fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

bool Socket::IsNonBlocking()
{
    return (fcntl(fd_, F_GETFL) & O_NONBLOCK) != 0;
}

int Socket::Accept(std::shared_ptr<InetAddress>& addr)
{
    struct sockaddr_in tmp;
    socklen_t len = sizeof(tmp);
    int clnt_sockfd = ::accept(fd_, (struct sockaddr*)(&tmp), &len);

    Errif(clnt_sockfd == -1, "Socket accept error!");

    addr->Init(tmp);

    return clnt_sockfd;
}

void Socket::Connect(std::shared_ptr<InetAddress>& addr)
{
    Errif(::connect(fd_, (struct sockaddr*)&addr->GetAddr(), sizeof(addr->GetAddr())) == -1, "Socket connect error!");
}

void Socket::Connect(const char* ip, uint16_t port)
{
    std::shared_ptr<InetAddress> addr = std::make_shared<InetAddress>(ip, port);
    Connect(addr);    
}

int Socket::GetFd() 
{
    return fd_;
}

}