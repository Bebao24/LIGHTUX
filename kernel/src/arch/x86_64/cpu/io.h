#pragma once
#include <stdint.h>

void x64_outb(uint16_t port, uint8_t value);
uint8_t x64_inb(uint16_t port);

void x64_outl(uint16_t port, uint32_t value);
uint32_t x64_inl(uint16_t port);

void x64_iowait();

