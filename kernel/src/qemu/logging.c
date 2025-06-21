#include "logging.h"
#include <e9.h>
#include <printf.h>

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

void debugf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf_internal(debugc, fmt, args);

    va_end(args);
}

