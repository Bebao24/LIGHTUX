#pragma once
#include <stdint.h>

#define IDT_TOTAL_ENTRIES 256

void InitializeIDT();
void IDT_SetGate(int interrupts, uint64_t handler, uint8_t flags);

