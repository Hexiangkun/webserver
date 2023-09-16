#ifndef HXK_SOCKETUTIL_H
#define HXK_SOCKETUTIL_H

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

namespace SocketUtil
{

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

sockaddr_in GetLocalAddr(int sock);
sockaddr_in GetPeerAddr(int sock);

std::string ConvertIp(const char* ip);
in_addr_t GetLocalAddrInfo();

rlim_t GetMaxOpenFd();

bool SetMaxOpenFd(rlim_t maxfd);

}

}



#endif