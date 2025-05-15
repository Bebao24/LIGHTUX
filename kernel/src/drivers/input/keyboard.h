#pragma once
#include <cpu.h>

#define PS2_KEYBOARD_PORT 0x60

void InitializeKeyboard();
void IRQKeyboardHandler(cpu_registers_t* cpu_status);

