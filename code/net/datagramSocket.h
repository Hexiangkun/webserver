#ifndef DATAGRAMSOCKET_H
#define DATAGRAMSOCKET_H

#include "poller.h"
#include "socket.h"
#include "typedefs.h"
#include <list>
namespace hxk
{

class EventLoop;

class DatagramSocket : public internal::Channel
{
public:
    explicit DatagramSocket(EventLoop* loop);
    ~DatagramSocket();

    DatagramSocket(const DatagramSocket& ) = delete;
    void operator=(const DatagramSocket&) = delete;


    void SetMaxPacketSize(std::size_t s);

    std::size_t GetMaxPacketSize() const { return m_maxPacketSize;}

    bool Bind(const InetAddress* addr);

    int Identifier() const override;
    bool HandleReadEvent() override;
    bool HandleWriteEvent() override;
    void HandleErrorEvent() override;

    bool SendPacket(const void*, size_t, const InetAddress* = nullptr);

    const InetAddress& PeerAddr() const { return m_srcAddr; }

private:
    //添加进链表中
    void _PutSendBuf(const void* data, size_t size, const InetAddress* dst);
    int _Send(const void* data, size_t size, const InetAddress& dst);
    struct Package
    {
        InetAddress dst;
        std::string data;
    };

private:
    EventLoop* const m_loop;
    int m_localSock;
    std::size_t m_maxPacketSize;
    InetAddress m_srcAddr;

    std::list<Package> m_sendList;
    UDPMessageCallback m_onMessage;
    UDPCreateCallback m_onCreate;

};

}

#endif