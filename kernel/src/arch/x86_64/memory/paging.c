#include "paging.h"
#include <stddef.h>
#include <pmm.h>
#include <system.h>
#include <boot.h>
#include <memory.h>

uint64_t* g_PageDir = NULL;

void InitializePaging()
{
    // Get the limine's pd
    uint64_t pdPhysicalAddr = 0;
    asm volatile("movq %%cr3,%0" : "=r"(pdPhysicalAddr));

    if (!pdPhysicalAddr)
    {
        panic("[PAGING] Can't get limine's page directory!\n");
    }

    uint64_t pdVirtAddr = pdPhysicalAddr + bootInfo.hhdmOffset;
    g_PageDir = (uint64_t*)pdVirtAddr;
}

uint64_t paging_PhysicalAllocate()
{
    uint64_t physicalAddr = (uint64_t)pmm_AllocatePage();

    void* virtAddr = (void*)(physicalAddr + bootInfo.hhdmOffset);
    memset(virtAddr, 0, PAGE_SIZE);

    return physicalAddr;
}

void invalidate(uint64_t virtualAddr)
{
    asm volatile("invlpg %0" ::"m"(virtualAddr));
}

void paging_MapPage(void* virtAddr, void* physicalAddr, uint64_t flags)
{
    // For convenient, we will cast them into uint64_t
    uint64_t virtAddress = (uint64_t)virtAddr;
    uint64_t physicalAddress = (uint64_t)physicalAddr;

    // Stripping out the upper 16 bits
    virtAddress = AMD64_MM_STRIPSX(virtAddress);

    uint64_t pml4_index = PML4E(virtAddress);
    uint64_t pdp_index = PDPTE(virtAddress);
    uint64_t pd_index = PDE(virtAddress);
    uint64_t pt_index = PTE(virtAddress);

    if (!(g_PageDir[pml4_index] & PF_PRESENT))
    {
        size_t targetAddr = paging_PhysicalAllocate();
        g_PageDir[pml4_index] = targetAddr | PF_PRESENT | PF_RW | PF_USER;
    }

    uint64_t* pdp = (uint64_t*)(PTE_GET_ADDR(g_PageDir[pml4_index]) + bootInfo.hhdmOffset);

    if (!(pdp[pdp_index] & PF_PRESENT))
    {
        size_t targetAddr = paging_PhysicalAllocate();
        pdp[pdp_index] = targetAddr | PF_PRESENT | PF_RW | PF_USER;
    }

    uint64_t* pd = (uint64_t*)(PTE_GET_ADDR(pdp[pdp_index]) + bootInfo.hhdmOffset);

    if (!(pd[pd_index] & PF_PRESENT))
    {
        size_t targetAddr =  paging_PhysicalAllocate();
        pd[pd_index] = targetAddr | PF_PRESENT | PF_RW | PF_USER;
    }

    uint64_t* pt = (uint64_t*)(PTE_GET_ADDR(pd[pd_index]) + bootInfo.hhdmOffset);

    pt[pt_index] = (P_PHYS_ADDR(physicalAddress)) | PF_PRESENT | flags;
    invalidate(virtAddress);
}


