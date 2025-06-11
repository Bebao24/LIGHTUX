#pragma once
#include <stdint.h>

#define GDT_KERNEL_CODE (0x08)
#define GDT_KERNEL_DATA (0x10)
#define GDT_USER_CODE (0x18)
#define GDT_USER_DATA (0x20)
#define GDT_TSS (0x28)

typedef struct
{
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint32_t iopb;
} __attribute__((packed)) TSSPtr;

void InitializeGDT();

