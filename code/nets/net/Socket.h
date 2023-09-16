#ifndef SOCKET_H
#define SOCKET_H

#include <memory>

namespace hxk
{
class InetAddress;

class Socket
{
private:
    int fd_;
public:
    Socket(/* args */);

    Socket(int fd);

    ~Socket();

    int GetFd() const;

    void SetNonBlocking();
    bool IsNonBlocking() const;

    void Create();

    void Bind(std::shared_ptr<InetAddress>& addr) const;
    void Bind(const char* ip, uint16_t port) const;

    void Listen() const;

    int Accept(std::shared_ptr<InetAddress>& addr) const;

    void Connect(std::shared_ptr<InetAddress>& addr) const;

    void Connect(const char* ip, uint16_t port) const;
    
    size_t RecvTcpBufSize() const;
};

}


#endif