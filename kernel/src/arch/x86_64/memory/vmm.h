#pragma once
#include <stddef.h>

void* vmm_AllocatePage();
void* vmm_AllocatePages(size_t numPages);

