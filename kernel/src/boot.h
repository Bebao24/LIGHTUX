#pragma once
#include <stdint.h>
#include <limine.h>

typedef struct
{
	uint64_t hhdmOffset;

	uint64_t mmTotal; // Total amount of memory
	uint64_t mmNumEntries;
	LIMINE_PTR(struct limine_memmap_entry**)mmEntries;
} bootInfo_t;

extern bootInfo_t bootInfo;

void GetBootInfo();


