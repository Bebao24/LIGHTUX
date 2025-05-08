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

	uint8_t testArray[20];
	bitmap_t testBitmap;

	InitializeBitmap(&testBitmap, sizeof(testArray), testArray);

	bitmap_Set(&testBitmap, 1, true);
	bitmap_Set(&testBitmap, 3, true);
	bitmap_Set(&testBitmap, 5, true);
	bitmap_Set(&testBitmap, 8, true);
	bitmap_Set(&testBitmap, 11, true);
	bitmap_Set(&testBitmap, 15, true);
	bitmap_Set(&testBitmap, 20, true);

	for (int i = 0; i < 21; i++)
	{
		debugf(bitmap_Get(&testBitmap, i) ? "true" : "false");
		debugf("\n");
	}


	halt();
}
