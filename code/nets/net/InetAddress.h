#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include "SocketUtil.h"

namespace hxk
{

std::string ConvertIp(const char* ip);

class InetAddress
{
private:
    sockaddr_in addr_;

public:

    InetAddress()
    {
        Clear();
    }

    InetAddress(const sockaddr_in& addr)
    {
        Init(addr);
    }
    
    void Init(const sockaddr_in& addr) 
    {
        memcpy(&addr_, &addr, sizeof(addr));
    }

    InetAddress(uint32_t net_ip, uint16_t net_port)
    {
        Init(net_ip, net_port);
    }

    void Init(uint32_t net_ip, uint16_t net_port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(net_port);
        addr_.sin_addr.s_addr = net_ip;
    }
    InetAddress(const char* ip, uint16_t host_port)
    {
        Init(ip, host_port);
    }

    void Init(const char* ip, uint16_t net_port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(net_port);
        std::string sip = hxk::ConvertIp(ip);
        addr_.sin_addr.s_addr = ::inet_addr(sip.data());
    }

    InetAddress(const std::string& ip, uint16_t net_port)
    {
        Init(ip.data(), net_port);
    }

    InetAddress(const std::string& ip_port)
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
        return GetIp() + ":" + std::to_string(GetPort());
    }

    bool IsValid() const
    {
        return addr_.sin_family != 0;
    }

    inline friend bool operator==(const InetAddress& a, const InetAddress& b)
    {
        return a.addr_.sin_family == b.addr_.sin_family &&
                a.addr_.sin_port == b.addr_.sin_port &&
                a.addr_.sin_addr.s_addr == b.addr_.sin_addr.s_addr;
    }

    inline friend bool operator!=(const InetAddress& a, const InetAddress& b)
    {
        return !(a==b);
    }
};



} // namespace hxk

namespace std
{
    template<>
    struct hash<hxk::InetAddress>
    {
        typedef hxk::InetAddress argument_type;
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