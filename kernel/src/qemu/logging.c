#include "logging.h"
#include <e9.h>

void debugc(char c)
{
    e9Putc(c);
}

void debugs(const char* string)
{
    while (*string)
    {
        debugc(*string);
        string++;
    }
}

