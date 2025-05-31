#include "pci.h"
#include <io.h>
#include <console.h>
#include <maths.h>
#include <heap.h>
#include <linked_list.h>
#include <ahci.h>

PCI* firstPCI;

PCI* PCI_LookUpDevice(PCIDevice* device)
{
    PCI* browse = firstPCI;

    while (browse)
    {
        if (browse->bus == device->bus && browse->slot == device->slot && 
            browse->func == device->func)
        {
            break;
        }

        browse = browse->next;
    }

    return browse;
}

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

void PCI_GetDevice(PCIDevice* device, uint16_t bus, uint8_t slot, uint8_t func)
{
    // Literally just manually copy everything...

    device->bus = bus;
    device->slot = slot;
    device->func = func;

    device->vendorID = PCI_ConfigReadWord(bus, slot, func, PCI_VENDOR_ID);
    device->deviceID = PCI_ConfigReadWord(bus, slot, func, PCI_DEVICE_ID);

    device->command = PCI_ConfigReadWord(bus, slot, func, PCI_COMMAND);
    device->status = PCI_ConfigReadWord(bus, slot, func, PCI_STATUS);

    // Split 16 bits into two 8 bits
    uint16_t revision_progIF = PCI_ConfigReadWord(bus, slot, func, PCI_REVISION_ID);
    device->revision = EXPORT_BYTE(revision_progIF, true);
    device->progIF = EXPORT_BYTE(revision_progIF, false);

    uint16_t subclass_class = PCI_ConfigReadWord(bus, slot, func, PCI_SUBCLASS);
    device->subClassID = EXPORT_BYTE(subclass_class, true);
    device->classID = EXPORT_BYTE(subclass_class, false);

    // Might be the longest variable I have ever wrote....
    uint16_t cacheLineSize_LatencyTimer = PCI_ConfigReadWord(bus, slot, func, PCI_CACHE_LINE_SIZE);
    device->cacheLineSize = EXPORT_BYTE(cacheLineSize_LatencyTimer, true);
    device->latencyTimer = EXPORT_BYTE(cacheLineSize_LatencyTimer, false);

    uint16_t headerType_bist = PCI_ConfigReadWord(bus, slot, func, PCI_HEADER_TYPE);
    device->headerType = EXPORT_BYTE(headerType_bist, true);
    device->bist = EXPORT_BYTE(headerType_bist, false);
}

void PCI_GetGeneralDevice(PCIDevice* device, PCIGeneralDevice* target)
{
    // For convenient
    uint16_t bus = device->bus;
    uint8_t slot = device->slot;
    uint8_t func = device->func;

    for (int i = 0; i < 6; i++)
    {
        target->bar[i] = COMBINE_WORD(PCI_ConfigReadWord(bus, slot, func, PCI_BAR0 + 4 * i + 2),
        PCI_ConfigReadWord(bus, slot, func, PCI_BAR0 + 4 * i));
    }

    target->systemVendorID = PCI_ConfigReadWord(bus, slot, func, PCI_SYSTEM_VENDOR_ID);
    target->systemID = PCI_ConfigReadWord(bus, slot, func, PCI_SYSTEM_ID);

    target->expansionROMAddr = COMBINE_WORD(PCI_ConfigReadWord(bus, slot, func, PCI_ROM_EXPANSION_ADDR + 2), 
                                PCI_ConfigReadWord(bus, slot, func, PCI_ROM_EXPANSION_ADDR));

    target->capabilitiesPtr = EXPORT_BYTE(PCI_ConfigReadWord(bus, slot, func, PCI_CAPABILITIES_PTR), true);

    uint16_t interruptLine_interruptPin = PCI_ConfigReadWord(bus, slot, func, PCI_INTERRUPT_LINE);
    target->interruptLine = EXPORT_BYTE(interruptLine_interruptPin, true);
    target->interruptPIN = EXPORT_BYTE(interruptLine_interruptPin, false);

    uint16_t minGrant_maxLatency = PCI_ConfigReadWord(bus, slot, func, PCI_MIN_GRANT);
    target->minGrant = EXPORT_BYTE(minGrant_maxLatency, true);
    target->maxLatency = EXPORT_BYTE(minGrant_maxLatency, false);
}

void InitializePCI()
{
    PCIDevice* device = (PCIDevice*)malloc(sizeof(PCIDevice));

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

                PCI_GetDevice(device, bus, slot, func);

                // Check if it is a general device
                // We're only support general device (at least for now...)
                if ((device->headerType & ~(1 << 7)) != PCI_GENERAL_DEVICE)
                {
                    continue;
                }

                PCI* target = LinkedListAllocate((void**)(&firstPCI), sizeof(PCI));
                target->bus = bus;
                target->slot = slot;
                target->func = func;

                target->vendorID = device->vendorID;
                target->deviceID = device->deviceID;

                // Initialize the devices' drivers
                if (device->classID == 0x01 &&        // Mass storage
                    device->subClassID == 0x06 &&     // SATA controller
                    device->progIF == 0x01)           // AHCI 1.0
                {
                    InitializeAHCI(device);
                }
            }
        }
    }

    free(device);
}

