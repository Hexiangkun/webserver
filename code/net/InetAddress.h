#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include "SocketUtil.h"

namespace hxk
{

class InetAddress
{
private:
    sockaddr_in addr_;

private:
    std::string ConvertIp(const char* ip);

public:

    InetAddress();

    InetAddress(const sockaddr_in& addr);
    void Init(const sockaddr_in& addr) ;

    InetAddress(uint32_t net_ip, uint16_t net_port);
    void Init(uint32_t net_ip, uint16_t net_port);

    InetAddress(const char* ip, uint16_t host_port);
    void Init(const char* ip, uint16_t net_port);

    InetAddress(const std::string& ip, uint16_t net_port);
    InetAddress(const std::string& ip_port);

    void Init(const std::string& ip_port);

    const sockaddr_in& GetAddr() const;

    std::string GetIp() const;

    uint16_t GetPort() const;
    
    void Clear();

    std::string ToString() const;

    bool IsValid() const;


    inline friend bool operator==(const InetAddress& a, const InetAddress& b);

    inline friend bool operator!=(const InetAddress& a, const InetAddress& b);
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