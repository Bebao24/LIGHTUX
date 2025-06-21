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

    // Manually constructing the IRETQ frame
    uint64_t* iretqRsp = (uint64_t*)(nextTask->rsp0);

    if (!nextTask->isKernelTask)
    {
        // Push usermode iretq frame
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.ss); // SS
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rsp); // RSP
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rflags); // RFLAGS (IF = 1)
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.cs); // CS
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rip); // RIP (entry)

        // Push two dummy error code and interrupts number
        PUSH_TO_STACK(iretqRsp, uint64_t, 0); // Error code
        PUSH_TO_STACK(iretqRsp, uint64_t, 0); // Interrupts number

        // Push general-purposes registers
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r15);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r14);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r13);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r12);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r11);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r10);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r9);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.r8);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rdi);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rsi);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rbp);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rdx);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rcx);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rbx);
        PUSH_TO_STACK(iretqRsp, uint64_t, nextTask->cpu_status.rax);
    }
    else
    {
        // Copy the entire CPU status to the stack
        iretqRsp = (uint64_t*)(nextTask->rsp0 - sizeof(cpu_registers_t));
        memcpy(iretqRsp, &nextTask->cpu_status, sizeof(cpu_registers_t));
    }

    debugf("IRETQ frame:\n");
    debugf(" RIP = 0x%llx\n", nextTask->cpu_status.rip);
    debugf(" CS  = 0x%llx\n", nextTask->cpu_status.cs);
    debugf(" RFLAGS = 0x%llx\n", nextTask->cpu_status.rflags);
    debugf(" RSP = 0x%llx\n", nextTask->cpu_status.rsp);
    debugf(" SS  = 0x%llx\n", nextTask->cpu_status.ss);

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

