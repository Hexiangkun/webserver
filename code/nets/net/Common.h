#ifndef MACROS_H
#define MACROS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

namespace hxk
{

#define ASSERT(expr, message) assert((expr) && (message))

#define DISALLOW_COPY(cname)    \
    cname(const cname&) = delete;   \
    cname& operator=(const cname&) = delete;

#define DISALLOW_MOVE(cname)    \
    cname(cname&&) = delete;    \
    cname& operaotr=(cname&&) = delete; 

#define DISALLOW_COPY_AND_MOVE(cname)   \
    DISALLOW_COPY(cname);        \
    DISALLOW_MOVE(canme);


void ERRIF(bool condition, const char* msg);

}


#endif


