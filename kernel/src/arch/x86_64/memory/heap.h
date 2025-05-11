#pragma once
#include <stddef.h>
#include <stdbool.h>

#define HEAP_SIZE_IN_PAGES (0x20)
#define HEAP_ALIGNMENT (0x10)

typedef struct HeapNode
{
    size_t size;
    bool free;
    struct HeapNode* next;
    struct HeapNode* last;
} HeapNode_t;

void InitializeHeap(size_t numPages);

void* malloc(size_t size);
void free(void* ptr);


