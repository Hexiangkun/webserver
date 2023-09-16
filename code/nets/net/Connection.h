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
    enum STATE
    {
        Invalid = 0,
        Connected,
        Closed,
        Failed,
    };
    Connection(std::shared_ptr<EventLoop>, int fd);
    ~Connection();

    void HandleReadEvent();
    void HandleWriteEvent();
    void HandleSendEvent(std::string msg);

    void SetDeleteConnCallback(const std::function<void(int)>&);
    void SetOnRecvCallback(const std::function<void(Connection*)>&);
    
    STATE GetState() const;
    void Close();

    void SetWriteBuffer(const char* str);
    std::shared_ptr<Buffer>& GetReadBuffer();
    std::shared_ptr<Buffer>& GetWriteBuffer();
    
    std::shared_ptr<Socket>& GetSocket();

    void OnConnect(std::function<void()>);
    void OnMessage(std::function<void()>);
private:
    void Business();
    void ReadNonBlocking();
    void WriteNonBlocking();

    void ReadBlocking();
    void WriteBlocking();

private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_clnt_sock;
    std::shared_ptr<Buffer> m_read_buffer;
    std::shared_ptr<Buffer> m_write_buffer;
    std::shared_ptr<Channel> m_channel;
    
    STATE m_state;
    
    std::function<void(int)> m_deleteConnCallback;
    std::function<void(Connection*)> m_recvCallback;


};


}

#endif