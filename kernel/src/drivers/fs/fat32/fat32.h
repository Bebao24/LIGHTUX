#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <mbr.h>

typedef struct
{
    uint8_t Name[11]; // 8.3 name
    uint8_t Attributes;
    uint8_t Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} __attribute__((packed)) FAT32_DirectoryEntry;

typedef struct
{
    uint32_t size;
    uint32_t currentCluster;
    uint32_t currentOffset;
    bool isDirectory;
} FAT32_FileHandle;

bool FAT32_Initialize(Partition* partition);
FAT32_FileHandle* FAT32_Open(const char* path);

bool FAT32_ListDirectory(const char* path);
bool FAT32_TraversePath(const char* path, FAT32_DirectoryEntry* entryOut);
uint32_t FAT32_Read(FAT32_FileHandle* handle, void* buffer, uint32_t bytesCount);

void FAT32_Close(FAT32_FileHandle* handle);
