#include "gdt.h"
#include <stdint.h>
#include <memory.h>
#include <logging.h>

static uint64_t gdtEntries[7];
static uint64_t tssDescriptor[2];

typedef struct
{
    uint16_t Limit;
    uint64_t Base;
} __attribute__((packed)) GDTR;

// Setup the GDTR
static GDTR gdtr;
static TSSPtr tss;

TSSPtr* tssPtr = &tss;

extern void LoadGDT(GDTR* gdtr);

void ConstructTSSDescriptor(uint64_t base, uint64_t* descriptor)
{
    // Construct the TSS descriptor from the TSS Ptr
    // The TSS descriptor is 128 bits
    uint64_t high = 0;
    uint64_t low = 0;

    low |= sizeof(TSSPtr) - 1; // Limit
    low |= (base & 0xFFFFFF) << 16; // Lower base
    low |= (uint64_t)0x9 << 40; // Type = 0x9 (64 bits TSS)
    low |= (uint64_t)1 << 47; // Present
    low |= ((base >> 24) & 0xFF) << 56; // Middle base

    high |= (base >> 32); // Upper base

    // Assign the descriptor
    descriptor[0] = low;
    descriptor[1] = high;
}

void GDTLoadTSS()
{
    asm volatile("ltr %0" : : "rm"((uint16_t)GDT_TSS) : "memory");
}

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

    // Add the TSS to the GDT entries
    ConstructTSSDescriptor((uint64_t)&tss, tssDescriptor);
    gdtEntries[5] = tssDescriptor[0];
    gdtEntries[6] = tssDescriptor[1];

    gdtr.Limit = sizeof(gdtEntries) - 1;
    gdtr.Base = (uint64_t)gdtEntries;

    // Load and flush the GDT
    LoadGDT(&gdtr);

    // Load the TSS
    memset(&tss, 0, sizeof(TSSPtr));
    GDTLoadTSS(&tss);
}

