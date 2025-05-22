#include "timer.h"
#include <io.h>
#include <irq.h>
#include <scheduler.h>

uint64_t ticks;
uint32_t frequency = 1193;

void timerTick(cpu_registers_t* cpu_status)
{
    (void)cpu_status;
    ticks++;

    schedule(cpu_status);
}

void InitializeTimer()
{
    ticks = 0;

    // Register the IRQ handler for the PIT
    IRQ_RegisterHandler(0, timerTick);

    uint32_t divisor = 1193180 / frequency;

    x64_outb(0x43, 0x36);
    x64_outb(0x40, (uint8_t)(divisor & 0xFF));
    x64_outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void sleep(uint64_t miliseconds)
{
    uint64_t startTick = ticks;
    while (ticks < startTick + miliseconds)
    {
        asm volatile ("hlt");
    }
}

