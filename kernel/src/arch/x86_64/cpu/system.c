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

	// Announce that the kernel panic in the host console
	debugf("[KERNEL] KERNEL PANIC!!! ");

	// Print the panic message
	va_list args;
	va_start(args, message);

	printf_internal(debugc, message, args);
	
	va_end(args);

	// Display the "KERNEL PANIC" on the screen
	printf(" _  _______ ____  _   _ _____ _       ____   _    _   _ ___ ____\n"
		"| |/ / ____|  _ \\| \\ | | ____| |     |  _ \\ / \\  | \\ | |_ _/ ___|\n"
		"| ' /|  _| | |_) |  \\| |  _| | |     | |_) / _ \\ |  \\| || | |    \n"
		"| . \\| |___|  _ <| |\\  | |___| |___  |  __/ ___ \\| |\\  || | |___ \n"
		"|_|\\_\\_____|_| \\_\\_| \\_|_____|_____| |_| /_/   \\_\\_| \\_|___\\____|\n");
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

bool checkInterrupts()
{
	uint16_t flags;
	asm volatile("pushf; pop %0" : "=g"(flags));
	return flags & (1 << 9);
}


