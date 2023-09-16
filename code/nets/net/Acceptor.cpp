#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"

namespace hxk
{

//初始化列表按照变量定义顺序进行初始化
Acceptor::Acceptor(std::shared_ptr<EventLoop>& loop): m_eventLoop(loop),
            m_serv_sock(std::make_shared<Socket>()),
            m_serv_addr(std::make_shared<InetAddress>("127.0.0.1", 8888))         
{
    m_serv_sock->Create();
    m_serv_sock->Bind(m_serv_addr);
    m_serv_sock->Listen();
    // m_sock->SetNonBlocking();

    m_acceptChannel = std::make_shared<Channel>(m_eventLoop, m_serv_sock->GetFd());
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    m_acceptChannel->SetReadCallbck(cb);
    m_acceptChannel->SetEnableRead();    //默认使用LT模式
    // m_acceptChannel->SetUseET(true);    //默认为true，可用使用false
    // m_acceptChannel->SetEnableRead_ET();    //直接设置enable read and et mode
}

Acceptor::~Acceptor()
{

}


void Acceptor::AcceptConnection() const
{
    std::shared_ptr<InetAddress> clnt_addr = std::make_shared<InetAddress>();
    std::shared_ptr<Socket> clnt_sock = std::make_shared<Socket>(m_serv_sock->Accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp().c_str(), clnt_addr->GetPort());
    clnt_sock->SetNonBlocking();

    if(m_newConnectionCallback){
        m_newConnectionCallback(clnt_sock->GetFd());
    }
}

void Acceptor::SetNewConnectionCallback(const std::function<void(int)>& cb)
{
    m_newConnectionCallback = std::move(cb);
}


}