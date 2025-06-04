#include "ahci.h"
#include <logging.h>
#include <heap.h>
#include <maths.h>
#include <boot.h>
#include <memory.h>
#include <paging.h>
#include <spinlock.h>
#include <timer.h>
#include <console.h>

void InitializeAHCI(PCIDevice* device)
{
    debugf("[AHCI] AHCI controller detected!\n");

    PCIGeneralDevice* generalDevice = (PCIGeneralDevice*)malloc(sizeof(PCIGeneralDevice)); 
    PCI_GetGeneralDevice(device, generalDevice);

    uint32_t BAR5Base = generalDevice->bar[5] & 0xFFFFFFF0; // Align

    // Enable PCI bus mastering, memory access, (not) interrupts
    uint32_t command_status = COMBINE_WORD(device->command, device->status);
    if (!(command_status & (1 << 2)))
    {
        command_status |= (1 << 2);
    }
    if (!(command_status & (1 << 1)))
    {
        command_status |= (1 << 1);
    }
    if (command_status & (1 << 10))
    {
        command_status &= ~(1 << 10);
    }

    PCI_ConfigWriteDWord(device->bus, device->slot, device->func, PCI_COMMAND, command_status);

    paging_MapPage((void*)(uint64_t)BAR5Base, (void*)(uint64_t)BAR5Base, PF_RW);
    HBA_MEM* mem = (HBA_MEM*)((uint64_t)BAR5Base);
    ahci* ahciPtr = (ahci*)malloc(sizeof(ahci));
    memset(ahciPtr, 0, sizeof(ahci));

    ahciPtr->mem = mem;
    free(generalDevice);

    // Do a full HBA reset (Don't because it will reset (literally) everything)
    // mem->ghc |= (1 << 0);
    // while (mem->ghc & (1 << 0));

    // BIOS handoff if needed
    if (!(mem->bohc & 2) || mem->cap2 & AHCI_BIOS_OWNED)
    {
        debugf("[AHCI] BIOS Handoff required!\n");
        mem->bohc = (mem->bohc & ~8) | 2;
        while (mem->bohc & 1);

        if (mem->bohc & 1)
        {
            // Forcibly BIOS handoff
            mem->bohc = 2;
            mem->bohc |= 8;
        }
    }

    // Reset AHCI controller
    if (!(mem->ghc & (1 << 31)))
    {
        mem->ghc |= (1 << 31);
    }

    AHCI_PortProbe(ahciPtr, mem);

    // Set AHCI mode (AE)
    if (!(mem->ghc & (1 << 1)))
    {
        mem->ghc |= (1 << 1);
    }

    // Test the read function here
    int pos = 0;
    while (!(ahciPtr->sata & (1 << pos)))
    {
        pos++;
    }

    // Disk reading test
    uint8_t* buffer = (uint8_t*)malloc(512);
    memset(buffer, 0, 512);

    if (!AHCI_DiskRead(ahciPtr, pos, &ahciPtr->mem->ports[pos], 0, 1, buffer))
    {
        debugf("Failed to read disk!\n");
    }

    // Verify boot sector
    if (buffer[510] == 0x55 && buffer[511] == 0xAA)
    {
        debugf("[AHCI] Reading test passed!\n");
    }
    else
    {
        debugf("[AHCI] Something is wrong with disk reading!\n");
    }
    free(buffer);

    // Disk writing test
    uint8_t* testPattern = (uint8_t*)malloc(512);
    memset(testPattern, 0, 512);

    memcpy(testPattern, "AHCI_TEST", 9);
    for (int i = 9; i < 512; i++)
    {
        testPattern[i] = i & 0xFF;
    }

    AHCI_DiskWrite(ahciPtr, pos, &ahciPtr->mem->ports[pos], 100, 1, testPattern);

    uint8_t* verifyBuffer = (uint8_t*)malloc(512);
    memset(verifyBuffer, 0, 512);

    if (!AHCI_DiskRead(ahciPtr, pos, &ahciPtr->mem->ports[pos], 100, 1, verifyBuffer))
    {
        debugf("[AHCI] Failed to read!\n");
    }

    // Verify
    if (memcmp(testPattern, verifyBuffer, 512) == 0)
    {
        debugf("[AHCI] Writing test passed!\n");
    }


    free(testPattern);
}

spinlock_t AHCI_CMD_FIND_LOCK;
int AHCI_FindCmdSlot(HBA_PORT* port)
{
    spinlockAcquire(&AHCI_CMD_FIND_LOCK);

    // If not set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    for (int i = 0; i < 32; i++)
    {
        if ((slots & 1) == 0)
        {
            spinlockRelease(&AHCI_CMD_FIND_LOCK);
            return i;
        }

        slots >>= 1;
    }

    spinlockRelease(&AHCI_CMD_FIND_LOCK);

    debugf("[AHCI] Couldn't find free slot!\n");
    return -1;
}

