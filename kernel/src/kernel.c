#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3); // Recommended base revision


// Request a framebuffer
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request limineFbReq = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *dest, const void *src, size_t n) {
	uint8_t *pdest = (uint8_t *)dest;
	const uint8_t *psrc = (const uint8_t *)src;

	for (size_t i = 0; i < n; i++) {
		pdest[i] = psrc[i];
	}

	return dest;
}

void *memset(void *s, int c, size_t n) {
	uint8_t *p = (uint8_t *)s;

	for (size_t i = 0; i < n; i++) {
		p[i] = (uint8_t)c;
	}

	return s;
}

void *memmove(void *dest, const void *src, size_t n) {
	uint8_t *pdest = (uint8_t *)dest;
	const uint8_t *psrc = (const uint8_t *)src;

	if (src > dest) {
		for (size_t i = 0; i < n; i++) {
			pdest[i] = psrc[i];
		}
	} else if (src < dest) {
		for (size_t i = n; i > 0; i--) {
			pdest[i-1] = psrc[i-1];
		}
	}

	return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	const uint8_t *p1 = (const uint8_t *)s1;
	const uint8_t *p2 = (const uint8_t *)s2;

	for (size_t i = 0; i < n; i++) {
		if (p1[i] != p2[i]) {
			return p1[i] < p2[i] ? -1 : 1;
		}
	}

	return 0;
}

void halt()
{
	while (true)
	{
		asm volatile("hlt");
	}
}

void kmain()
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false)
	{
		halt();
	}

	if (limineFbReq.response == NULL || limineFbReq.response->framebuffer_count < 1)
	{
		halt();
	}
	
	struct limine_framebuffer* fb = limineFbReq.response->framebuffers[0];

	for (size_t i = 0; i < 100; i++) {
        uint32_t* fbPtr = fb->address;
		fbPtr[i * (fb->pitch / 4) + 1] = 0xffffff;
    }

	// Do nothing
	halt();
}
