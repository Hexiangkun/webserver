#ifndef SOCKET_H
#define SOCKET_H

#include "InetAddress.h"
#include "Common.h"
#include <memory>
#include <unistd.h>
#include <fcntl.h>
namespace hxk
{

class Socket
{
private:
    int fd_;
public:
    Socket(/* args */);
    Socket(int fd);
    ~Socket();

    void Bind(std::shared_ptr<InetAddress>& addr);
    void Bind(const char* ip, uint16_t port);
    void Listen();

    void SetNonBlocking();
    bool IsNonBlocking();

    int Accept(std::shared_ptr<InetAddress>& addr);

    void Connect(const char* ip, uint16_t port);
    void Connect(std::shared_ptr<InetAddress>&);

    int GetFd();
};

}


#endif