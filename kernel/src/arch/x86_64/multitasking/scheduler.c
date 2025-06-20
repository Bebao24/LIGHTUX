#include "scheduler.h"
#include <task.h>
#include <memory.h>
#include <paging.h>
#include <logging.h>
#include <pic.h>
#include <stack.h>
#include <pmm.h>
#include <vmm.h>
#include <gdt.h>

extern void asm_finalizeScheduler(uint64_t newStack, uint64_t newPageDir);

void schedulerDeleteTask(size_t taskId);

extern TSSPtr* tssPtr;

void schedule(cpu_registers_t* cpu_status)
{
    if (!taskInitialized || !schedulerReady)
    {
        return;
    }

    // Find the next task
    task_t* nextTask = currentTask->next;
    if (!nextTask)
    {
        // Back to the first task
        nextTask = firstTask;
    }

    if (currentTask->status != TASK_STATUS_DEAD && currentTask->status != TASK_STATUS_READY)
    {
        currentTask->status = TASK_STATUS_READY;
    }

    int fullRun = 0;
    while (nextTask->status != TASK_STATUS_READY)
    {
        if (nextTask->status == TASK_STATUS_DEAD)
        {
            schedulerDeleteTask(nextTask->id);
        }

        nextTask = nextTask->next;

        if (!nextTask)
        {
            fullRun++;
            if (fullRun > 2)
            {
                break;
            }

            nextTask = firstTask;
        }
    }

    // Use dummy task if found no task
    if (!nextTask)
    {
        nextTask = dummyTask;
    }

    nextTask->status = TASK_STATUS_RUNNING;
    
    task_t* oldTask = currentTask;
    currentTask = nextTask;

    // Change TSS rsp0
    tssPtr->rsp0 = nextTask->rsp0;

    // Save the cpu registers
    memcpy(&oldTask->cpu_status, cpu_status, sizeof(cpu_registers_t));

    cpu_registers_t* iretqRsp = (cpu_registers_t*)(nextTask->cpu_status.rsp - sizeof(cpu_registers_t));
    memcpy(iretqRsp, &nextTask->cpu_status, sizeof(cpu_registers_t));

    ChangePageDirFake(nextTask->pageDir);

    PIC_SendEOI(0);
    asm_finalizeScheduler((uint64_t)iretqRsp, (uint64_t)paging_VirtToPhysical((void*)nextTask->pageDir));
}

void schedulerDeleteTask(size_t taskId)
{
    task_t* task = TaskGet(taskId);
    if (!task)
    {
        return;
    }

    // Free the stack
    size_t stackSize = DEFAULT_STACK_SIZE_IN_PAGES * PAGE_SIZE;
    void* stack = (void*)(task->cpu_status.rsp - stackSize);
    vmm_FreePages(stack, DEFAULT_STACK_SIZE_IN_PAGES);

    // Free the pd
    void* pageDir = task->pageDir;
    vmm_FreePage(pageDir);

    // Finally, free the task itself
    TaskFree(task);
}

