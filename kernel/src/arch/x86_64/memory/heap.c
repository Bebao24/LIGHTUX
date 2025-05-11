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

void SplitNode(HeapNode_t* node, size_t splitSize)
{
    if (splitSize < HEAP_ALIGNMENT)
    {
        return;
    }

    HeapNode_t* newNode = (HeapNode_t*)((void*)node + splitSize + sizeof(HeapNode_t));

    newNode->size = node->size - splitSize - sizeof(HeapNode_t);
    newNode->free = true;
    newNode->next = node->next;

    node->size = splitSize;
    node->free = false;
    node->next = newNode;

    if (lastNode == node)
    {
        // Assign it to "newNode" since it is next to "node"
        lastNode = newNode;
    }
}

void ExpandHeap(size_t size)
{
    // Align it to pagess
    size = align(size, PAGE_SIZE);
    debugf("[HEAP] Expanding heap with size: 0x%llx\n", size);

    size_t numPages = DivRoundUp(size, PAGE_SIZE);
    HeapNode_t* newNode = (HeapNode_t*)vmm_AllocatePages(numPages);

    debugf("[HEAP] New heap address: 0x%llx\n", (uint64_t)newNode);

    newNode->next = NULL;
    newNode->last = lastNode;
    newNode->size = numPages * PAGE_SIZE;
    newNode->free = true;

    lastNode->next = newNode;
    lastNode = newNode;
}

void CombineForward(HeapNode_t* node)
{
    // Check for the next node
    if (node->next == NULL)
    {
        return;
    }

    if (!node->next->free)
    {
        return;
    }

    if (node->next == lastNode)
    {
        lastNode = node;
    }

    if (node->next->next != NULL)
    {
        node->next->next->last = node;
    }

    node->size = node->size + node->next->size + sizeof(HeapNode_t);
    node->next = node->next->next;
}

void CombineBackward(HeapNode_t* node)
{
    if (node->last != NULL && node->last->free)
    {
        CombineBackward(node->last);
    }
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
                    // Split the node
                    SplitNode(currentNode, size);
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

        if (currentNode == lastNode)
        {
            // Expand the heap
            ExpandHeap(size);
            if (currentNode->last != NULL)
            {
                currentNode = currentNode->last;
            }
        }

        currentNode = currentNode->next;
    }

    // Should never get here
    return NULL;
}

void free(void* ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    HeapNode_t* node = (HeapNode_t*)ptr - 1;
    node->free = true;
    CombineForward(node);
    CombineBackward(node);
}

