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
    enum CONNSTATE
    {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };
    Connection(std::shared_ptr<EventLoop>&, std::shared_ptr<Socket>&);
    ~Connection();

    void HandleReadEvent(int fd);
    void HandleWriteEvent(int fd);
    
    void SetDeleteConnCallback(std::function<void(int)>);
    void SetOnConnectCallback(std::function<void(std::shared_ptr<Connection>)>);

    CONNSTATE GetState();
    void Close();

    void SetSendBuffer(const char* str);
    std::shared_ptr<Buffer> GetReadBuffer();
    std::shared_ptr<Buffer> GetWriteBuffer();
    
    std::shared_ptr<Socket> GetSocket();

    void OnConnect(std::function<void()>);

private:
    void ReadNonBlocking();
    void WriteNonBlocking();

    void ReadBlocking();
    void WriteBlocking();

private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_sock;
    std::shared_ptr<Buffer> m_read_buffer;
    std::shared_ptr<Buffer> m_write_buffer;
    std::shared_ptr<Buffer> m_buffer;
    CONNSTATE m_state;
    Channel* m_channel;
    
    std::function<void(int)> m_deleteConnCallback;
    std::function<void(std::shared_ptr<Connection>)> m_connCallback;


};


}

#endif