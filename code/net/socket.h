#ifndef HXK_SOCKET_H
#define HXK_SOCKET_H

#include <arpa/inet.h>
#include <sys/resource.h>
#include <cassert>

#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <string>
#include <memory>
#include <functional>


namespace hxk
{
std::string ConvertIp(const char* ip);


struct SocketAddr
{
private:
    sockaddr_in addr_;

public:
    static const uint16_t KInvalidPort = -1;

    SocketAddr()
    {
        Clear();
    }

    SocketAddr(const sockaddr_in& addr)
    {
        Init(addr);
    }
    
    void Init(const sockaddr_in& addr) 
    {

    }

    SocketAddr(uint32_t net_ip, uint16_t net_port)
    {
        Init(net_ip, net_port);
    }

    void Init(uint32_t net_ip, uint16_t net_port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = net_port;
        addr_.sin_addr.s_addr = net_ip;
    }
    SocketAddr(const char* ip, uint16_t host_port)
    {
        Init(ip, host_port);
    }

    void Init(const char* ip, uint16_t host_port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(host_port);
        std::string sip = ConvertIp(ip);
        addr_.sin_addr.s_addr = ::inet_addr(sip.data());
    }

    SocketAddr(const std::string& ip, uint16_t host_port)
    {
        Init(ip.data(), host_port);
    }

    SocketAddr(const std::string& ip_port)
    {
        Init(ip_port);
    }

    void Init(const std::string& ip_port)
    {

    }




    

    void Clear()
    {

    }
};


}

#endif