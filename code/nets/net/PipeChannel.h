#ifndef PIPECHANNEL_H
#define PIPECHANNEL_H

#include <unistd.h>
#include <assert.h>

#include "Poller.h"
#include "SocketUtil.h"

namespace hxk
{

class PipeChannel : public internal::Channel
{
public:
    PipeChannel();
    ~PipeChannel();

    PipeChannel(const PipeChannel&) = delete;
    void operator=(const PipeChannel&) = delete;

    int Identifier() const override;

    bool HandleReadEvent() override;

    bool HandleWriteEvent() override;

    void HandleErrorEvent() override;

    bool Notify();

private:
    int m_readFd;
    int m_writeFd;
};


}


#endif