#include "keyboard.h"
#include <io.h>
#include <console.h>
#include <irq.h>
#include <kb_translate.h>

static bool uppercase = false;

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

    switch (scancode)
    {
        case LeftShift:
            uppercase = true;
            return;
        case LeftShift + 0x80:
            // Left shift released
            uppercase = false;
            return;
        case RightShift:
            uppercase = true;
            return;
        case RightShift + 0x80:
            uppercase = false;
            return;
    }

    char key = TranslateToASCII(scancode, uppercase);

    if (key != 0)
    {
        putc(key);
    }

    return;
}

