#include "boot.h"
#include <system.h>

bootInfo_t bootInfo;

// Request for 4 levels paging
static volatile struct limine_paging_mode_request liminePagingReq = {
	.id = LIMINE_PAGING_MODE_REQUEST,
	.revision = 0,
	.mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

// Request for higher half direct map (HHDM)
static volatile struct limine_hhdm_request limineHHDMReq = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

// Request memory map
static volatile struct limine_memmap_request limineMMReq = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

void GetBootInfo()
{
	struct limine_paging_mode_response* liminePagingRes = liminePagingReq.response;

	// Check if it is actually a 4 levels page table
	if (liminePagingRes->mode != LIMINE_PAGING_MODE_X86_64_4LVL)
	{
		panic();
	}

	// HHDM
	struct limine_hhdm_response* limineHHDMRes = limineHHDMReq.response;
	bootInfo.hhdmOffset = limineHHDMRes->offset;

	struct limine_memmap_response* limineMMRes = limineMMReq.response;
	bootInfo.mmEntries = limineMMRes->entries;
	bootInfo.mmNumEntries = limineMMRes->entry_count;

	// Calculate the total memory
	for (int i = 0; i < limineMMRes->entry_count; i++)
	{
		struct limine_memmap_entry* entry = limineMMRes->entries[i];

		if (entry->type != LIMINE_MEMMAP_RESERVED)
		{
			bootInfo.mmTotal += entry->base;
		}
	}
}
