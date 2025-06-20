#pragma once
#include <stdint.h>
#include <task.h>

// 16 KB
#define DEFAULT_STACK_SIZE_IN_PAGES 4
#define USER_STACK_TOP 0x600000000000

void* StackAllocate();
void StackGenerateUser(task_t* task);

