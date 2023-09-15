#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"

namespace hxk
{

//初始化列表按照变量定义顺序进行初始化
Acceptor::Acceptor(std::shared_ptr<EventLoop>& loop): m_eventLoop(loop),
            m_sock(std::make_shared<Socket>()),
            m_addr(std::make_shared<InetAddress>("127.0.0.1", 8888)),
            m_acceptChannel(std::make_shared<Channel>(m_eventLoop, m_sock->GetFd()))
{
    m_sock->Bind(m_addr);
    m_sock->Listen();
    // m_sock->SetNonBlocking();

    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    m_acceptChannel->SetReadCallbck(cb);
    m_acceptChannel->SetEnableReading();    //默认使用LT模式
    // m_acceptChannel->SetUseET(true);    //默认为true，可用使用false
    // m_acceptChannel->SetEnableRead_ET();    //直接设置enable read and et mode
    m_acceptChannel->SetUseThreadPool(false);    //默认为true，使用线程池
}

Acceptor::~Acceptor()
{

}


void Acceptor::AcceptConnection()
{
    std::shared_ptr<InetAddress> clnt_addr = std::make_shared<InetAddress>();
    std::shared_ptr<Socket> clnt_sock = std::make_shared<Socket>(m_sock->Accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp().c_str(), clnt_addr->GetPort());
    clnt_sock->SetNonBlocking();
    m_newConnectionCallback(clnt_sock);
}

void Acceptor::SetNewConnectionCallback(std::function<void(std::shared_ptr<Socket>&)> cb)
{
    m_newConnectionCallback = cb;
}


}