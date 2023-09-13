#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <memory>
#include <iostream>

namespace hxk
{

namespace internal
{



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



}

}

#endif