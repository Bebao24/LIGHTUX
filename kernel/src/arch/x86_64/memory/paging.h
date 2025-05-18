#pragma once
#include <stdint.h>

// Paging flags
#define PF_PRESENT (1 << 0)
#define PF_RW (1 << 1)
#define PF_USER (1 << 2)
#define PF_PWT (1 << 3)
#define PF_PCD (1 << 4)
#define PF_ACCESS (1 << 5)
#define PF_DIRTY (1 << 6)
#define PF_PS (1 << 7)
#define PF_PAT (1 << 7)
#define PF_GLOBAL (1 << 8)
#define PF_SHARED (1 << 9)

#define PGSHIFT_PML4E 39
#define PGSHIFT_PDPTE 30
#define PGSHIFT_PDE 21
#define PGSHIFT_PTE 12
#define PGMASK_ENTRY 0x1ff
#define PGMASK_OFFSET 0x3ff

#define PML4E(a) (((a) >> PGSHIFT_PML4E) & PGMASK_ENTRY)
#define PDPTE(a) (((a) >> PGSHIFT_PDPTE) & PGMASK_ENTRY)
#define PDE(a) (((a) >> PGSHIFT_PDE) & PGMASK_ENTRY)
#define PTE(a) (((a) >> PGSHIFT_PTE) & PGMASK_ENTRY)

#define AMD64_MM_STRIPSX(a) ((uintptr_t)(a) & 0xFFFFFFFFFFFF)

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define P_PHYS_ADDR(x) ((x) & ~0xFFF)

void InitializePaging();

void paging_MapPage(void* virtAddr, void* physicalAddr, uint64_t flags);
void* paging_VirtToPhysical(void* virtAddr);

uint64_t* GetPageDir();
uint64_t* paging_AllocatePD();

void ChangePageDirUnsafe(uint64_t* pd);
void ChangePageDirFake(uint64_t* pd);
void ChangePageDir(uint64_t* pd);
