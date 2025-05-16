#include "keyboard.h"
#include <io.h>
#include <console.h>
#include <irq.h>
#include <kb_translate.h>
#include <heap.h>
#include <memory.h>

key_info_t* g_KeyInfo;

void InitializeKeyboard()
{
    // Register the IRQ handler
    IRQ_RegisterHandler(1, IRQKeyboardHandler);

    // Allocate memory for the key info
    g_KeyInfo = malloc(sizeof(key_info_t));
    memset(g_KeyInfo, 0, sizeof(key_info_t));
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
            g_KeyInfo->uppercase = true;
            return;
        case LeftShift + 0x80:
            // Left shift released
            g_KeyInfo->uppercase = false;
            return;
        case RightShift:
            g_KeyInfo->uppercase = true;
            return;
        case RightShift + 0x80:
            g_KeyInfo->uppercase = false;
            return;
    }

    char key = TranslateToASCII(scancode, g_KeyInfo->uppercase);

    if (key != 0)
    {
        putc(key);
    }

    return;
}

