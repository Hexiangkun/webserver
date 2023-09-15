#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <functional>
#include "code/util/Buffer.h"

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
    void HandleWriteEvent(int fd);
    
    void SetDeleteConnCallback(std::function<void(int)>);

private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_sock;
    std::shared_ptr<Buffer> m_buffer;
    Channel* m_channel;
    
    std::function<void(int)> m_deleteConnCallback;
};


}

#endif