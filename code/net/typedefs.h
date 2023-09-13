#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <functional>

namespace hxk
{

struct InetAddress;

class Connection;
class DatagramSocket;
class EventLoop;

using NewTcpConnCallback = std::function<void(Connection*)>;
using TcpConnFailCallback = std::function<void(EventLoop*, const InetAddress& peer)>;
using TcpMessageCallback = std::function<size_t(Connection*, const char* data, size_t len)>;
using TcpWriteCompleteCallback = std::function<void(Connection*)>;
using BindCallback = std::function<void(bool succ, const InetAddress&)>;

using UDPMessageCallback = std::function<void(DatagramSocket*, const char* data, size_t len)>;
using UDPCreateCallback = std::function<void(DatagramSocket*)>;


} // namespace hxk



#endif