#include "datagramSocket.h"
#include <iostream>
namespace hxk
{

DatagramSocket::DatagramSocket(EventLoop* loop) : m_loop(loop), m_localSock(KInvalid), m_maxPacketSize(2048)
{

}

DatagramSocket::~DatagramSocket()
{
    std::cout << "Close Udp socket" << Identifier() << std::endl;
    CloseSocket(m_localSock);
}

bool DatagramSocket::Bind(const InetAddress* addr)
{
    if(m_localSock != KInvalid) {
        std::cout << "Udp socket repeat create" << std::endl;
        return false;
    }

    m_localSock = CreateUDPSocket();

    if(m_localSock == KInvalid) {
        std::cout << "Failed create udp socket!" << std::endl;
        return false;
    }

    SetNonBlock(m_localSock);
    SetReuseAddr(m_localSock);

    const bool isServer = (addr && addr->IsValid());

    if(isServer) {  //server udp
        uint16_t port = addr->GetPort();
        int ret = ::bind(m_localSock, (struct sockaddr*)&addr->GetAddr(), sizeof(addr->GetAddr()));
        if(KError == ret) {
            CloseSocket(m_localSock);
            std::cout << "cannot bind udp port" << port << std::endl;
            return false;
        }
    }
    else {
        ;
    }

    //todo loop
    if(!m_loop->Register(internal::eET_Read, shared_from_this())) {
        std::cout << "add udp to loop failed, socket = " << m_localSock;
        return false;
    }
    if(m_onCreate) {
        m_onCreate(this);
    }

    std::cout << "Create new udp fd = " << m_localSock;
    return true;
}

int DatagramSocket::Identifier() const
{
    return m_localSock;
}

bool DatagramSocket::HandleReadEvent()
{
    std::unique_ptr<char[]> recvbuf(new char[m_maxPacketSize]);
    while (true)
    {
        socklen_t len = sizeof(m_srcAddr);
        int bytes = ::recvfrom(m_localSock, &recvbuf[0], m_maxPacketSize, 0, (struct sockaddr*)&m_srcAddr, &len);

        if(KError == bytes && (EAGAIN == errno || EWOULDBLOCK == errno)) {
            return true;
        }

        if(bytes <= 0) {
            std::cout << "udp fd" << m_localSock << ", handle read error :" 
                    << bytes << ", errno=" << errno << std::endl;
            return true;
        }

        m_onMessage(this, &recvbuf[0], bytes);
    }
    return true;
}

bool DatagramSocket::HandleWriteEvent()
{
    while(!m_sendList.empty()) {
        const auto& pkg = m_sendList.front();

        int bytes = _Send(pkg.data.data(), pkg.data.size(), pkg.dst);

        if(bytes == 0) {
            return true;
        }
        else if(bytes > 0) {
            m_sendList.pop_front();
        }
        else {
            std::cout << "Fatal error when send to" << pkg.dst.ToString() 
                    <<", must skip it" << std::endl;
            m_sendList.pop_front();
        }
    }
    //todo 

    if(m_sendList.empty()) {
        m_loop->Modify(internal::eET_Read, shared_from_this());
    }
    return true;
}

void DatagramSocket::HandleErrorEvent()
{
    std::cout << "handleErrorEvent" << std::endl;
}

bool DatagramSocket::SendPacket(const void* data, size_t size, const InetAddress* dst)
{
    if(size == 0 || !data){
        return true;
    }
    if(!dst) {
        dst = &m_srcAddr;
    }

    if(!m_sendList.empty()) {
        _PutSendBuf(data, size, dst);
        return true;
    }

    int bytes = _Send(data, size, *dst);
    if(bytes == 0) {
        _PutSendBuf(data, size, dst);
        m_loop->Modify(internal::eET_Read | internal::eET_Write, shared_from_this());
        return true;
    }
    else if(bytes < 0) {
        std::cout << "Fatal error when send udp to" << dst->ToString() << std::endl;
        return false;
    }
    return ture;
}

void DatagramSocket::_PutSendBuf(const void* data, size_t size, const InetAddress* dst)
{
    Package pkg;
    pkg.dst = *dst;
    pkg.data.assign(reinterpret_cast<const char*>(data), size);

    m_sendList.emplace_back(std::move(pkg));
}

int DatagramSocket::_Send(const void* data, size_t size, const InetAddress& dst)
{
    int bytes = ::sendto(m_localSock, data, size, 0, (const struct sockaddr*)&dst, sizeof(dst));

    if(bytes == KError && (EAGAIN == errno || EWOULDBLOCK == errno)) {
        std::cout << "send wouldblock" << std::endl;
        return 0;
    }
    return bytes;
}
}