#include "vmm.h"
#include <pmm.h>
#include <stdint.h>
#include <boot.h>
#include <paging.h>
#include <system.h>

void* vmm_AllocatePage()
{
    return vmm_AllocatePages(1);
}

void* vmm_AllocatePages(size_t numPages)
{
    uint64_t physical = (uint64_t)pmm_AllocatePages(numPages);
    uint64_t virtAddr = physical + bootInfo.hhdmOffset;

    return (void*)virtAddr;
}

void vmm_FreePage(void* addr)
{
    vmm_FreePages(addr, 1);
}

void vmm_FreePages(void* addr, size_t numPages)
{
    uint64_t physicalAddr = (uint64_t)paging_VirtToPhysical(addr);
    if (!physicalAddr)
    {
        panic("[VMM] Can't find the physical address!\n");
    }

    pmm_FreePages((void*)physicalAddr, numPages);
}

