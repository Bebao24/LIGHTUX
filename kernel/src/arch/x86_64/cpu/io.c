#include "io.h"

#define UNUSED_PORT 0x80

void x64_outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0" : : "dN"(port), "a"(value));
}

uint8_t x64_inb(uint16_t port)
{
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

void x64_outl(uint16_t port, uint32_t value)
{
    asm volatile ("outl %1, %0" : : "d"(port), "a"(value));
}

uint32_t x64_inl(uint16_t port)
{
    uint32_t result;
    asm volatile ("inl %1, %0" : "=a"(result) : "d"(port));
    return result;
}

void x64_iowait()
{
    x64_outb(UNUSED_PORT, 0);
}

