#include "disk.h"
#include <pci.h>
#include <ahci.h>
#include <logging.h>
#include <memory.h>

void InitializeDisk(Partition* partitionOut)
{
    // Detect our MBR disk partition
	uint8_t mbrBytes[512];
	diskRead(0, 1, mbrBytes);

	void* partPtr = &mbrBytes[446]; // First partition entry
	MBR_DetectPartition(partitionOut, partPtr);
}

void diskBytes(uint64_t sector, uint32_t sectorCount, void* buffer, bool write)
{
    PCI* browse = firstPCI;
    while (browse)
    {
        if (browse->driverType == PCI_DRIVER_AHCI)
        {
            break;
        }

        browse = browse->next;
    }

    if (!browse)
    {
        // We can't find the AHCI device
        debugf("[DISK] Can't find AHCI device!\n");
        memset(buffer, 0, sectorCount * SECTOR_SIZE);
        return;
    }

    ahci* ahciPtr = (ahci*)browse->devicePtr;
    int pos = 0;
    while (!(ahciPtr->sata & (1 << pos)))
    {
        pos++;
    }

    if (write)
    {
        AHCI_DiskWrite(ahciPtr, pos, &ahciPtr->mem->ports[pos], sector, sectorCount, buffer);
    }
    else
    {
        AHCI_DiskRead(ahciPtr, pos, &ahciPtr->mem->ports[pos], sector, sectorCount, buffer);
    }
}

void diskRead(uint64_t sector, uint32_t sectorCount, void* buffer)
{
    diskBytes(sector, sectorCount, buffer, false);
}

void diskWrite(uint64_t sector, uint32_t sectorCount, void* buffer)
{
    diskBytes(sector, sectorCount, buffer, true);
}


