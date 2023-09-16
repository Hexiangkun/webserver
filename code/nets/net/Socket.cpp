#include "Socket.h"
#include "Common.h"
#include "InetAddress.h"

namespace hxk
{

Socket::Socket() : fd_(-1)
{

}

Socket::Socket(int fd):fd_(fd) 
{
    ERRIF(fd_ == -1, "Copy create socket error!");
}

Socket::~Socket()
{
    if(fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

int Socket::GetFd() const
{
    return fd_;
}

void Socket::SetNonBlocking()
{
    ERRIF(::fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK) == -1, "Socket set non-blocking error!");
}

bool Socket::IsNonBlocking() const
{
    return (fcntl(fd_, F_GETFL) & O_NONBLOCK) != 0;
}

void Socket::Create()
{
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    ERRIF(fd_ == -1, "Create socket error!");
}

void Socket::Bind(std::shared_ptr<InetAddress>& addr) const
{
    ERRIF(::bind(fd_, (struct sockaddr*)(&addr->GetAddr()), sizeof(addr->GetAddr())) == -1, "Socket bind error!");
}

void Socket::Bind(const char* ip, uint16_t port) const
{
    std::shared_ptr<InetAddress> addr = std::make_shared<InetAddress>(ip, port);
    Bind(addr);
}

void Socket::Listen() const
{
    ERRIF(::listen(fd_, SOMAXCONN) == -1, "Socket listen error!");
}


int Socket::Accept(std::shared_ptr<InetAddress>& addr) const
{
    int clnt_sockfd = -1;
    struct sockaddr_in tmp;
    socklen_t len = sizeof(tmp);
    if(IsNonBlocking()) {
        while (true)
        {
            clnt_sockfd = ::accept(fd_, (struct sockaddr*)(&tmp), &len);
            if(clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                continue;
            }
            else if (clnt_sockfd == -1)
            {
                ERRIF(true, "socket nonBlocking accept error");
            }
            else {
                break;
            }
        } 
    }
    else {
        clnt_sockfd = ::accept(fd_, (struct sockaddr*)(&tmp), &len);
        ERRIF(clnt_sockfd == -1, "socket accept error");
    }
    addr->Init(tmp);
    return clnt_sockfd;
}

void Socket::Connect(std::shared_ptr<InetAddress>& addr) const
{
    struct sockaddr_in tmp_addr = addr->GetAddr();
    if(IsNonBlocking()) {
        while (true)
        {
            int ret = ::connect(fd_, (struct sockaddr*)&tmp_addr, sizeof(tmp_addr));
            if(ret == 0) {
                break;
            }
            else if(ret == -1 && (errno == EINPROGRESS)) {
                continue;
            }
            else if(ret == -1) {
                ERRIF(true, "socket nonBlocking connect error");
            }   
        }
    }
    else {
        ERRIF(::connect(fd_, (struct sockaddr*)&tmp_addr, sizeof(tmp_addr)) == -1, "socket connect error");
    }
}

   
void Socket::Connect(const char* ip, uint16_t port) const
{
    std::shared_ptr<InetAddress> addr = std::make_shared<InetAddress>(ip, port);
    Connect(addr);
}


size_t Socket::RecvTcpBufSize() const
{
    size_t size = -1;
    if(::ioctl(fd_, FIONREAD, &size) == -1) {
        perror("Socket get recv buf size failed!");
    }
    return size;
}
    
}