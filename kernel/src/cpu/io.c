#include "io.h"

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