HBA_CMD_TBL* AHCI_SetupCMD(ahci* ahciPtr, int portNum, int slot, uint32_t sectorCount, void* buffer, bool write)
{
    // Find the cmd header
    HBA_CMD_HEADER* cmdHeader = (HBA_CMD_HEADER*)ahciPtr->clbVirt[portNum];
    cmdHeader = &cmdHeader[slot];
    memset(cmdHeader, 0, sizeof(HBA_CMD_HEADER));

    // Find the ctba
    void* ctbaVirt = ahciPtr->ctbaVirt[portNum][slot];
    void* ctbaPhysical = paging_VirtToPhysical(ctbaVirt);
    cmdHeader->ctba = (uint32_t)(uint64_t)ctbaPhysical;
    cmdHeader->ctbau = (uint32_t)((uint64_t)ctbaPhysical >> 32);
    cmdHeader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
    cmdHeader->w = (uint8_t)write;
    cmdHeader->prdtl = 1; // TODO: Actually dynamically set based on the buffer size!

    HBA_CMD_TBL* cmdTbl = (HBA_CMD_TBL*)((uint64_t)ahciPtr->ctbaVirt[portNum][slot]);
    memset(cmdTbl, 0, sizeof(HBA_CMD_TBL) + cmdHeader->prdtl * sizeof(HBA_PRDT_ENTRY));

    // Setup the buffer
    void* bufferPhysical = paging_VirtToPhysical(buffer);
    cmdTbl->prdt_entry[0].dba = (uint32_t)(uint64_t)bufferPhysical;
    cmdTbl->prdt_entry[0].dbau = (uint32_t)((uint64_t)bufferPhysical >> 32);
    cmdTbl->prdt_entry[0].dbc = (sectorCount << 9) - 1; // 512 bytes per sector
    cmdTbl->prdt_entry[0].i = 1;

    return cmdTbl;
}

bool AHCI_PortReady(HBA_PORT* port)
{
    // Wait for the port
    uint64_t start = ticks;
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)))
    {
        if (ticks >= start + 1000)
        {
            debugf("[AHCI] Port is hung!\n");
            return false;
        }
    }

    return true;
}

bool AHCI_CMDIssue(HBA_PORT* port, int slot)
{
    // Issue command
    port->ci = 1 << slot;

    // Poll until complete
    while (1)
    {
        if ((port->ci & (1 << slot)) == 0)
        {
            break;
        }

        if ((port->is & HBA_PxIS_TFES))
        {
            return false;
        }
    }

    return true;
}

bool AHCI_DiskRead(ahci* ahciPtr, int portNum, HBA_PORT* port, uint64_t sector, uint32_t sectorCount, void* buffer)
{
    uint32_t sectorLow = (uint32_t)sector;
    uint32_t sectorHigh = (uint32_t)(sector >> 32);

    port->is = (uint32_t)-1; // Clear pending interrupt bits
    int slot = AHCI_FindCmdSlot(port);
    if (slot == -1)
    {
        return false;
    }

    HBA_CMD_TBL* cmdTbl = AHCI_SetupCMD(ahciPtr, portNum, slot, sectorCount, buffer, false);

    // Setup the FIS
    FIS_REG_H2D* cmdFis = (FIS_REG_H2D*)(&cmdTbl->cfis);
    memset(cmdFis, 0, sizeof(FIS_REG_H2D));
    cmdFis->fis_type = FIS_TYPE_REG_H2D;
    cmdFis->c = 1; // Command
    cmdFis->command = ATA_CMD_READ_DMA_EX;

    // Setup the sector
    cmdFis->lba0 = (uint8_t)sectorLow;
    cmdFis->lba1 = (uint8_t)(sectorLow >> 8);
    cmdFis->lba2 = (uint8_t)(sectorLow >> 16);
    
    cmdFis->lba3 = (uint8_t)(sectorLow >> 24);
    cmdFis->lba4 = (uint8_t)sectorHigh;
    cmdFis->lba5 = (uint8_t)(sectorHigh >> 8);

    cmdFis->device = 1 << 6; // LBA mode

    // Setup the sector count
    cmdFis->countl = sectorCount & 0xFF;
    cmdFis->counth = (sectorCount >> 8) & 0xFF;

    if (!AHCI_PortReady(port))
    {
        return false;
    }

    return AHCI_CMDIssue(port, slot);
}

bool AHCI_DiskWrite(ahci* ahciPtr, int portNum, HBA_PORT* port, uint64_t sector, uint32_t sectorCount, void* buffer)
{
    uint32_t sectorLow = (uint32_t)sector;
    uint32_t sectorHigh = (uint32_t)(sector >> 32);

    port->is = (uint32_t)-1;
    int slot = AHCI_FindCmdSlot(port);
    if (slot == -1)
    {
        return false;
    }

    HBA_CMD_TBL* cmdTbl = AHCI_SetupCMD(ahciPtr, portNum, slot, sectorCount, buffer, true);

    // Setup the FIS
    FIS_REG_H2D* cmdFis = (FIS_REG_H2D*)(&cmdTbl->cfis);
    memset(cmdFis, 0, sizeof(FIS_REG_H2D));
    cmdFis->fis_type = FIS_TYPE_REG_H2D;
    cmdFis->c = 1; // Command
    cmdFis->command = ATA_CMD_WRITE_DMA_EX;

    // Setup the sector
    cmdFis->lba0 = (uint8_t)sectorLow;
    cmdFis->lba1 = (uint8_t)(sectorLow >> 8);
    cmdFis->lba2 = (uint8_t)(sectorLow >> 16);
    
    cmdFis->lba3 = (uint8_t)(sectorLow >> 24);
    cmdFis->lba4 = (uint8_t)sectorHigh;
    cmdFis->lba5 = (uint8_t)(sectorHigh >> 8);
    
    cmdFis->device = 1 << 6; // LBA mode
    
    // Setup the sector count
    cmdFis->countl = sectorCount & 0xFF;
    cmdFis->counth = (sectorCount >> 8) & 0xFF;

    if (!AHCI_PortReady(port))
    {
        return false;
    }

    return AHCI_CMDIssue(port, slot);
}

