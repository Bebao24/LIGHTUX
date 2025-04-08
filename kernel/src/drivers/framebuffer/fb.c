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

void fb_putPixel(uint64_t x, uint64_t y, uint64_t color)
{
    uint32_t offset = (y * fb_pitch) + (x * 4);
    framebuffer[offset / 4] = color;
}

