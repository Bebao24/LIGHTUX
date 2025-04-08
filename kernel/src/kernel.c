#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <system.h>
#include <boot.h>
#include <logging.h>
#include <fb.h>

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
	// Draw a red rectangle
	for (uint64_t y = 0; y < 50; y++)
	{
		for (uint64_t x = 0; x < 100; x++)
		{
			fb_putPixel(x, y, COLOR(255, 0, 0));
		}
	}

	halt();
}
