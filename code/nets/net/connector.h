#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "socket.h"
#include "typedefs.h"
#include "poller.h"
#include "code/util/timer.h"

namespace hxk
{

namespace internal
{

enum class ConnectState
{
    None,
    Connecting,
    Connected,
    Failed,
};

class Connector : public Channel
{
public:
    explicit Connector(EventLoop* loop);
    ~Connector();

    Connector(const Connector&) = delete;
    void operator=(const Connector&) = delete;

    void SetNewConnCallback(NewTcpConnCallback cb);

    void SetFailCallback(TcpConnFailCallback cb);

    bool Connect(const InetAddress&, DurationMs, EventLoop* = nullptr);

    int Identifier() const override;
    bool HandleReadEvent() override;
    bool HandleWriteEvent() override;
    void HandleErrorEvent() override;

private:
    void _OnSuccess();
    void _OnFailed();

    int m_localSock = KInvalid;
    InetAddress m_peer;
    EventLoop* const m_loop;
    EventLoop* m_dstLoop;

    ConnectState m_state = ConnectState::None;

    TimerPtrId m_timeoutId;

    TcpConnFailCallback m_onConnectFail;
    NewTcpConnCallback m_newConnCallback;
};

}

} // namespace hxk


#endif