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

static volatile LIMINE_BASE_REVISION(3);

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

	// Panic testing...
	int value = 1 / 0;
	// uint64_t* overflow = 0x900000000000000000000000000000000000000000000000000000;
	// *overflow = 69;

	halt();
}
