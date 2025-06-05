#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <system.h>
#include <boot.h>
#include <logging.h>
#include <fb.h>
#include <console.h>
#include <bitmap.h>
#include <pmm.h>
#include <vmm.h>
#include <paging.h>
#include <heap.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <pic.h>
#include <keyboard.h>
#include <timer.h>
#include <task.h>
#include <pci.h>
#include <disk.h>
#include <memory.h>
#include <mbr.h>

static volatile LIMINE_BASE_REVISION(3);

void test1()
{
	debugf("Hello World from task1!\n");

	while (true)
	{
	}
}

void test2()
{
	debugf("Hello World from task2!\n");

	while (true)
	{
	}
}

void kmain()
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false)
	{
		panic("Limine base revision isn't supported!");
	}

	GetBootInfo();

	InitializeFramebuffer(); // Limine mapped the framebuffer
	InitializeConsole();

	// Testing printf()
	printf("Hello World!\n");
	printf("It works because it can print out 0x%x\n", 0x123);

	InitializePMM();
	InitializePaging();
	InitializeHeap(HEAP_SIZE_IN_PAGES);

	InitializeGDT();
	InitializeIDT();
	InitializeISR();
	InitializeIRQ();

	InitializeKeyboard();
	InitializeTimer();

	InitializeTask();

	InitializePCI();
	printf("PCI detection: \n");

	// Print all PCI devices' info
	PCI* browse = firstPCI;
	while (browse)
	{
		printf("Vendor ID: %x, Device ID: %x\n", browse->vendorID, browse->deviceID);
		browse = browse->next;
	}

	Partition partition;
	InitializeDisk(&partition);	

	debugf("[DISK] partition offset: %d\n", partition.partitionOffset);
	debugf("[DISK] partition size: %d\n", partition.partitionSize);

	// Try to read the first partition
	uint8_t buffer[512];
	if (!MBR_ReadSectors(&partition, 0, 1, buffer))
	{
		debugf("Failed to read partition!\n");
	}

	for (int i = 0; i < 512; i++)
	{
		printf("%c", buffer[i]);
	}

	while (true)
	{
		char key = GetKey();

		if (key == '\r')
		{
			printf("\n");
		}

		putc(key);
	}

	halt();
}
