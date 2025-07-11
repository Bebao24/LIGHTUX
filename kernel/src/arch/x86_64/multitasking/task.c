#include "task.h"
#include <heap.h>
#include <memory.h>
#include <paging.h>
#include <logging.h>
#include <gdt.h>
#include <stack.h>
#include <spinlock.h>
#include <system.h>

// First task is just the kernel
task_t* firstTask;
task_t* currentTask;
task_t* dummyTask;
bool taskInitialized;
bool schedulerReady;

size_t currentID = 0;
size_t GetFreeID()
{
    return currentID++;
}

spinlock_t TASK_LOCK;

task_t* TaskAllocate()
{
    task_t* task = (task_t*)malloc(sizeof(task_t));
    memset(task, 0, sizeof(task_t));

    asm volatile ("cli");

    // Put the new task in the final of the linked list
    task_t* browse = firstTask;
    while (browse)
    {
        if (!browse->next)
        {
            break;
        }

        browse = browse->next;
    }

    browse->next = task;
    asm volatile ("sti");

    return task;
}

void TaskFree(task_t* target)
{
    asm volatile ("cli");

    task_t* browse = firstTask;
    while (browse)
    {
        if (browse->next == target)
        {
            break;
        }

        browse = browse->next;
    }

    browse->next = target->next;

    asm volatile ("sti");
    free(target);
}

task_t* TaskCreate(uint64_t entry, uint64_t* pageDir, void* arg, bool isKernelTask)
{
    task_t* task = TaskAllocate();

    task->id = GetFreeID();
    task->status = TASK_STATUS_CREATED; // Not ready yet
    task->pageDir = pageDir;

    // Setup the code & stack selector
    if (isKernelTask)
    {
        task->cpu_status.ss = GDT_KERNEL_DATA;
        task->cpu_status.cs = GDT_KERNEL_CODE;
    }
    else
    {
        task->cpu_status.ss = GDT_USER_DATA | 0x03;
        task->cpu_status.cs = GDT_USER_CODE | 0x03;
    }

    StackGenerateUser(task);
    task->cpu_status.rsp = USER_STACK_TOP;
    task->rsp0 = (uint64_t)StackAllocate();
    task->isKernelTask = isKernelTask;
    task->cpu_status.rflags = 0x202; // Enable interrupts and a legacy feature
    task->cpu_status.rip = entry;
    task->cpu_status.rdi = (uint64_t)arg;
    task->cpu_status.rbp = 0;

    return task;
}

void TaskKill(size_t id)
{
    task_t* task = TaskGet(id);
    if (!task)
    {
        return;
    }

    // TODO: Maybe make a reaper task!

    // Will be handled by the scheduler
    task->status = TASK_STATUS_DEAD;
    // while (true);
}

task_t* TaskGet(size_t id)
{
    spinlockAcquire(&TASK_LOCK);

    task_t* browse = firstTask;
    while (browse)
    {
        if (browse->id == id)
        {
            break;
        }

        browse = browse->next;
    }

    spinlockRelease(&TASK_LOCK);

    return browse;
}

void TaskList()
{
    // Useful for debugging...
    task_t* browse = firstTask;
    while (browse)
    {
        debugf("Task name: %s\n", browse->taskName);
        browse = browse->next;
    }
}

void taskName(task_t* task, char* name, size_t len)
{
    task->taskName = malloc(len);
    memcpy(task->taskName, name, len);
    task->taskNameLen = len;
}

void idle()
{
    while (true)
    {
        asm volatile ("pause");
    }
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
    currentTask->cpu_status.rflags = 0x202;
    currentTask->isKernelTask = true;
    currentTask->rsp0 = (uint64_t)StackAllocate();
    taskName(currentTask, TASK_KERNEL_NAME, sizeof(TASK_KERNEL_NAME));

    debugf("[TASK] Ready for basic multitasking!\n");
    taskInitialized = true;

    // Create a dummy task
    dummyTask = TaskCreate((uint64_t)idle, paging_AllocatePD(), 0, false);
    dummyTask->status = TASK_STATUS_DUMMY;

    taskName(dummyTask, TASK_DUMMY_NAME, sizeof(TASK_DUMMY_NAME));

    schedulerReady = true;
    debugf("[TASK] Ready for scheduler!\n");
}

