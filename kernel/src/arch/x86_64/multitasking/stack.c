#include "stack.h"
#include <vmm.h>
#include <pmm.h>
#include <memory.h>
#include <system.h>
#include <paging.h>

// Allocate a kernel stack and return the stack top
void* StackAllocate()
{
    void* stackAddr = vmm_AllocatePages(DEFAULT_STACK_SIZE_IN_PAGES);
    size_t stackSize = DEFAULT_STACK_SIZE_IN_PAGES * PAGE_SIZE;
    memset(stackAddr, 0, stackSize);

    return (void*)((uint64_t)stackAddr + stackSize);
}

// Map the user stack
void StackMapUser(task_t* task)
{
    // Check if the page dir is valid
    if (GetPageDir() != task->pageDir)
    {
        panic("[STACK] DO NOT use StackMapUser() if current pageDir != task->pageDir!");
    }

    for (int i = 0; i < DEFAULT_STACK_SIZE_IN_PAGES; i++)
    {
        // The stack grows downward
        size_t virtAddr = USER_STACK_TOP - DEFAULT_STACK_SIZE_IN_PAGES * PAGE_SIZE + i * PAGE_SIZE;
        paging_MapPage((void*)virtAddr, pmm_AllocatePage(), PF_USER | PF_RW);
        memset((void*)virtAddr, 0, PAGE_SIZE);
    }
}

void StackGenerateUser(task_t* task)
{
    // Switch to the user's page dir temporarily
    void* oldPageDir = GetPageDir();
    ChangePageDir(task->pageDir);

    StackMapUser(task);

    // Switch back to kernel's page dir
    ChangePageDir(oldPageDir);
}
