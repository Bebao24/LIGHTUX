#pragma once
#include <stdint.h>
#include <stdbool.h>

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39
#define Tab 0x0F

char TranslateToASCII(uint8_t scancode, bool uppercase);

