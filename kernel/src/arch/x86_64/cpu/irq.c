#include "irq.h"
#include <isr.h>
#include <pic.h>
#include <stddef.h>
#include <logging.h>

IRQHandler g_IRQHandlers[IRQ_ENTRIES];

void IRQ_Handler(cpu_registers_t* cpu_status)
{
    int irq = cpu_status->interrupt_number - PIC_REMAP_OFFSET;

    if (g_IRQHandlers[irq] != NULL)
    {
        g_IRQHandlers[irq](cpu_status);
    }
    else
    {
        // Unhandled IRQ
        // For some reason, I can't mask the PIT
        if (irq != 0)
        {
            debugf("[IRQ] Unhandled IRQ: %d\n", irq);
        }
    }

    PIC_SendEOI(irq);
}

void InitializeIRQ()
{
    // Already remap the PIC in isr.c

    for (int i = 0; i < 16; i++)
    {
        ISR_RegisterHandler(PIC_REMAP_OFFSET + i, IRQ_Handler);
    }
}

void IRQ_RegisterHandler(int irq, IRQHandler handler)
{
    g_IRQHandlers[irq] = handler;
}

