#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <system.h>
#include <boot.h>
#include <logging.h>
#include <fb.h>
#include <console.h>
#include <bitmap.h>
#include <pmm.h>
#include <vmm.h>
#include <paging.h>
#include <heap.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <pic.h>
#include <keyboard.h>
#include <timer.h>
#include <task.h>
#include <pci.h>
#include <disk.h>
#include <memory.h>
#include <mbr.h>
#include <fat32.h>
#include <shell.h>
#include <spinlock.h>

static volatile LIMINE_BASE_REVISION(3);

uint8_t userspaceCode[] = {
	0xeb, 0xfe // jmp $
};

#define USER_BINARY_BASE 0x00400000

void TestUserspace()
{
	// Create the userspace task
	task_t* task = TaskCreate(USER_BINARY_BASE, paging_AllocatePD(), 0, false);

	uint8_t* codePage = (uint8_t*)USER_BINARY_BASE;
	// Map the code page
	uint64_t* oldPageDir = GetPageDir();
	ChangePageDirUnsafe(task->pageDir);

	paging_MapPage(codePage, pmm_AllocatePage(), PF_USER | PF_RW);
	memset(codePage, 0, PAGE_SIZE);

	// Copy the code
	codePage[0] = userspaceCode[0];
	codePage[1] = userspaceCode[1];
	ChangePageDirUnsafe(oldPageDir);

	task->status = TASK_STATUS_READY;
}

void kmain()
{
	if (LIMINE_BASE_REVISION_SUPPORTED == false)
	{
		panic("Limine base revision isn't supported!");
	}

	GetBootInfo();

	InitializeFramebuffer(); // Limine mapped the framebuffer
	InitializeConsole();

	InitializePMM();
	InitializePaging();
	InitializeHeap(HEAP_SIZE_IN_PAGES);

	InitializeGDT();
	InitializeIDT();
	InitializeISR();
	InitializeIRQ();

	InitializeKeyboard();
	InitializeTimer();

	InitializeTask();

	TestUserspace();

	InitializePCI();

	Partition partition;
	InitializeDisk(&partition);	

	if (!FAT32_Initialize(&partition))
	{
		panic("[KERNEL] Failed to initialize FAT32 driver!\n");
	}

	launchShell();

	halt();
}
