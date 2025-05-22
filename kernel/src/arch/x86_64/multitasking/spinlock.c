#include "spinlock.h"

void spinlockAcquire(spinlock_t* lock)
{
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE));
}

void spinlockRelease(spinlock_t* lock)
{
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

