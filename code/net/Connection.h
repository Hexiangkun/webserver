#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <functional>

namespace hxk
{
class EventLoop;
class Socket;
class Channel;

class Connection
{
public:
    Connection(std::shared_ptr<EventLoop>&, std::shared_ptr<Socket>&);
    ~Connection();

    void HandleReadEvent(int fd);

    void SetDeleteConnCallback(std::function<void(std::shared_ptr<Socket>&)>);

private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_sock;
    Channel* m_channel;

    std::function<void(std::shared_ptr<Socket>&)> m_deleteConnCallback;
};


}

#endif