#include "Common.h"
#include <stdio.h>
#include <stdlib.h>

namespace hxk
{

void Errif(bool condition, const char* msg)
{
    if(condition) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
}