#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <system.h>
#include <boot.h>
#include <logging.h>
#include <fb.h>
#include <console.h>

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

	// Draw "Hi!" on the screen
	console_drawChar('H', 50, 50, COLOR(255, 255, 255));
	console_drawChar('i', 50 + 8, 50, COLOR(255, 255, 255));
	console_drawChar('!', 50 + 16, 50, COLOR(255, 255, 255));

	halt();
}
