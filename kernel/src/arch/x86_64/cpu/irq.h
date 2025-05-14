#pragma once
#include <cpu.h>

#define IRQ_ENTRIES 16

typedef void (*IRQHandler)(cpu_registers_t* cpu_status);

void InitializeIRQ();
void IRQ_RegisterHandler(int irq, IRQHandler handler);

