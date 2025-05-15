#include "keyboard.h"
#include <io.h>
#include <console.h>
#include <irq.h>
#include <kb_translate.h>

void InitializeKeyboard()
{
    // Register the IRQ handler
    IRQ_RegisterHandler(1, IRQKeyboardHandler);
}

void IRQKeyboardHandler(cpu_registers_t* cpu_status)
{
    // Stop GCC warning
    (void)cpu_status;

    // Read the scancode from the PS2 port
    uint8_t scancode = x64_inb(PS2_KEYBOARD_PORT);

    char key = TranslateToASCII(scancode, false);

    if (key != 0)
    {
        putc(key);
    }

    return;
}

