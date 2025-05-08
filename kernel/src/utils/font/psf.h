#pragma once
#include <stdint.h>
#include <stdbool.h>

#define PSF1_MAGIC_0 0x36
#define PSF1_MAGIC_1 0x04

typedef struct
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} PSF_Font;

extern PSF_Font* font;

bool psf_Load();
uint8_t* psf_GetGlyph(uint8_t symbolNumber);


