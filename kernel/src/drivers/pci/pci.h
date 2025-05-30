#pragma once
#include <stdint.h>
#include <stdbool.h>

#define PCI_MAX_BUSES 256
#define PCI_MAX_SLOTS 32
#define PCI_MAX_FUNCTIONS 8

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint16_t PCI_ConfigReadWord(uint16_t bus, uint8_t slot, uint8_t func, uint8_t offset);

void InitializePCI();
bool PCI_FilterDevice(uint16_t bus, uint8_t slot, uint8_t func);


