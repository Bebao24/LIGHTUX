#include "fat32.h"
#include <disk.h>
#include <logging.h>
#include <console.h>
#include <string.h>
#include <memory.h>
#include <ctype.h>

#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 256

typedef struct
{
    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];
} __attribute__((packed)) FAT_EBR;

typedef struct
{
    uint32_t SectorsPerFat;
    uint16_t Flags;
    uint16_t FatVersion;
    uint32_t RootDirectoryCluster;
    uint16_t FSInfoSector;
    uint16_t BackupBootSector;
    uint8_t Reserved[12];
} __attribute__((packed)) FAT32_EBR;

typedef struct
{
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    // FAT32 EBR
    FAT32_EBR EBR32;

    // FAT EBR
    FAT_EBR EBR;
} __attribute__((packed)) FAT_BootSector;

// This struct will contain all the FAT data
typedef struct
{
    // We will use a union since our read function can only read full sector
    union
    {
        FAT_BootSector BS;
        uint8_t BSBytes[SECTOR_SIZE];
    } BootSector;

    uint32_t fatStartLba;
    uint32_t clusterStartLba;
} FAT32_Data;

static FAT32_Data g_FatData;

bool FAT32_ReadBootSector(Partition* partition)
{
    return MBR_ReadSectors(partition, 0, 1, g_FatData.BootSector.BSBytes);
}

bool FAT32_Initialize(Partition* partition)
{
    // Read the boot sector
    if (!FAT32_ReadBootSector(partition))
    {
        debugf("[FAT32] Failed to read boot sector!\n");
        return false;
    }

    // Calculate where the FAT and cluster begin
    g_FatData.fatStartLba = partition->partitionOffset + g_FatData.BootSector.BS.ReservedSectors;
    g_FatData.clusterStartLba = partition->partitionOffset + g_FatData.BootSector.BS.ReservedSectors +
                                 (g_FatData.BootSector.BS.EBR32.SectorsPerFat * g_FatData.BootSector.BS.FatCount);

    debugf("[FAT32] FAT start LBA: %d\n", g_FatData.fatStartLba);
    debugf("[FAT32] Cluster start LBA: %d\n", g_FatData.clusterStartLba);

    return true;
}

uint32_t FAT32_ClusterToLba(uint32_t cluster)
{
    return g_FatData.clusterStartLba + (cluster - 2) * g_FatData.BootSector.BS.SectorsPerCluster;
}

void FAT32_ShortNameToName(uint8_t* shortName, char* out)
{
    int out_i = 0;
    
    // Copy the name before the extension character
    for (int i = 0; i < 8 && shortName[i] != ' '; i++)
    {
        out[out_i++] = shortName[i];
    }

    // Copy the extension character and the extension after it
    if (shortName[8] != ' ')
    {
        out[out_i++] = '.';
        for (int i = 8; i < 11 && shortName[i] != ' '; i++)
        {
            out[out_i++] = shortName[i];
        }
    }

    // NULL termination
    out[out_i] = '\0';
}

uint32_t FAT32_NextCluster(uint32_t cluster)
{
    uint32_t fatIndex = cluster * 4;
    uint32_t lba = g_FatData.fatStartLba + (fatIndex / SECTOR_SIZE);
    uint8_t buffer[SECTOR_SIZE];
    if (!diskRead(lba, 1, buffer))
    {
        debugf("[FAT32] Read error!\n");
        return 0xFFFFFFFF;
    }

    return (*(uint32_t*)&buffer[fatIndex % SECTOR_SIZE]) & 0x0FFFFFFF;
}

