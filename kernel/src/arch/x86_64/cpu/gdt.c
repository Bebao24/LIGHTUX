#include "gdt.h"
#include <stdint.h>

uint64_t gdtEntries[5];

typedef struct
{
    uint16_t Limit;
    uint64_t Base;
} __attribute__((packed)) GDTR;

// Setup the GDTR
GDTR gdtr;

extern void LoadGDT(GDTR* gdtr);

void InitializeGDT()
{
    // NULL entry
    gdtEntries[0] = 0;

    // Kernel code entry
    uint64_t kernelCode = 0;
    kernelCode |= 0b1011 << 8; // kernel code
    kernelCode |= 1 << 12; // Not a system descriptor
    kernelCode |= 0 << 13; // DPL = 0
    kernelCode |= 1 << 15; // Present
    kernelCode |= 1 << 21; // Long mode segment
    gdtEntries[1] = kernelCode << 32;

    uint64_t kernelData = 0;
    kernelData |= 0b0011 << 8; // kernel data
    kernelData |= 1 << 12; // Not a system descriptor
    kernelData |= 0 << 13; // DPL = 0
    kernelData |= 1 << 15; // Present
    kernelData |= 1 << 21; // Long mode segment
    gdtEntries[2] = kernelData << 32;

    uint64_t userCode = kernelCode | (3 << 13); // DPL = 3
    gdtEntries[3] = userCode;

    uint64_t userData = kernelData | (3 << 13); // DPL = 3
    gdtEntries[4] = userData;

    gdtr.Limit = sizeof(gdtEntries) - 1;
    gdtr.Base = (uint64_t)gdtEntries;

    // Load and flush the GDT
    LoadGDT(&gdtr);

}

