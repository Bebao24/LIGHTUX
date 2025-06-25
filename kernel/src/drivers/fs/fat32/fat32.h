#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
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
    uint8_t Order;
    uint16_t Chars1[5]; // UTF-16
    uint8_t Attributes;
    uint8_t Type;
    uint8_t Checksum;
    uint16_t Chars2[6];
    uint16_t Zero;
    uint16_t Chars3[2];
} __attribute__((packed)) FAT32_LFNEntry;

typedef struct
{
    uint32_t firstCluster;
    uint32_t size;
    uint32_t currentCluster;
    uint32_t currentOffset;
    bool isDirectory;
} FAT32_FileHandle;

#define FAT32_LFN_MAXCHARS 256 // Including NULL termination
#define FAT32_ORDER_FINAL 0x40
#define FAT32_LFN_MAX_INDEX 20

bool FAT32_Initialize(Partition* partition);
FAT32_FileHandle* FAT32_Open(const char* path);

bool FAT32_ListDirectory(const char* path);
bool FAT32_TraversePath(const char* path, FAT32_DirectoryEntry* entryOut);
uint32_t FAT32_Read(FAT32_FileHandle* handle, void* buffer, uint32_t bytesCount);
size_t FAT32_Seek(FAT32_FileHandle* handle, uint32_t offset);

void FAT32_Close(FAT32_FileHandle* handle);
