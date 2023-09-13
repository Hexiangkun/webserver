#ifndef SOCKET_H
#define SOCKET_H

#include "InetAddress.h"
#include "code/util/util.h"

namespace hxk
{

class Socket
{
private:
    int fd_;
public:
    Socket(/* args */) : fd_(-1)
    {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        errif(fd_ == -1, "Create socket error!");
    }

    Socket(int fd):fd_(fd) 
    {
        errif(fd_ == -1, "Create socket error!");
    }

    ~Socket()
    {
        if(fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    void Bind(InetAddress* addr)
    {
        errif(::bind(fd_, (struct sockaddr*)(&addr->GetAddr()), sizeof(addr->GetAddr())) == -1, "Socket bind error!");
    }

    void Listen()
    {
        errif(::listen(fd_, SOMAXCONN) == -1, "Listen error!");
    }

    void SetNonBlocking()
    {
        fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
    }


    int Accept(InetAddress* addr)
    {
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        int clnt_sockfd = ::accept(fd_, (struct sockaddr*)(&tmp), &len);

        errif(clnt_sockfd == -1, "Socket accept error!");

        addr->Init(tmp);

        return clnt_sockfd;
    }

    int GetFd() 
    {
        return fd_;
    }
};

}


#endif