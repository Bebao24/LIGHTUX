#include "ahci.h"
#include <logging.h>
#include <memory.h>
#include <vmm.h>

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
                AHCI_PortRebase(ahciPtr, &mem->ports[i]);
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

void AHCI_PortRebase(ahci* ahciPtr, HBA_PORT* port)
{
    AHCI_StopCommand(port);

    // Quite a waste of memory....
    void* clbBase = vmm_AllocatePage();
    port->clb = (uint32_t)(uint64_t)clbBase;
    port->clbu = (uint32_t)((uint64_t)clbBase >> 32);
    memset(clbBase, 0, 1024);

    void* fisBase = vmm_AllocatePage();
    port->fb = (uint32_t)(uint64_t)fisBase;
    port->fbu = (uint32_t)((uint64_t)fisBase >> 32);
    memset(fisBase, 0, 256);

    HBA_CMD_HEADER* cmdHeader = (HBA_CMD_HEADER*)((uint64_t)port->clb + ((uint64_t)port->clbu << 32));

    for (int i = 0; i < 32; i++)
    {
        cmdHeader->prdtl = 8;

        void* cmdTableAddr = vmm_AllocatePage();
        uint64_t addr = (uint64_t)cmdTableAddr + (i << 8);

        cmdHeader[i].ctba = (uint32_t)addr;
        cmdHeader[i].ctbau = (uint32_t)(addr >> 32);
        memset(cmdTableAddr, 0, 256);
    }

    AHCI_StartCommand(port);
}

