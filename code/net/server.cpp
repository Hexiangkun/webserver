#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "code/util/util.h"
#include "Epoller.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024
using namespace hxk;

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
void handleReadEvent(int);

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->Bind(serv_addr);
    serv_sock->Listen();    
    Epoller::_ptr ep = std::make_shared<Epoller>();
    // Epoller *ep = new Epoller();
    serv_sock->SetNonBlocking();
    Channel *servChannel = new Channel(ep, serv_sock->GetFd());
    servChannel->SetEnableReading();
    while(true){
        std::vector<Channel*> activeChannels = ep->poll(1024);
        int nfds = activeChannels.size();
        for(int i = 0; i < nfds; ++i){
            int chfd = activeChannels[i]->GetFd();
            if(chfd == serv_sock->GetFd()){        //新客户端连接
                InetAddress *clnt_addr = new InetAddress();      //会发生内存泄露！没有delete
                Socket *clnt_sock = new Socket(serv_sock->Accept(clnt_addr));       //会发生内存泄露！没有delete
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->GetAddr().sin_addr), ntohs(clnt_addr->GetAddr().sin_port));
                clnt_sock->SetNonBlocking();
                Channel *clntChannel = new Channel(ep, clnt_sock->GetFd());
                clntChannel->SetEnableReading();
            } else if(activeChannels[i]->GetREvents() & EPOLLIN){      //可读事件
                handleReadEvent(activeChannels[i]->GetFd());
            } else{         //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}
