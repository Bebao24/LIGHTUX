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
	fb_clearScreen(COLOR(0, 0, 255));

	// Draw a red rectangle
	fb_drawRect(50, 50, 100, 100, COLOR(255, 0, 0));

	halt();
}
