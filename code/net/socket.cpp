#include "socket.h"

namespace hxk
{
std::string ConvertIp(const char* ip)
{
    if(strncmp(ip, "loopback", 8) == 0) {
        return "127.0.0.1";
    }

    if(strncmp(ip, "localhost", 9) == 0) {

    }
}

in_addr_t GetLocalAddrInfo()
{
    char buf[512];
    struct ifconf conf;
    conf.ifc_len = sizeof(buf);
    conf.ifc_buf = buf;


}
}