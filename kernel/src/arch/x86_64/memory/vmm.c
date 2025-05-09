#include "vmm.h"
#include <pmm.h>
#include <stdint.h>
#include <boot.h>

void* vmm_AllocatePage()
{
    uint64_t physical = (uint64_t)pmm_AllocatePage();
    uint64_t virtAddr = physical + bootInfo.hhdmOffset;

    return (void*)virtAddr;
}

void* vmm_AllocatePages(size_t numPages)
{
    uint64_t physical = (uint64_t)pmm_AllocatePages(numPages);
    uint64_t virtAddr = physical + bootInfo.hhdmOffset;

    return (void*)virtAddr;
}

// TODO: Make a vmm_FreePages() function



