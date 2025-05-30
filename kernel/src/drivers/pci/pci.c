#include "pci.h"
#include <io.h>
#include <console.h>

uint16_t PCI_ConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    // Create configuration address as figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    x64_outl(PCI_CONFIG_ADDRESS, address);

    // Read the data
    tmp = (uint16_t)((x64_inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

bool PCI_FilterDevice(uint16_t bus, uint8_t slot, uint8_t func)
{
    // Check for invalid vendor ID
    uint16_t vendorID = PCI_ConfigReadWord(bus, slot, func, 0x00);
    return !(vendorID == 0xFFFF || !vendorID);
}

void InitializePCI()
{
    printf("PCI detection: \n");

    for (uint16_t bus = 0; bus < PCI_MAX_BUSES; bus++)
    {
        for (uint8_t slot = 0; slot < PCI_MAX_SLOTS; slot++)
        {
            for (uint8_t func = 0; func < PCI_MAX_FUNCTIONS; func++)
            {
                if (!PCI_FilterDevice(bus, slot, func))
                {
                    continue;
                }

                uint16_t vendor, device;

                vendor = PCI_ConfigReadWord(bus, slot, func, 0x00);
                device = PCI_ConfigReadWord(bus, slot, func, 0x02);

                printf("Vendor ID: %x, Device ID: %x\n", vendor, device);
            }
        }
    }
}

