#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <cpu.h>

#define TASK_KERNEL_ID 0

#define TASK_KERNEL_NAME "kernel"

typedef enum
{
    TASK_STATUS_READY = 0,
    TASK_STATUS_RUNNING = 1,
    TASK_STATUS_DEAD = 2
} TASK_STATUS;

typedef struct Task
{
    size_t id;
    uint8_t status;
    cpu_registers_t* cpu_status;
    uint64_t* pageDir;

    char* taskName;
    size_t taskNameLen;

    struct Task* next;
} task_t;

extern task_t* firstTask;
extern task_t* currentTask;
extern bool taskInitialized;

void InitializeTask();
task_t* TaskGet(size_t id);
void taskName(task_t* task, char* name, size_t len);

