#pragma once
#include <stdbool.h>

typedef struct
{
    bool locked;
} spinlock_t;

void spinlockAcquire(spinlock_t* lock);
void spinlockRelease(spinlock_t* lock);

