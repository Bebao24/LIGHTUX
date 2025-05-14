#pragma once
#include <cpu.h>

#define ISR_ENTRIES 256

typedef void (*ISRHandler)(cpu_registers_t* cpu_status);

extern void* isr_stub_table[];

void InitializeISR();
void ISR_RegisterHandler(int interrupt, ISRHandler handler);

