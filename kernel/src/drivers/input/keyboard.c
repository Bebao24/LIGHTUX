#include "keyboard.h"
#include <io.h>
#include <logging.h>
#include <irq.h>

void InitializeKeyboard()
{
    // Register the IRQ handler
    IRQ_RegisterHandler(1, IRQKeyboardHandler);
}

void IRQKeyboardHandler(cpu_registers_t* cpu_status)
{
    // TODO: Translate the scancode into ASCII and be able to actually type on the screen

    // Stop GCC warning
    (void)cpu_status;

    // Read the scancode from the PS2 port
    uint8_t scancode = x64_inb(PS2_KEYBOARD_PORT);
    (void)scancode;

    // debugf("Key pressed!");

    return;
}

