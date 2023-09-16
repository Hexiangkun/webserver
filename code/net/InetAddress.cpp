#include "InetAddress.h"


namespace hxk
{


InetAddress::InetAddress()
{
    Clear();
}

InetAddress::InetAddress(const sockaddr_in& addr)
{
    Init(addr);
}

void InetAddress::Init(const sockaddr_in& addr) 
{
    memcpy(&addr_, &addr, sizeof(addr));
}

InetAddress::InetAddress(uint32_t net_ip, uint16_t net_port)
{
    Init(net_ip, net_port);
}

void InetAddress::Init(uint32_t net_ip, uint16_t net_port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(net_port);
    addr_.sin_addr.s_addr = net_ip;
}

InetAddress::InetAddress(const char* ip, uint16_t host_port)
{
    Init(ip, host_port);
}

void InetAddress::Init(const char* ip, uint16_t net_port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(net_port);
    std::string sip = ConvertIp(ip);
    addr_.sin_addr.s_addr = ::inet_addr(sip.data());
}

InetAddress::InetAddress(const std::string& ip, uint16_t net_port)
{
    Init(ip.data(), net_port);
}

InetAddress::InetAddress(const std::string& ip_port)
{
    Init(ip_port);
}

void InetAddress::Init(const std::string& ip_port)
{
    std::string::size_type pos = ip_port.find_first_of(':');
    if(pos == std::string::npos) {
        throw std::runtime_error("ip port error!");
    }

    std::string ip = ip_port.substr(0, pos);
    std::string port = ip_port.substr(pos+1);

    Init(ip.c_str(), static_cast<uint16_t>(std::stoi(port)));
}

const sockaddr_in& InetAddress::GetAddr() const
{
    return addr_;
}

std::string InetAddress::GetIp() const
{
    char tmp[32];
    const char* res = inet_ntop(AF_INET, &addr_.sin_addr.s_addr, tmp, (socklen_t)sizeof(tmp));
    return std::string(res);
}

uint16_t InetAddress::GetPort() const
{
    return ntohs(addr_.sin_port);
}

void InetAddress::Clear()
{
    memset(&addr_, 0 , sizeof(addr_));
}

std::string InetAddress::ToString() const
{
    return GetIp() + ":" + std::to_string(GetPort());
}

bool InetAddress::IsValid() const
{
    return addr_.sin_family != 0;
}

std::string InetAddress::ConvertIp(const char* ip)
{
    if(strncmp(ip, "loopback", 8) == 0) {
        return "127.0.0.1";
    }

    if(strncmp(ip, "localhost", 9) == 0) {
        return "127.0.0.1";
    }

    return std::string(ip);
}


bool operator==(const InetAddress& a, const InetAddress& b)
{
    return a.addr_.sin_family == b.addr_.sin_family &&
            a.addr_.sin_port == b.addr_.sin_port &&
            a.addr_.sin_addr.s_addr == b.addr_.sin_addr.s_addr;
}

bool operator!=(const InetAddress& a, const InetAddress& b)
{
    return !(a==b);
}
}