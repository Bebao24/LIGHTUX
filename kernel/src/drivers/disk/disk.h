#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <mbr.h>

#define SECTOR_SIZE 512

void InitializeDisk(Partition* partitionOut);

bool diskBytes(uint64_t sector, uint32_t sectorCount, void* buffer, bool write);
bool diskRead(uint64_t sector, uint32_t sectorCount, void* buffer);
bool diskWrite(uint64_t sector, uint32_t sectorCount, void* buffer);



