#include "psf.h"
#include <fontHeader.h>
#include <system.h>
#include <logging.h>

PSF_Font* font;

bool psf_Load()
{
    void* fontData = &zap_light_psf[0];
    PSF_Font* fontHeader = (PSF_Font*)fontData;

    // Verify
    if (fontHeader->magic[0] != PSF1_MAGIC_0 || fontHeader->magic[1] != PSF1_MAGIC_1)
    {
        panic("[PSF] Invalid PSF1 font type!\n");
    }

    font = fontData;
    debugf("[PSF] PSf1 font loaded successfully! charsize=%d\n", font->charsize);

    return true;
}

uint8_t* psf_GetGlyph(uint8_t symbolNumber)
{
    return (uint8_t*)font + sizeof(PSF_Font) + (symbolNumber * font->charsize);
}

