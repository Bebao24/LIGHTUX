#include "e9.h"
#include <io.h>

#define SERIAL_PORT 0xE9

void e9Putc(char c)
{
    x64_outb(SERIAL_PORT, c);
}


