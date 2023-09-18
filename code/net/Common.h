#ifndef COMMON_H
#define COMMON_H

namespace hxk
{

enum EventType
{
    None = 0,           //0
    Read = 0x1 << 0,    //1
    Write = 0x1 << 1,   //2
    ET = 0x1 << 2,      //4
    Error = 0x1 << 3,   //8
};

#define DISALLOW_COPY(cname)     \
    cname(const cname &) = delete; \
    cname &operator=(const cname &) = delete;

#define DISALLOW_MOVE(cname) \
    cname(cname &&) = delete;  \
    cname &operator=(cname &&) = delete;

#define DISALLOW_COPY_AND_MOVE(cname) \
    DISALLOW_COPY(cname);               \
    DISALLOW_MOVE(cname);

void Errif(bool condition, const char* msg);

}

#endif