#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <memory>
#include <iostream>

namespace hxk
{

namespace internal
{

enum EventType
{
    eET_None = 0,           //0
    eET_Read = 0x1 << 0,    //1
    eET_Write = 0x1 << 1,   //2
    eET_Error = 0x1 << 2    //4
};

class Channel : public std::enable_shared_from_this<Channel>
{
private:
    unsigned int m_unique_id = 0;

public:
    Channel()
    {
        std::cout << "New Channel " << (void*)this << std::endl;    //for debug
    }
    ~Channel()
    {
        std::cout << "Delete Channel " << (void*)this << std::endl; //for debug
    }

    Channel(const Channel&) = delete;
    void operator=(const Channel&) = delete;


    unsigned int GetUniqueId() const        //unique_id will nerver repeat in whole process
    {
        return m_unique_id;
    }

    void SetUniqueId(unsigned int id)
    {
        m_unique_id = id;
    }

    virtual int Identifier() const = 0;         //for debug

    virtual bool HandleReadEvent() = 0;
    virtual bool HandleWriteEvent() = 0;
    virtual void HandleErrorEvent() = 0;
};

struct FiredEvent
{
    int events;         //事件类型
    void* userData;     //用户数据

    FiredEvent():events(0), userData(nullptr) {}
};

class Poller
{
protected:
    int m_mutiplexer;
    std::vector<FiredEvent> m_firedEvents;

public:
    Poller() : m_mutiplexer(-1) {}

    virtual ~Poller() {}

    virtual bool Register(int fd, int events, void* userPtr) = 0;
    virtual bool Modify(int fd, int events, void* userPtr) = 0;
    virtual bool Cancel(int fd, int events) = 0;

    virtual int Poll(std::size_t maxEvent, int timeoutMs) = 0;      //轮询事件

    const std::vector<FiredEvent>& GetFiredEvents() const
    {
        return m_firedEvents;
    }
};

}

}

#endif