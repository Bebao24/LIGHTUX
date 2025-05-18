#include "task.h"
#include <heap.h>
#include <memory.h>
#include <paging.h>
#include <logging.h>

// First task is just the kernel
task_t* firstTask;
task_t* currentTask;
bool taskInitialized;

task_t* TaskGet(size_t id)
{
    // TODO: Implement spinlock
    asm volatile ("cli");

    task_t* browse = firstTask;
    while (browse)
    {
        if (browse->id == id)
        {
            break;
        }

        browse = browse->next;
    }

    asm volatile ("sti");
}

void taskName(task_t* task, char* name, size_t len)
{
    task->taskName = malloc(len);
    memcpy(task->taskName, name, len);
    task->taskNameLen = len;
}

void InitializeTask()
{
    // Setup the kernel task
    firstTask = (task_t*)malloc(sizeof(task_t));
    memset(firstTask, 0, sizeof(task_t));

    currentTask = firstTask;
    currentTask->id = TASK_KERNEL_ID;
    currentTask->status = TASK_STATUS_READY;
    currentTask->pageDir = GetPageDir();
    taskName(currentTask, TASK_KERNEL_NAME, sizeof(TASK_KERNEL_NAME));

    debugf("[TASK] Ready for multitasking!\n");
    taskInitialized = true;
}

