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
    m_sock->SetNonBlocking();

    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    m_acceptChannel->SetCallbck(cb);
    m_acceptChannel->SetEnableReading();
}

Acceptor::~Acceptor()
{

}

void Acceptor::SetNewConnectionCallback(std::function<void(std::shared_ptr<Socket>&)> cb)
{
    m_newConnectionCallback = cb;
}

void Acceptor::AcceptConnection()
{
    m_newConnectionCallback(m_sock);
}


}