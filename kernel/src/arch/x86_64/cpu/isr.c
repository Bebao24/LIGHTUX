#include "isr.h"
#include <console.h>
#include <logging.h>
#include <idt.h>
#include <stddef.h>
#include <system.h>
#include <pic.h>

ISRHandler g_ISRHandlers[ISR_ENTRIES];

// A list of all possible exceptions...
static const char* const g_Exceptions[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

void interrupt_handler(cpu_registers_t* cpu_status)
{
    if (g_ISRHandlers[cpu_status->interrupt_number] != NULL)
    {
        g_ISRHandlers[cpu_status->interrupt_number](cpu_status);
    }
    else if (cpu_status->interrupt_number >= 32)
    {
        // Probably an IRQ or syscall
        debugf("[ISR] Unhandled interrupt: 0x%x\n", cpu_status->interrupt_number);
    }
    else
    {
        // If the interrupt number < 32 then it is an exception triggered by the CPU
        // TODO: Print out all the registers' value
        if (cpu_status->interrupt_number == 14)
        {
            // Page fault
            uint64_t pageFault_addr;
            asm volatile("movq %%cr2, %0" : "=r"(pageFault_addr));
            debugf("[ISR] Page fault occured at address %llx\n", pageFault_addr);
        }
        debugf("Error code: %d\n", cpu_status->error_code);
        panic("[ISR] Exception: %s\n", g_Exceptions[cpu_status->interrupt_number]);
    }
}

void InitializeISR()
{
    PIC_Remap(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    for (int i = 0; i < 48; i++)
    {
        IDT_SetGate(i, (uint64_t)isr_stub_table[i], 0x8E);
    }

    // Re-enable interrupts
    asm volatile ("sti");
}

void ISR_RegisterHandler(int interrupt, ISRHandler handler)
{
    g_ISRHandlers[interrupt] = handler;
}
