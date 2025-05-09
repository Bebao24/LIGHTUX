#include "console.h"
#include <psf.h>
#include <fb.h>
#include <printf.h>
#include <stdbool.h>

// The reason that we used "int" instead of "uint32_t" is that we can check the number if it is < 0
int g_ScreenX, g_ScreenY;

void InitializeConsole()
{
	console_clearScreen();

	psf_Load();
}

void console_drawChar(char c, uint32_t x, uint32_t y, uint32_t color)
{
	uint8_t* glyphBuffer = psf_GetGlyph(c);

	for (uint32_t yy = y; yy < y + 16; yy++)
	{
		for (uint32_t xx = x; xx < x + 8; xx++)
		{
			if ((*glyphBuffer & (0b10000000 >> (xx - x))) > 0)
			{
				fb_putPixel(xx, yy, color);
			}
		}

		glyphBuffer++;
	}
}

void console_clearScreen()
{
	// Reset cursor
	g_ScreenX = 0;
	g_ScreenY = 0;

	// Clear the screen (or the framebuffer)
	fb_clearScreen(BACKGROUND_COLOR);

	// Finally, update the cursor
	updateCursor();
}

bool show_cursor = true;

void eraseCursor()
{
	if (!show_cursor)
	{
		return;
	}

	fb_drawRect(g_ScreenX, g_ScreenY, CHAR_WIDTH, CHAR_HEIGHT, BACKGROUND_COLOR);
}

void updateCursor()
{
	if (!show_cursor)
	{
		return;
	}

	if ((uint64_t)g_ScreenX >= fb_width)
	{
		// Create a new line
		g_ScreenY += CHAR_HEIGHT;

		if ((uint64_t)g_ScreenY >= fb_height)
		{
			console_clearScreen();
		}

		g_ScreenX = 0;
	}

	fb_drawRect(g_ScreenX, g_ScreenY, CHAR_WIDTH, CHAR_HEIGHT, FOREGROUND_COLOR);
}

void putc(char c)
{
	switch (c)
	{
		case '\r':
			// Carriage return
			g_ScreenX = 0;
			break;
		case '\n':
			// New line
			eraseCursor();
			g_ScreenX = 0;
			g_ScreenY += CHAR_HEIGHT;
			break;
		case '\t':
			// Tab (4 spaces)
			eraseCursor();
			for (int i = 0; i < 4 - (g_ScreenX % 4); i++)
			{
				putc(' ');
			}
			break;
		default:
			eraseCursor();
			console_drawChar(c, g_ScreenX, g_ScreenY, FOREGROUND_COLOR);
			g_ScreenX += CHAR_WIDTH;
			break;
	}

	updateCursor();

	// Check for screen overflow
	if ((uint64_t)g_ScreenX >= fb_width)
	{
		// Create a new line
		g_ScreenX = 0;
		g_ScreenY += CHAR_HEIGHT;
	}

	if ((uint64_t)g_ScreenY >= fb_height)
	{
		// Might as well implement scrolling
		console_clearScreen();
	}
}

void puts(const char* string)
{
	while (*string)
	{
		putc(*string);
		string++;
	}
}

void printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	printf_internal(putc, fmt, args);

	va_end(args);
}

