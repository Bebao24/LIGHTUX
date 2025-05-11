#include "heap.h"
#include <vmm.h>
#include <pmm.h>
#include <logging.h>
#include <stdint.h>
#include <maths.h>

void* heapStart;
HeapNode_t* lastNode;
void* heapEnd;

void InitializeHeap(size_t numPages)
{
    void* heapStartAddr = vmm_AllocatePages(numPages);
    debugf("[HEAP] Starting address: 0x%llx, heap size: 0x%x pages\n", heapStartAddr, numPages);

    size_t heapSize = numPages * PAGE_SIZE;

    heapStart = heapStartAddr;
    heapEnd = (void*)((uint64_t)heapStart + heapSize);

    // The last node is the final node at the end of the heap
    // Currently, we only have only 1 node
    lastNode = (HeapNode_t*)heapStart;
    lastNode->size = heapSize - sizeof(HeapNode_t);
    lastNode->next = NULL;
    lastNode->last = NULL;
    lastNode->free = true;
}

void* malloc(size_t size)
{
    HeapNode_t* currentNode = (HeapNode_t*)heapStart;

    if (size == 0)
    {
        debugf("[HEAP] Can't allocate with size 0!\n");
        return NULL;
    }

    // Align the size
    size = align(size, HEAP_ALIGNMENT);

    while (currentNode)
    {
        if (currentNode->free)
        {
            if (currentNode->size >= size)
            {
                // Found a free node that is free and large enough
                if (currentNode->size > size)
                {
                    // TODO: Split the node
                    currentNode->free = false;
                    currentNode->size = size;
                }
                else
                {
                    currentNode->free = false;
                }

                return (void*)currentNode + sizeof(HeapNode_t);
            }
        }

        // TODO: Check for heap overflow and expand heap

        currentNode = currentNode->next;
    }

    return NULL;
}

