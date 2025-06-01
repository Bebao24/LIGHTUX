#include "ahci.h"

void AHCI_StopCommand(HBA_PORT* port)
{
    port->cmd &= ~(HBA_PxCMD_FRE);
    port->cmd &= ~(HBA_PxCMD_ST);

    // Wait until FR and CR bits are cleared
    while (true)
    {
        if (port->cmd & HBA_PxCMD_FR)
        {
            continue;
        }
        if (port->cmd & HBA_PxCMD_CR)
        {
            continue;
        }

        break;
    }
}

void AHCI_StartCommand(HBA_PORT* port)
{
    while (port->cmd & HBA_PxCMD_CR);

    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

