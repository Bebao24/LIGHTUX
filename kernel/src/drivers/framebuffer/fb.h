#pragma once
#include <stdint.h>

#define COLOR(r, g, b) ((r << 16) | (g << 8) | (b))

extern uint32_t* framebuffer;

extern uint64_t fb_width;
extern uint64_t fb_height;
extern uint64_t fb_pitch;

void InitializeFramebuffer();
void fb_putPixel(uint64_t x, uint64_t y, uint32_t color);

void fb_drawRect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color);
void fb_clearScreen(uint32_t color);


