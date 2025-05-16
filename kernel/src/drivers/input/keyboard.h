#pragma once
#include <cpu.h>
#include <stdbool.h>

#define PS2_KEYBOARD_PORT 0x60

typedef struct
{
    char key;
    bool uppercase;
} key_info_t;

void InitializeKeyboard();
void IRQKeyboardHandler(cpu_registers_t* cpu_status);

