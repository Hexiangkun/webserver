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
        Invalid = 0,
        Connected,
        Closed,
        Failed,
    };
    Connection(std::shared_ptr<EventLoop>, std::shared_ptr<Socket>);
    ~Connection();

    void HandleReadEvent();
    void HandleWriteEvent();

    void HandleSendEvent(std::string msg);
    
    void SetDeleteConnCallback(const std::function<void(int)>&);
    void SetOnRecvCallback(const std::function<void(Connection*)>&);

    CONNSTATE GetState() const;
    void Close();

    void SetWriteBufferContent(const char* str);
    std::shared_ptr<Buffer> GetReadBuffer() const;
    std::shared_ptr<Buffer> GetWriteBuffer() const;
    
    std::shared_ptr<Socket> GetSocket() const;

private:
    void ReadNonBlocking();
    void WriteNonBlocking();

    void ReadBlocking();
    void WriteBlocking();

    void Business();

private:
    std::shared_ptr<EventLoop> m_eventLoop;
    std::shared_ptr<Socket> m_clnt_sock;
    std::shared_ptr<Buffer> m_read_buffer;
    std::shared_ptr<Buffer> m_write_buffer;
    CONNSTATE m_state;
    Channel* m_channel;
    
    std::function<void(int)> m_deleteConnCallback;
    std::function<void(Connection*)> m_onRecvCallback;

};


}

#endif