bool FAT32_ListDirectoryEntry(uint32_t startCluster)
{
    uint8_t buffer[SECTOR_SIZE];
    uint32_t currentCluster = startCluster;

    while (currentCluster < 0x0FFFFFF8)
    {
        for (uint8_t i = 0; i < g_FatData.BootSector.BS.SectorsPerCluster; i++)
        {
            uint32_t lba = FAT32_ClusterToLba(currentCluster);
            if (!diskRead(lba + i, 1, buffer))
            {
                debugf("[FAT32] Read error!\n");
                return false;
            }

            FAT32_DirectoryEntry* entries = (FAT32_DirectoryEntry*)buffer;

            for (int j = 0; j < SECTOR_SIZE / sizeof(FAT32_DirectoryEntry); j++)
            {
                if (entries[j].Name[0] == 0x00)
                {
                    // No more entries
                    continue;
                }
                if (entries[j].Attributes == 0x0F ||
                    entries[j].Name[0] == 0xE5 ||
                    entries[j].Attributes == 0x08)
                {
                    // Skip LFN, no more entries, deleted entry, volume label
                    continue;
                }

                char name[MAX_NAME_LEN];
                FAT32_ShortNameToName(entries[j].Name, name);

                printf("%s\n", name);
            }
        }

        currentCluster = FAT32_NextCluster(currentCluster);
    }

    return true;
}

bool FAT32_FindDirectoryEntry(uint32_t startCluster, char* name, FAT32_DirectoryEntry* entryOut)
{
    uint8_t buffer[SECTOR_SIZE];
    uint32_t currentCluster = startCluster;

    while (currentCluster < 0x0FFFFFF8)
    {
        for (int i = 0; i < g_FatData.BootSector.BS.SectorsPerCluster; i++)
        {
            uint32_t lba = FAT32_ClusterToLba(currentCluster);

            if (!diskRead(lba + i, 1, buffer))
            {
                debugf("[FAT32] Read error!\n");
            }
            FAT32_DirectoryEntry* entries = (FAT32_DirectoryEntry*)buffer;

            for (int j = 0; j < SECTOR_SIZE / sizeof(FAT32_DirectoryEntry); j++)
            {
                if (entries[j].Name[0] == 0x00)
                {
                    // No more entries
                    continue;
                }
                if (entries[j].Attributes == 0x0F ||
                    entries[j].Name[0] == 0xE5 ||
                    entries[j].Attributes == 0x08)
                {
                    // Skip LFN, no more entries, deleted entry, volume label
                    continue;
                }
                if (!(entries[j].Attributes & 0x10))
                {
                    // Not a directory
                    continue;
                }

                char entryName[MAX_NAME_LEN];
                for (int i = 0; i < strlen(name); i++)
                {
                    name[i] = toupper(name[i]);
                }


                FAT32_ShortNameToName(entries[j].Name, entryName);

                if (strcmp(entryName, name) == 0)
                {
                    *entryOut = entries[j];
                    return true;
                }
            }
        }

        currentCluster = FAT32_NextCluster(currentCluster);
    }

    return false; // Not found
}

bool FAT32_ListDirectory(const char* path)
{
    uint32_t currentCluster = g_FatData.BootSector.BS.EBR32.RootDirectoryCluster;
    FAT32_DirectoryEntry entry;

    if (!path || path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))
    {
        // Just return the root directory
        return FAT32_ListDirectoryEntry(currentCluster);
    }

    // Ignore leading slash
    if (path[0] == '/')
    {
        path++;
    }

    char name[MAX_PATH_LEN];
    while (*path)
    {
        bool isLast = false;
        const char* delim = strchr(path, '/');
        if (delim != NULL)
        {
            memcpy(name, path, delim - path);
            name[delim - path] = '\0';
            path = delim + 1;
        }
        else
        {
            size_t len = strlen(path);
            memcpy(name, path, len);
            name[len] = '\0';
            path += len;
            isLast = true;
        }

        if (!FAT32_FindDirectoryEntry(currentCluster, name, &entry))
        {
            debugf("[FAT32] Directory entry not found!\n");
            return false;
        }

        // Update the currentCluster
        currentCluster = (entry.FirstClusterHigh << 16) | entry.FirstClusterLow;
    }

    return FAT32_ListDirectoryEntry(currentCluster);
}

