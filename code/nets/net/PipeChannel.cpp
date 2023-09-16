#include "PipeChannel.h"

namespace hxk
{

PipeChannel::PipeChannel()
{
    int fd[2];

    int ret = ::pipe(fd);
    assert(ret == 0);

    m_readFd = fd[0];
    m_writeFd = fd[1];

    SocketUtil::SetNonBlock(m_readFd, true);
    SocketUtil::SetNonBlock(m_writeFd, true);
}

PipeChannel::~PipeChannel()
{
    SocketUtil::CloseSocket(m_readFd);
    SocketUtil::CloseSocket(m_writeFd);   
}


int PipeChannel::Identifier() const 
{
    return m_readFd;
}

bool PipeChannel::HandleReadEvent() 
{
    char ch ;
    auto n = ::read(m_readFd, &ch, sizeof(ch));
    return n == 1;
}

bool PipeChannel::HandleWriteEvent() 
{
    assert(false);
    return false;
}

void PipeChannel::HandleErrorEvent() 
{

}


bool PipeChannel::Notify()
{
    char ch;
    auto n = ::write(m_writeFd, &ch, sizeof(ch));
    return n == 1;
}


}