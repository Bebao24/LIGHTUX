#include "system.h"
#include <stdbool.h>

void panic()
{
	// TODO: Add printing the panic message
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


