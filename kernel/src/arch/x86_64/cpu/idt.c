#include "idt.h"
#include <gdt.h>

typedef struct
{
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t BaseMid;
    uint32_t BaseHigh;
    uint32_t Reserved;
} __attribute__((packed)) IDTEntry;

typedef struct
{
    uint16_t Limit;
    uint64_t Base;
} __attribute__((packed)) IDTR;

IDTEntry idtEntries[IDT_TOTAL_ENTRIES];
IDTR idtr;

void IDT_SetGate(int interrupts, uint64_t handler, uint8_t flags)
{
    idtEntries[interrupts].BaseLow = (uint16_t)handler;
    idtEntries[interrupts].SegmentSelector = GDT_KERNEL_CODE;
    idtEntries[interrupts].ist = 0;
    idtEntries[interrupts].attributes = flags;
    idtEntries[interrupts].BaseMid = (uint16_t)(handler >> 16);
    idtEntries[interrupts].BaseHigh = (uint32_t)(handler >> 32);
    idtEntries[interrupts].Reserved = 0;
}

void InitializeIDT()
{
    idtr.Limit = sizeof(idtEntries) - 1;
    idtr.Base = (uint64_t)&idtEntries;

    // Load the IDT
    asm volatile ("lidt %0" : : "m"(idtr) : "memory");
}



