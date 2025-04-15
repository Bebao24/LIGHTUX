#include "fb.h"
#include <stddef.h>
#include <initFb.h>

uint32_t* framebuffer = NULL;

uint64_t fb_width;
uint64_t fb_height;
uint64_t fb_pitch;

void InitializeFramebuffer()
{
    getFBInfo();
}

void fb_putPixel(uint64_t x, uint64_t y, uint32_t color)
{
    uint32_t offset = (y * fb_pitch) + (x * 4);
    framebuffer[offset / 4] = color;
}


void fb_drawRect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color)
{
    for (uint64_t yy = 0; yy < h; yy++)
    {
        for (uint64_t xx = 0; xx < w; xx++)
        {
            fb_putPixel(x + xx, y + yy, color);
        }
    }
}

void fb_clearScreen(uint32_t color)
{
    for (uint64_t y = 0; y < fb_height; y++)
    {
        for (uint64_t x = 0; x < fb_width; x++)
        {
            fb_putPixel(x, y, color);
        }
    }
}
