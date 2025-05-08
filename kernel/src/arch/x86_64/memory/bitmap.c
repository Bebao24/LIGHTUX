#include "bitmap.h"
#include <boot.h>
#include <memory.h>

void InitializeBitmap(bitmap_t* bitmap, size_t bitmapSize, void* buffer)
{
    bitmap->bitmapSize = bitmapSize;

    // Direct mapping using the hhdm
    bitmap->buffer = (uint8_t*)(buffer);
    bitmap->lastDeepFragmented = 0;

    memset(bitmap->buffer, 0, bitmapSize);
}

bool bitmap_Get(bitmap_t* bitmap, uint64_t index)
{
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8; // Calculate the remainder
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    if ((bitmap->buffer[byteIndex] & bitIndexer) > 0)
    {
        return true;
    }

    return false;
}

void bitmap_Set(bitmap_t* bitmap, uint64_t index, bool enabled)
{
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8; // Calculate the remainder
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    // Set the bit to 0
    bitmap->buffer[byteIndex] &= ~(bitIndexer);

    if (enabled)
    {
        // Set the bit to 1
        bitmap->buffer[byteIndex] |= bitIndexer;
    }
}

