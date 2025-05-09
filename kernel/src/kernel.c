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
	debugf("Free memory: %lld KB\n", DivRoundUp(pmm_GetFreeMem(), 1024));
	debugf("Used memory: %lld KB\n", DivRoundUp(pmm_GetUsedMem(), 1024));
	debugf("Reserved memory: %lld KB\n", DivRoundUp(pmm_GetReservedMem(), 1024));

	halt();
}
