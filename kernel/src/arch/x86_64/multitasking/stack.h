#pragma once
#include <stdint.h>
#include <task.h>

// 16 KB
#define DEFAULT_STACK_SIZE_IN_PAGES 4
#define USER_STACK_TOP 0x00007FFFFFFFE000

// a is stack ptr
// b is type size
// c is push value
#define PUSH_TO_STACK(a, b, c)                                                 \
  a -= sizeof(b);                                                              \
  *((b *)(a)) = c

void* StackAllocate();
void StackGenerateUser(task_t* task);

