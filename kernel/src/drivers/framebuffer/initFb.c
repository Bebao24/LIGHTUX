#include "initFb.h"
#include <limine.h>
#include <fb.h>
#include <system.h>
#include <stddef.h>
#include <logging.h>

static volatile struct limine_framebuffer_request limineFbReq = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void getFBInfo()
{
    // Check if the framebuffer is avaliable
    if (limineFbReq.response == NULL || limineFbReq.response->framebuffer_count < 1)
    {
        panic("Can't request for a framebuffer!\n");
    }

    // Write the framebuffer's information
    struct limine_framebuffer* limineFb = limineFbReq.response->framebuffers[0]; // Fetch the first one
    framebuffer = (uint32_t*)limineFb->address;
    fb_width = limineFb->width;
    fb_height = limineFb->height;
    fb_pitch = limineFb->pitch;

    debugf("Framebuffer information: %dx%dx%d\n", fb_width, fb_height, limineFb->bpp);
}


