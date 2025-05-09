#include "pmm.h"
#include <stdint.h>
#include <boot.h>
#include <limine.h>
#include <system.h>
#include <bitmap.h>

/* These operations shouldn't be used outside of initializing the pmm */
void pmm_ReservePage(void* addr);
void pmm_UnreservePage(void* addr);

void pmm_ReservePages(void* addr, size_t numPages);
void pmm_UnreservePages(void* addr, size_t numPages);

size_t freeMemory;
size_t usedMemory;
size_t reservedMemory;

bitmap_t g_PhysicalBitmap;

void InitializePMM()
{
    void* largestMemSeg = NULL;
    size_t largestMemSegSize = 0;

    // Find the largest memory segment
    for (uint64_t i = 0; i < bootInfo.mmNumEntries; i++)
    {
        struct limine_memmap_entry* entry = bootInfo.mmEntries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            if (entry->length > largestMemSegSize)
            {
                largestMemSeg = (void*)entry->base;
                largestMemSegSize = entry->length;
            }
        }
    }

    // Found nothing?
    if (largestMemSeg == NULL)
    {
        panic("[PMM] Can't find a free region for initializing!\n");
    }

    // Setting up the bitmap
    freeMemory = bootInfo.mmTotal;
    uint64_t bitmapSize = bootInfo.mmTotal / PAGE_SIZE / 8 + 1;
    InitializeBitmap(&g_PhysicalBitmap, bitmapSize, largestMemSeg);

    // Starting to reserve pages
    pmm_ReservePages(0, bootInfo.mmTotal / PAGE_SIZE + 1);
    for (size_t i = 0; i < bootInfo.mmNumEntries; i++)
    {
        struct limine_memmap_entry* entry = bootInfo.mmEntries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            // Shrink down the free regions for safety ensure
            pmm_UnreservePages((void*)entry->base, entry->length / PAGE_SIZE);
        }
    }

    pmm_ReservePages(0, 0x10); // Reserve the important pages
    pmm_LockPages(largestMemSeg, g_PhysicalBitmap.bitmapSize / PAGE_SIZE + 1);
}

void pmm_LockPage(void* addr)
{
    uint64_t index = (uint64_t)addr / PAGE_SIZE;
    if (bitmap_Get(&g_PhysicalBitmap, index))
    {
        // Already locked (or occupied)
        return;
    }

    bitmap_Set(&g_PhysicalBitmap, index, true);
    freeMemory -= PAGE_SIZE;
    usedMemory += PAGE_SIZE;
}

void pmm_FreePage(void* addr)
{
    uint64_t index = (uint64_t)addr / PAGE_SIZE;
    if (!bitmap_Get(&g_PhysicalBitmap, index))
    {
        // Already freed
        return;
    }

    bitmap_Set(&g_PhysicalBitmap, index, false);
    freeMemory += PAGE_SIZE;
    usedMemory -= PAGE_SIZE;

    if (g_PhysicalBitmap.lastDeepFragmented > index)
    {
        g_PhysicalBitmap.lastDeepFragmented = index;
    }
}

void pmm_ReservePage(void* addr)
{
    uint64_t index = (uint64_t)addr / PAGE_SIZE;
    if (bitmap_Get(&g_PhysicalBitmap, index))
    {
        // Already reserved
        return;
    }

    bitmap_Set(&g_PhysicalBitmap, index, true);
    freeMemory -= PAGE_SIZE;
    reservedMemory += PAGE_SIZE;
}

void pmm_UnreservePage(void* addr)
{
    uint64_t index = (uint64_t)addr / PAGE_SIZE;
    if (!bitmap_Get(&g_PhysicalBitmap, index))
    {
        // Already unreserved
        return;
    }

    bitmap_Set(&g_PhysicalBitmap, index, false);
    freeMemory += PAGE_SIZE;
    reservedMemory -= PAGE_SIZE;

    if (g_PhysicalBitmap.lastDeepFragmented > index)
    {
        g_PhysicalBitmap.lastDeepFragmented = index;
    }
}

void pmm_LockPages(void* addr, size_t numPages)
{
    for (size_t i = 0; i < numPages; i++)
    {
        pmm_LockPage((void*)((uint64_t)addr + (i * PAGE_SIZE)));
    }
}

void pmm_FreePages(void* addr, size_t numPages)
{
    for (size_t i = 0; i < numPages; i++)
    {
        pmm_FreePage((void*)((uint64_t)addr + (i * PAGE_SIZE)));
    }
}

void pmm_ReservePages(void* addr, size_t numPages)
{
    for (size_t i = 0; i < numPages; i++)
    {
        pmm_ReservePage((void*)((uint64_t)addr + (i * PAGE_SIZE)));
    }
}

void pmm_UnreservePages(void* addr, size_t numPages)
{
    for (size_t i = 0; i < numPages; i++)
    {
        pmm_UnreservePage((void*)((uint64_t)addr + (i * PAGE_SIZE)));
    }
}

uint64_t pmm_FindFreeRegion(size_t numPages)
{
    uint64_t currentRegionStart = g_PhysicalBitmap.lastDeepFragmented;
    size_t currentRegionSize = 0;

    for (uint64_t i = currentRegionStart; i < g_PhysicalBitmap.bitmapSize * 8; i++)
    {
        if (bitmap_Get(&g_PhysicalBitmap, i))
        {
            // Already occupied
            currentRegionSize = 0;
            currentRegionStart = i + 1;
        }
        else
        {
            if (numPages == 1)
            {
                // Optimization
                g_PhysicalBitmap.lastDeepFragmented = currentRegionStart + 1;
            }

            currentRegionSize++;
            if (currentRegionSize >= numPages)
            {
                return currentRegionStart;
            }
        }
    }

    // Will be handled by the pmm_AllocatePage()
    return INVALID_PAGE;
}

void* pmm_AllocatePage()
{
    uint64_t region = pmm_FindFreeRegion(1);
    if (region == INVALID_PAGE)
    {
        panic("[PMM] Physical memory ran out!\n");
        return NULL;
    }

    pmm_LockPage((void*)(region * PAGE_SIZE));

    return (void*)(region * PAGE_SIZE);
}

void* pmm_AllocatePages(size_t numPages)
{
    if (!numPages)
    {
        return NULL;
    }

    uint64_t region = pmm_FindFreeRegion(numPages);
    if (region == INVALID_PAGE)
    {
        panic("[PMM] Physical memory ran out!\n");
        return NULL;
    }

    pmm_LockPages((void*)(region * PAGE_SIZE), numPages);
    
    return (void*)(region * PAGE_SIZE);
}

size_t pmm_GetFreeMem()
{
    return freeMemory;
}

size_t pmm_GetUsedMem()
{
    return usedMemory;
}

size_t pmm_GetReservedMem()
{
    return reservedMemory;
}

