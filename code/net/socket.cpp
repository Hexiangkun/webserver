#include "socket.h"

namespace hxk
{

const int KInvalid = -1;
const int KTimeout = 0;
const int KError = -1;
const int KFof = -2;


std::string ConvertIp(const char* ip)
{
    if(strncmp(ip, "loopback", 8) == 0) {
        return "127.0.0.1";
    }

    if(strncmp(ip, "localhost", 9) == 0) {
        hxk::SocketAddr tmp;
        tmp.Init(hxk::GetLocalAddrInfo(), 0);
        return tmp.GetIp();
    }

    return std::string(ip);
}

in_addr_t GetLocalAddrInfo()
{
    char buf[512];
    struct ifconf conf;
    conf.ifc_len = sizeof(buf);
    conf.ifc_buf = buf;

    int sockfd = CreateUDPSocket();
    ioctl(sockfd, SIOCGIFCONF, &conf); //获取所有接口信息

    int Addr_num = conf.ifc_len / sizeof(struct ifreq);
    struct ifreq* ifr = conf.ifc_req;

    in_addr_t result = 0;
    for(int i = 0; i < Addr_num; i++) {
        if(ifr == nullptr) {
            break;
        }

        ioctl(sockfd, SIOCGIFFLAGS, ifr);

        if(((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP)) {
            struct sockaddr_in *pAddr = (struct sockaddr_in*)(&ifr->ifr_addr);
            result = pAddr->sin_addr.s_addr;
            break;
        }
        ++ifr;
    }
    CloseSocket(sockfd);
    return result;
}

int CreateTCPSocket()
{
    ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

int CreateUDPSocket()
{
    ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

bool CreateSocketPair(int& readSock, int& writeSock)
{
    int s[2];
    int ret = socketpair(AF_LOCAL, SOCK_STREAM, IPPROTO_TCP, s);
    if(ret != 0) {
        return false;
    }

    readSock = s[0];
    writeSock = s[1];

    return true;
}

void CloseSocket(int& sockfd)
{
    if(sockfd != KInvalid) {
        ::close(sockfd);
        sockfd = KInvalid;
    }
}

void SetNonBlock(int sockfd, bool nonBlock)
{
    int flag = ::fcntl(sockfd, F_GETFL, 0);
    assert(flag >= 0);

    if(nonBlock) {
        flag = fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);
    }
    else {
        flag = fcntl(sockfd, F_SETFL, flag & ~O_NONBLOCK);
    }
}

void SetNonDelay(int sockfd, bool nonDelay)
{
    int enable = nonDelay ? 1 : 0;
    ::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, (socklen_t)sizeof(enable));
}

void SetSendBuf(int sockfd, socklen_t size)
{
    ::setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size));
}

void SetRecvBuf(int sockfd, socklen_t size)
{
    ::setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size));
}

void SetReuseAddr(int sockfd)
{
    int reuse = 1;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
}

bool GetLocalAddr(int sock, SocketAddr& addr)
{
    sockaddr_in localAddr;
    socklen_t len = sizeof(localAddr);

    if(0 == getsockname(sock, (struct sockaddr*)&localAddr, &len)) {
        addr.Init(localAddr);
    }
    else {
        return false;
    }
    return true;
}

bool GetPeerAddr(int sock, SocketAddr& addr)
{
    sockaddr_in remoteAddr;
    socklen_t len = sizeof(remoteAddr);

    if(0 == getpeername(sock, (struct sockaddr*)&remoteAddr, &len)) {
        addr.Init(remoteAddr);
    }
    else {
        return false;
    }
    return true;
}


rlim_t GetMaxOpenFd()
{
    struct rlimit rlp;
    if(getrlimit(RLIMIT_NOFILE, &rlp) == 0) {
        return rlp.rlim_cur;
    }
    return 0;
}

bool SetMaxOpenFd(rlim_t maxfd)
{
    struct rlimit rlp;
    if(getrlimit(RLIMIT_NOFILE, &rlp) != 0) {
        return false;
    }

    if(maxfd <= rlp.rlim_cur) {
        return true;
    }
    if(maxfd > rlp.rlim_max) {
        return false;
    }

    rlp.rlim_cur = maxfd;
    return setrlimit(RLIMIT_NOFILE, &rlp) == 0;
}
}