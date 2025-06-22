#include "gdt.h"
#include <stdint.h>
#include <memory.h>

typedef struct
{
    uint16_t Limit;
    uint16_t BaseLow;
    uint8_t BaseMid;
    uint8_t Access;
    uint8_t Granularity;
    uint8_t BaseHigh;
} __attribute__((packed)) GDTEntry;

typedef struct
{
    uint16_t Limit;
    uint64_t Base;
} __attribute__((packed)) GDTR;

typedef struct
{
    uint16_t Length;
    uint16_t BaseLow;
    uint8_t BaseMid;
    uint8_t Flags1;
    uint8_t Flags2;
    uint8_t BaseHigh;
    uint32_t BaseUpper;
    uint32_t Reserved;
} __attribute__((packed)) TSSEntry;

typedef struct
{
    GDTEntry descriptors[5];
    TSSEntry tss;
} __attribute__((packed)) GDTEntries;

// Setup the GDTR
static GDTR gdtr;
static TSSPtr tss;
static GDTEntries gdt;

TSSPtr* tssPtr = &tss;

extern void LoadGDT(GDTR* gdtr);

void GDTLoadTSS(TSSPtr* ptr)
{
    // Construct the TSS base
    uint64_t addr = (uint64_t)ptr;

    gdt.tss.BaseLow = (uint16_t)addr;
    gdt.tss.BaseMid = (uint16_t)(addr >> 16);
    gdt.tss.Flags1 = 0b10001001;
    gdt.tss.Flags2 = 0;
    gdt.tss.BaseHigh = (uint8_t)(addr >> 24);
    gdt.tss.BaseUpper = (uint32_t)(addr >> 32);
    gdt.tss.Reserved = 0;

    asm volatile("ltr %0" : : "rm"((uint16_t)GDT_TSS) : "memory");
}

void InitializeGDT()
{
    // NULL descriptor
    gdt.descriptors[0].Limit = 0;
    gdt.descriptors[0].BaseLow = 0;
    gdt.descriptors[0].BaseMid = 0;
    gdt.descriptors[0].Access = 0;
    gdt.descriptors[0].Granularity = 0;
    gdt.descriptors[0].BaseHigh = 0;

    // Kernel code 64 bits
    gdt.descriptors[1].Limit = 0;
    gdt.descriptors[1].BaseLow = 0;
    gdt.descriptors[1].BaseMid = 0;
    gdt.descriptors[1].Access = 0b10011010;
    gdt.descriptors[1].Granularity = 0b00100000;
    gdt.descriptors[1].BaseHigh = 0;

    // Kernel data 64 bits
    gdt.descriptors[2].Limit = 0;
    gdt.descriptors[2].BaseLow = 0;
    gdt.descriptors[2].BaseMid = 0;
    gdt.descriptors[2].Access = 0b10010010;
    gdt.descriptors[2].Granularity = 0;
    gdt.descriptors[2].BaseHigh = 0;

    // User code 64 bits
    gdt.descriptors[3].Limit = 0;
    gdt.descriptors[3].BaseLow = 0;
    gdt.descriptors[3].BaseMid = 0;
    gdt.descriptors[3].Access = 0b11111010;
    gdt.descriptors[3].Granularity = 0b00100000;
    gdt.descriptors[3].BaseHigh = 0;

    // User data 64 bits
    gdt.descriptors[4].Limit = 0;
    gdt.descriptors[4].BaseLow = 0;
    gdt.descriptors[4].BaseMid = 0;
    gdt.descriptors[4].Access = 0b11110010;
    gdt.descriptors[4].Granularity = 0;
    gdt.descriptors[4].BaseHigh = 0;

    // TSS 64 bits
    // We will construct this later (after the GDT is loaded)
    gdt.tss.Length = 104;
    gdt.tss.BaseLow = 0;
    gdt.tss.BaseMid = 0;
    gdt.tss.Flags1 = 0b10001001;
    gdt.tss.Flags2 = 0;
    gdt.tss.BaseHigh = 0;
    gdt.tss.BaseUpper = 0;
    gdt.tss.Reserved = 0;

    gdtr.Limit = sizeof(GDTEntries) - 1;
    gdtr.Base = (uint64_t)&gdt;

    LoadGDT(&gdtr);

    memset(&tss, 0, sizeof(TSSPtr));
    GDTLoadTSS(&tss);
}

