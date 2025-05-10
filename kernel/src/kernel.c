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
#include <maths.h>

static volatile LIMINE_BASE_REVISION(3);

void kmain()
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false)
	{
		panic("Limine base revision isn't supported!");
	}

	GetBootInfo();

	InitializeFramebuffer(); // Limine mapped the framebuffer

	// Some testing...
	fb_clearScreen(COLOR(0, 0, 255));

	InitializeConsole();

	// Testing printf()
	printf("Hello World!\n");
	printf("It works because it can print out 0x%x\n", 0x123);

	InitializePMM();
	InitializePaging();

	// Test page mapping
	uint64_t* testAddr = 0x1000000000000000000000000000000;
	paging_MapPage(testAddr, (void*)0x10000, PF_RW);

	// Try writing to that address
	*testAddr = 69;
	debugf("Results: %d\n", *testAddr);

	// Should be 0x10000
	debugf("Physical address: 0x%x\n", paging_VirtToPhysical(testAddr));

	halt();
}
