#include "system.h"
#include <stdbool.h>
#include <logging.h>

void panic(const char* message)
{
	// Print the panic message first
	// TODO: Be able to use this also like printf() to pass arguments
	debugf("[KERNEL PANIC] %s\n", message);

	asm volatile ("cli"); // Disable interrupts first
	
	// halt loop
	while (true)
	{
		asm volatile ("hlt");
	}
}

void halt()
{
	while (true)
	{
		asm volatile ("hlt");
	}
}


