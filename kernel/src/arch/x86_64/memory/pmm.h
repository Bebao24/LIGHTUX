#pragma once
#include <stddef.h>

#define PAGE_SIZE 0x1000
#define INVALID_PAGE ((size_t)-1)

void InitializePMM();

/* Locking & freeing page */
void pmm_LockPage(void* addr);
void pmm_FreePage(void* addr);

void pmm_LockPages(void* addr, size_t numPages);
void pmm_FreePages(void* addr, size_t numPages);

/* Allocating */
void* pmm_AllocatePage();
void* pmm_AllocatePages(size_t numPages);

/* Getting pmm info */
size_t pmm_GetFreeMem();
size_t pmm_GetUsedMem();
size_t pmm_GetReservedMem();
