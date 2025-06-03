#include "ahci.h"
#include <logging.h>
#include <memory.h>
#include <vmm.h>
#include <paging.h>

uint8_t AHCI_CheckPortType(HBA_PORT* port)
{
    uint32_t sataStatus = port->ssts;
    
    uint8_t ipm = (sataStatus >> 8) & 0x0F;
    uint8_t det = sataStatus & 0x0F;
    
    if (det != HBA_PORT_DEV_PRESENT)
    {
        return AHCI_PORT_NONE;
    }
    if (ipm != HBA_PORT_IPM_ACTIVE)
    {
        return AHCI_PORT_NONE;
    }

    switch (port->sig)
    {
        case SATA_SIG_ATA:
            return AHCI_PORT_SATA;
        case SATA_SIG_ATAPI:
            return AHCI_PORT_SATAPI;
        case SATA_SIG_PM:
            return AHCI_PORT_PM;
        case SATA_SIG_SEMB:
            return AHCI_PORT_SEMB;
        default:
            return AHCI_PORT_NONE;
    }
}

void AHCI_PortProbe(ahci* ahciPtr, HBA_MEM* mem)
{
    uint32_t portsImplemented = mem->pi;

    for (int i = 0; i < 32; i++)
    {
        if (portsImplemented & (1 << i))
        {
            uint8_t portType = AHCI_CheckPortType(&mem->ports[i]);

            if (portType == AHCI_PORT_SATA)
            {
                debugf("[AHCI] Drive: SATA drive detected!\n");
                AHCI_PortRebase(ahciPtr, &mem->ports[i], i);
            }
            else if (portType == AHCI_PORT_SATAPI)
            {
                debugf("[AHCI] (Unsupported) Drive: SATAPI drive detected!\n");
            }
            else if (portType == AHCI_PORT_PM)
            {
                debugf("[AHCI] (Unsupported) Drive: PM drive detected!\n");
            }
            else if (portType == AHCI_PORT_SEMB)
            {
                debugf("[AHCI] (Unsupported) Drive: SEMB drive detected!\n");
            }
            // Otherwise, it is probably an invalid drive
        }
    }
}

void AHCI_PortRebase(ahci* ahciPtr, HBA_PORT* port, int portNum)
{
    AHCI_StopCommand(port);

    // Quite a waste of memory....
    void* clbVirt = vmm_AllocatePage();
    void* clbPhysical = paging_VirtToPhysical(clbVirt);
    memset(clbVirt, 0, 1024);

    port->clb = (uint32_t)(uint64_t)clbPhysical;
    port->clbu = (uint32_t)((uint64_t)clbPhysical >> 32);
    ahciPtr->clbVirt[portNum] = clbVirt;

    void* fisVirt = vmm_AllocatePage();
    void* fisPhysical = paging_VirtToPhysical(fisVirt);
    memset(fisVirt, 0, 256);

    port->fb = (uint32_t)(uint64_t)fisPhysical;
    port->fbu = (uint32_t)((uint64_t)fisPhysical >> 32);

    HBA_CMD_HEADER* cmdHeader = (HBA_CMD_HEADER*)clbVirt;

    for (int i = 0; i < 32; i++)
    {
        cmdHeader->prdtl = 8;

        void* cmdTableVirt = vmm_AllocatePage();
        void* cmdTablePhysical = paging_VirtToPhysical(cmdTablePhysical);

        uint64_t addr = (uint64_t)cmdTablePhysical + (i << 8);

        cmdHeader[i].ctba = (uint32_t)addr;
        cmdHeader[i].ctbau = (uint32_t)(addr >> 32);
        memset(cmdTableVirt, 0, 256);

        ahciPtr->ctbaVirt[portNum][i] = cmdTableVirt;
    }

    // Clear interface fatal error
    if (port->serr & (1 << 10))
    {
        port->serr |= (1 << 10);
    }

    port->serr = port->serr; // Effectively clearing all bits (that were set)

    AHCI_StartCommand(port);
}

