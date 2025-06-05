#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t partitionOffset;
    uint32_t partitionSize; // Size in sectors
} Partition;

void MBR_DetectPartition(Partition* part, void* partPtr);
void MBR_ReadSectors(Partition* part, uint64_t sector, uint32_t sectorsCount, void* buffer);

