#include "stack.h"
#include <vmm.h>
#include <pmm.h>
#include <memory.h>

void* StackAllocate()
{
    void* stackAddr = vmm_AllocatePages(DEFAULT_STACK_SIZE_IN_PAGES);
    size_t stackSize = DEFAULT_STACK_SIZE_IN_PAGES * PAGE_SIZE;
    memset(stackAddr, 0, stackSize);

    return (void*)((uint64_t)stackAddr + stackSize);
}

