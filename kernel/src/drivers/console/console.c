#include "console.h"
#include <psf.h>
#include <fb.h>

void InitializeConsole()
{
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

