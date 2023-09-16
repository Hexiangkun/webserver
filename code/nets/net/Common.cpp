#include "Common.h"

namespace hxk
{

void ERRIF(bool condition, const char* msg)
{
    if(condition) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
}