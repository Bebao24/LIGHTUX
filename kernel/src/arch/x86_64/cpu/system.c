#include "system.h"
#include <stdbool.h>
#include <logging.h>
#include <stdarg.h>
#include <printf.h>
#include <console.h>
#include <fb.h>

void panic(const char* message, ...)
{
	asm volatile ("cli"); // Disable interrupts first

	// Clear the screen
	console_clearScreen();

	// Print the panic message
	va_list args;
	va_start(args, message);

	printf_internal(debugc, message, args);
	
	va_end(args);

	// Display the "KERNEL PANIC" on the screen
	printf("Kernel panic!!!\n");
	printf("Please try to restart your computer\n");
	
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


