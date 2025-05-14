#include "pic.h"
#include <io.h>

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

typedef enum
{
    PIC_ICW1_ICW4 = 0x01,
    PIC_ICW1_SINGLE = 0x02,
    PIC_ICW1_INTERVAl = 0x04,
    PIC_ICW1_LEVEL = 0x08,
    PIC_ICW1_INITIALIZE = 0x10
} PIC_ICW1;

typedef enum
{
    PIC_ICW4_8086 = 0x01,
    PIC_ICW4_AUTO_EOI = 0x02,
    PIC_ICW4_BUFFER_MASTER = 0x04,
    PIC_ICW4_BUFFER_SLAVE = 0x00,
    PIC_ICW4_BUFFERED = 0x08,
    PIC_ICW4_SFMN = 0x10
} PIC_ICW4;

void PIC_Remap(uint8_t pic1Offset, uint8_t pic2Offset)
{
    // Initialize word control 1
    x64_outb(PIC1_COMMAND_PORT, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    x64_iowait();
    x64_outb(PIC2_COMMAND_PORT, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    x64_iowait();

    // Initialize word control 2
    x64_outb(PIC1_DATA_PORT, pic1Offset);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, pic2Offset);
    x64_iowait();

    // Initialize word control 3
    x64_outb(PIC1_DATA_PORT, 0x02);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, 0x04);
    x64_iowait();

    // Initialize word control 4
    x64_outb(PIC1_DATA_PORT, PIC_ICW4_8086);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, PIC_ICW4_8086);
    x64_iowait();

    // Clear data registers (unmasking all IRQs)
    x64_outb(PIC1_DATA_PORT, 0x00);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, 0x00);
    x64_iowait();
}

void PIC_MaskIRQ(int irq)
{
    uint8_t port;

	if (irq < 8)
	{
		port = PIC1_COMMAND_PORT;
	}
	else
	{
		port = PIC2_COMMAND_PORT;
		irq -= 8;
	}

	uint8_t mask = x64_inb(port);
	x64_outb(port, mask | (1 << irq));
}

void PIC_UnmaskIRQ(int irq)
{
    uint8_t port;

    if (irq < 8)
    {
        port = PIC1_COMMAND_PORT;
    }
    else
    {
        port = PIC2_COMMAND_PORT;
        irq -= 8;
    }

    uint8_t mask = x64_inb(port);
    x64_outb(port, mask & ~(1 << irq));
}

void PIC_SendEOI(int irq)
{
    if (irq >= 8)
    {
        x64_outb(PIC2_COMMAND_PORT, 0x20); // Send EOI to PIC 2
    }

    x64_outb(PIC1_COMMAND_PORT, 0x20); // Send EOI to PIC 1
}

