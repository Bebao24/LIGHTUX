#pragma once
#include <stddef.h>

// Really basic linked list implementation

typedef struct LinkedListHdr
{
    struct LinkedListHdr* next;
} LinkedListHdr;

void* LinkedListAllocate(void** LinkedListPtr, size_t size);

