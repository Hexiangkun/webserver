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

    void SetNewConnectionCallback(std::function<void(std::shared_ptr<Socket>&)>);



    void AcceptConnection();
private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_sock;
    std::shared_ptr<InetAddress> m_addr;
    std::shared_ptr<Channel> m_acceptChannel;

    std::function<void(std::shared_ptr<Socket>&)> m_newConnectionCallback;
};


}


#endif