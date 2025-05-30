#pragma once
#include <stdint.h>
#include <stdbool.h>

#define PCI_MAX_BUSES 256
#define PCI_MAX_SLOTS 32
#define PCI_MAX_FUNCTIONS 8

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint16_t PCI_ConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset);

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

void InitializePCI();
bool PCI_FilterDevice(uint16_t bus, uint8_t slot, uint8_t func);
void PCI_GetDevice(PCIDevice* device, uint16_t bus, uint8_t slot, uint8_t func);
PCI* PCI_LookUpDevice(PCIDevice* device);

