#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <memory>
#include <functional>

namespace hxk
{
class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor
{

public:
    Acceptor(std::shared_ptr<EventLoop>& );
    ~Acceptor();

    void SetNewConnectionCallback(const std::function<void(int)>&);

    void AcceptConnection() const;
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_serv_sock;
    std::shared_ptr<InetAddress> m_serv_addr;
    std::shared_ptr<Channel> m_acceptChannel;

    std::function<void(int)> m_newConnectionCallback;
};


}


#endif