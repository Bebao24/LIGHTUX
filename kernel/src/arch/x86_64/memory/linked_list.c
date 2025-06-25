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

// This is a helper of LinkedListRemove()
// Remove the target from the linked list
bool LinkedListUnregister(void** LinkedListFirst, void* target)
{
    LinkedListHdr* LinkedListFirstClone = *LinkedListFirst;

    LinkedListHdr* current = (LinkedListHdr*)(*LinkedListFirst);
    while (current)
    {
        if (current->next && current->next == target)
        {
            break;
        }

        current = current->next;
    }

    if (LinkedListFirstClone == target)
    {
        // The first linked list ptr is the target
        // Point the first ptr to the next element
        *LinkedListFirst = LinkedListFirstClone->next;
        return true;
    }
    else if (!current)
    {
        return false;
    }

    // The target ptr is in the current->next
    // So we will point the current->next to current->next->next, delete the current->next
    LinkedListHdr* LinkedListTarget = current->next;
    current->next = LinkedListTarget->next;

    return true;
}

// Remove from the linked list and free() the target
bool LinkedListRemove(void** LinkedListFirst, void* target)
{
    bool success = LinkedListUnregister(LinkedListFirst, target);
    free(target);
    return success;
}


