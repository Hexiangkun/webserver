#ifndef HXK_SOCKET_H
#define HXK_SOCKET_H

#include <arpa/inet.h>
#include <sys/resource.h>
#include <cassert>
#include <stdexcept>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <string>
#include <memory>
#include <functional>
#include <iostream>

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
        memcpy(&addr_, &addr, sizeof(addr));
    }

    SocketAddr(uint32_t net_ip, uint16_t net_port)
    {
        Init(net_ip, net_port);
    }

    void Init(uint32_t net_ip, uint16_t net_port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(net_port);
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
        std::string::size_type pos = ip_port.find_first_of(':');
        if(pos == std::string::npos) {
            throw std::runtime_error("ip port error!");
        }

        std::string ip = ip_port.substr(0, pos);
        std::string port = ip_port.substr(pos+1);

        Init(ip.c_str(), static_cast<uint16_t>(std::stoi(port)));
    }

    const sockaddr_in& GetAddr() const
    {
        return addr_;
    }

    std::string GetIp() const
    {
        char tmp[32];
        const char* res = inet_ntop(AF_INET, &addr_.sin_addr.s_addr, tmp, (socklen_t)sizeof(tmp));
        return std::string(res);
    }

    uint16_t GetPort() const
    {
        return ntohs(addr_.sin_port);
    }
    

    void Clear()
    {
        memset(&addr_, 0 , sizeof(addr_));
    }

    std::string ToString() const
    {
        return GetIp() + ":" + std::to_string(ntohs(addr_.sin_port));
    }

    bool IsValid() const
    {
        return addr_.sin_family != 0;
    }

    inline friend bool operator==(const SocketAddr& a, const SocketAddr& b)
    {
        return a.addr_.sin_family == b.addr_.sin_family &&
                a.addr_.sin_port == b.addr_.sin_port &&
                a.addr_.sin_addr.s_addr == b.addr_.sin_addr.s_addr;
    }

    inline friend bool operator!=(const SocketAddr& a, const SocketAddr& b)
    {
        return !(a==b);
    }
};


extern const int KInvalid;
extern const int KTimeout;
extern const int KError;
extern const int KFof;

int CreateTCPSocket();
int CreateUDPSocket();

bool CreateSocketPair(int& readSock, int& writeSock);

void CloseSocket(int& sockfd);

void SetNonBlock(int sockfd, bool nonBlock = true);
void SetNonDelay(int sockfd, bool nonDelay = true);

void SetSendBuf(int sockfd, socklen_t size = 64 * 1024);
void SetRecvBuf(int sockfd, socklen_t size = 64 * 1024);

void SetReuseAddr(int sockfd);

bool GetLocalAddr(int sock, SocketAddr&);
bool GetPeerAddr(int sock, SocketAddr&);

in_addr_t GetLocalAddrInfo();

rlim_t GetMaxOpenFd();

bool SetMaxOpenFd(rlim_t maxfd);

}

namespace std
{
    template<>
    struct hash<hxk::SocketAddr>
    {
        typedef hxk::SocketAddr argument_type;
        typedef std::size_t result_type ;
        result_type operator()(const argument_type& s) const noexcept
        {
            result_type h1 = std::hash<short>{}(s.GetAddr().sin_family);
            result_type h2 = std::hash<unsigned int>{}(s.GetAddr().sin_port);
            result_type h3 = std::hash<unsigned int>{}(s.GetAddr().sin_addr.s_addr);

            return h3 ^ ((h1 ^ (h2 << 1)) << 1);
        }
    };
}

#endif