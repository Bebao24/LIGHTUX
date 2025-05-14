#pragma once
#include <stdint.h>

#define PIC_REMAP_OFFSET 0x20

void PIC_Remap(uint8_t pic1Offset, uint8_t pic2Offset);
void PIC_MaskIRQ(int irq);
void PIC_UnmaskIRQ(int irq);

void PIC_SendEOI(int irq);
