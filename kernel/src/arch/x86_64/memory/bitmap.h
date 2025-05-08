#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
    size_t bitmapSize;
    uint8_t* buffer;

    // Optimization
    uint64_t lastDeepFragmented;
} bitmap_t;

void InitializeBitmap(bitmap_t* bitmap, size_t bitmapSize, void* buffer);

bool bitmap_Get(bitmap_t* bitmap, uint64_t index);
void bitmap_Set(bitmap_t* bitmap, uint64_t index, bool enabled);
