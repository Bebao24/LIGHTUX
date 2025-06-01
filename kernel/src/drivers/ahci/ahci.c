#include "ahci.h"
#include <logging.h>
#include <heap.h>
#include <maths.h>
#include <boot.h>
#include <memory.h>
#include <paging.h>

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

    AHCI_PortProbe(mem);

    // Set AHCI mode (AE)
    if (!(mem->ghc & (1 << 1)))
    {
        mem->ghc |= (1 << 1);
    }
}

