#include "ahci.h"
#include <logging.h>

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

void AHCI_PortProbe(HBA_MEM* mem)
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

