#ifndef PIPECHANNEL_H
#define PIPECHANNEL_H

#include <unistd.h>
#include <assert.h>

#include "poller.h"
#include "SocketUtil.h"

namespace hxk
{
namespace internal
{

class PipeChannel : public internal::Channel
{
public:
    PipeChannel()
    {
        int fd[2];

        int ret = ::pipe(fd);
        assert(ret == 0);

        m_readFd = fd[0];
        m_writeFd = fd[1];

        SetNonBlock(m_readFd, true);
        SetNonBlock(m_writeFd, true);
    }
    ~PipeChannel()
    {
        ::close(m_readFd);
        ::close(m_writeFd);    
    }

    PipeChannel(const PipeChannel&) = delete;
    void operator=(const PipeChannel&) = delete;

    int Identifier() const override
    {
        return m_readFd;
    }

    bool HandleReadEvent() override
    {
        char ch ;
        auto n = ::read(m_readFd, &ch, sizeof(ch));
        return n == 1;
    }

    bool HandleWriteEvent() override
    {
        assert(false);
        return false;
    }

    void HandleErrorEvent() override
    {

    }

    bool Notify()
    {
        char ch;
        auto n = ::write(m_writeFd, &ch, sizeof(ch));
        return n == 1;
    }

private:
    int m_readFd;
    int m_writeFd;
};


}
}


#endif