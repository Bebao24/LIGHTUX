#include "logging.h"
#include <e9.h>
#include <printf.h>
#include <spinlock.h>

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

spinlock_t SERIAL_LOCK;

void debugf(const char* fmt, ...)
{
    spinlockAcquire(&SERIAL_LOCK);

    va_list args;
    va_start(args, fmt);

    printf_internal(debugc, fmt, args);

    va_end(args);

    spinlockRelease(&SERIAL_LOCK);
}

