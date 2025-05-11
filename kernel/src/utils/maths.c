#include "maths.h"

size_t align(size_t size, uint64_t alignment)
{
    size_t newSize = size;

    if (newSize % alignment > 0)
    {
        newSize -= (newSize % alignment);
        newSize += alignment;
    }

    return newSize;
}

