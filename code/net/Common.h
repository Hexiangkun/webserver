#ifndef COMMON_H
#define COMMON_H

namespace hxk
{

#define DISALLOW_COPY(cname)     \
    cname(const cname &) = delete; \
    cname &operator=(const cname &) = delete;

#define DISALLOW_MOVE(cname) \
    cname(cname &&) = delete;  \
    cname &operator=(cname &&) = delete;

#define DISALLOW_COPY_AND_MOVE(cname) \
    DISALLOW_COPY(cname);               \
    DISALLOW_MOVE(cname);


}

#endif