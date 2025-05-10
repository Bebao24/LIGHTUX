#pragma once
#include <stddef.h>

void* vmm_AllocatePage();
void* vmm_AllocatePages(size_t numPages);

void vmm_FreePage(void* addr);
void vmm_FreePages(void* addr, size_t numPages);

