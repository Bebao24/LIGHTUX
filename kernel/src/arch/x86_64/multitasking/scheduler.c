#include "scheduler.h"
#include <task.h>
#include <memory.h>
#include <paging.h>
#include <logging.h>
#include <pic.h>

extern void asm_finalizeScheduler(uint64_t newStack, uint64_t newPageDir);

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

    int fullRun = 0;
    while (nextTask->status != TASK_STATUS_READY)
    {
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

    // debugf("Next task: %s\n", nextTask->taskName);

    task_t* oldTask = currentTask;
    currentTask = nextTask;

    // Save the cpu registers
    memcpy(&oldTask->cpu_status, cpu_status, sizeof(cpu_registers_t));

    cpu_registers_t* iretqRsp = (cpu_registers_t*)(nextTask->cpu_status.rsp - sizeof(cpu_registers_t));
    memcpy(iretqRsp, &nextTask->cpu_status, sizeof(cpu_registers_t));

    ChangePageDirFake(nextTask->pageDir);

    PIC_SendEOI(0);
    asm_finalizeScheduler((uint64_t)iretqRsp, (uint64_t)paging_VirtToPhysical((void*)nextTask->pageDir));
}

