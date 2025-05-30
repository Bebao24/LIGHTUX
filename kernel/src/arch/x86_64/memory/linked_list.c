#include "linked_list.h"
#include <heap.h>
#include <memory.h>

void* LinkedListAllocate(void** LinkedListPtr, size_t size)
{
    LinkedListHdr* target = (LinkedListHdr*)malloc(sizeof(size));
    memset(target, 0, size);

    LinkedListHdr* current = (LinkedListHdr*)(*LinkedListPtr);

    while (true)
    {
        if (current == NULL)
        {
            // That mean this is the first one
            *LinkedListPtr = target;
            break;
        }

        if (current->next == NULL)
        {
            // End of linked list
            current->next = target;
            break;
        }

        current = current->next;
    }

    target->next = NULL;
    return target;
}


