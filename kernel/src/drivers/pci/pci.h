#pragma once
#include <stdint.h>
#include <stdbool.h>

#define PCI_MAX_BUSES 256
#define PCI_MAX_SLOTS 32
#define PCI_MAX_FUNCTIONS 8

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint16_t PCI_ConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void PCI_ConfigWriteDWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t config);

// Device type
#define PCI_GENERAL_DEVICE 0x00

typedef struct
{
    uint16_t bus;
    uint8_t slot;
    uint8_t func;

    uint16_t vendorID;
    uint16_t deviceID;

    uint16_t command;
    uint16_t status;

    uint8_t revision;
    uint8_t progIF; // Program interface
    uint8_t subClassID;
    uint8_t classID;

    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    uint8_t headerType;
    uint8_t bist;
} PCIDevice;

typedef struct
{
    uint32_t bar[6];

    uint32_t cardBusCISPtr;

    uint16_t systemID;
    uint16_t systemVendorID;

    uint32_t expansionROMAddr;

    uint8_t capabilitiesPtr;

    uint8_t interruptLine;
    uint8_t interruptPIN;
    uint8_t minGrant;
    uint8_t maxLatency;
} PCIGeneralDevice;

typedef struct PCI
{
    struct PCI* next;

    uint16_t bus;
    uint8_t slot, func;
    uint16_t vendorID, deviceID;
} PCI;

extern PCI* firstPCI;

// PCI config offset
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0A
#define PCI_CLASS 0x0B
#define PCI_CACHE_LINE_SIZE 0x0C
#define PCI_LATENCY_TIMER 0x0D
#define PCI_HEADER_TYPE 0x0E
#define PCI_BIST 0x0F

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_SYSTEM_VENDOR_ID 0x2C
#define PCI_SYSTEM_ID 0x2E
#define PCI_ROM_EXPANSION_ADDR 0x30
#define PCI_CAPABILITIES_PTR 0x34
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_MIN_GRANT 0x3E

void InitializePCI();
bool PCI_FilterDevice(uint16_t bus, uint8_t slot, uint8_t func);
void PCI_GetDevice(PCIDevice* device, uint16_t bus, uint8_t slot, uint8_t func);
void PCI_GetGeneralDevice(PCIDevice* device, PCIGeneralDevice* target);

PCI* PCI_LookUpDevice(PCIDevice* device);

