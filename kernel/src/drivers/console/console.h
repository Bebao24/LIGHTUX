#pragma once
#include <stdint.h>
#include <color.h>

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

#define BACKGROUND_COLOR (COLOR(0, 0, 255))
#define FOREGROUND_COLOR (COLOR(255, 255, 255))

void InitializeConsole();
void console_drawChar(char c, uint32_t x, uint32_t y, uint32_t color);

void console_clearScreen();
void putc(char c);
void puts(const char* string);
void printf(const char* fmt, ...);

// Cursor
void eraseCursor();
void updateCursor();

