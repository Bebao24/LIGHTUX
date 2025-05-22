#include "paging.h"
#include <stddef.h>
#include <pmm.h>
#include <vmm.h>
#include <system.h>
#include <boot.h>
#include <memory.h>
#include <task.h>
#include <spinlock.h>

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

uint64_t* paging_AllocatePD()
{
    // Allocate a pd and then copy the kernel's pd
    if (!taskInitialized)
    {
        panic("[PAGING] Can't allocate pd without multitasking initialized!\n");
    }

    uint64_t* out = vmm_AllocatePage();

    // Get the kernel's pd
    uint64_t* target = TaskGet(TASK_KERNEL_ID)->pageDir;

    for (int i = 0; i < 512; i++)
    {
        out[i] = target[i];
    }

    return out;
}

// Will not check for task
void ChangePageDirUnsafe(uint64_t* pd)
{
    uint64_t target = (uint64_t)paging_VirtToPhysical((void*)pd);

    if (!target)
    {
        panic("[PAGING] Can't change to pd: 0x%llx\n", pd);
    }

    asm volatile("movq %0, %%cr3" ::"r"(target));
    g_PageDir = pd;
}

// Only modify the g_PageDir, not cr3
void ChangePageDirFake(uint64_t* pd)
{
    uint64_t target = (uint64_t)paging_VirtToPhysical((void*)pd);

    if (!target)
    {
        panic("[PANIC] Can't change g_PageDir to pd: 0x%llx\n", pd);
    }

    g_PageDir = pd;
}

spinlock_t PAGING_LOCK;

void ChangePageDir(uint64_t* pd)
{
    // Check for task
    if (taskInitialized)
    {
        currentTask->pageDir = pd;
    }

    ChangePageDirUnsafe(pd);
}

uint64_t* GetPageDir()
{
    return (uint64_t*)g_PageDir;
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

    spinlockAcquire(&PAGING_LOCK);

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

    spinlockRelease(&PAGING_LOCK);
}

void* paging_VirtToPhysical(void* virtAddr)
{
    uint64_t virtAddress = (uint64_t)virtAddr;

    if (virtAddress >= bootInfo.hhdmOffset && virtAddress <= (bootInfo.hhdmOffset + bootInfo.mmTotal))
    {
        return (void*)(virtAddress - bootInfo.hhdmOffset);
    }

    // Save virtual address
    uint64_t virtAddr_init = virtAddress;

    virtAddress &= ~(0xFFF);
    // Strip the upper 16 bits
    virtAddress = AMD64_MM_STRIPSX(virtAddress);

    uint64_t pml4_index = PML4E(virtAddress);
    uint64_t pdp_index = PDPTE(virtAddress);
    uint64_t pd_index = PDE(virtAddress);
    uint64_t pt_index = PTE(virtAddress);

    if (!(g_PageDir[pml4_index] & PF_PRESENT))
    {
        goto error;
    }

    uint64_t* pdp = (uint64_t*)(PTE_GET_ADDR(g_PageDir[pml4_index]) + bootInfo.hhdmOffset);

    if (!(pdp[pdp_index] & PF_PRESENT))
    {
        goto error;
    }

    uint64_t* pd = (uint64_t*)(PTE_GET_ADDR(pdp[pdp_index]) + bootInfo.hhdmOffset);

    if (!(pd[pd_index] & PF_PRESENT))
    {
        goto error;
    }

    uint64_t* pt = (uint64_t*)(PTE_GET_ADDR(pd[pd_index]) + bootInfo.hhdmOffset);

    if (pt[pt_index] & PF_PRESENT)
    {
        return (void*)(PTE_GET_ADDR(pt[pt_index]) + ((uint64_t)virtAddr_init & 0xFFF));
    }

    // Will go to error otherwise

error:
    return 0;
}


