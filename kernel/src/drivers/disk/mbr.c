#include "mbr.h"
#include <disk.h>

typedef struct
{
    uint8_t attributes;
    uint8_t chsStart[3];
    uint8_t partitionType;
    uint8_t chsEnd[3];
    uint32_t lbaStart;
    uint32_t sectorsCount;
} __attribute__((packed)) MBR_Entry;

void MBR_DetectPartition(Partition* part, void* partPtr)
{
    MBR_Entry* entry = (MBR_Entry*)partPtr;
    part->partitionOffset = entry->lbaStart;
    part->partitionSize = entry->sectorsCount;
}

bool MBR_ReadSectors(Partition* part, uint64_t sector, uint32_t sectorsCount, void* buffer)
{
    return diskRead(sector + part->partitionOffset, sectorsCount, buffer);
}

