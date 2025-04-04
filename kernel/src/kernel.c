#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <system.h>
#include <boot.h>
#include <logging.h>

static volatile LIMINE_BASE_REVISION(3);

void kmain()
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false)
	{
		panic();
	}

	GetBootInfo();

	debugs("Hello World!\n");

	halt();
	
}
