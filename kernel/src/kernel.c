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

static volatile LIMINE_BASE_REVISION(3);

#define USER_BINARY_BASE 0x00400000

void TestUserspace()
{
	// Create the userspace task
	char* name = "Userspace task";
	task_t* task = TaskCreate(USER_BINARY_BASE, paging_AllocatePD(), 0, false);
	taskName(task, name, sizeof(name));

	uint64_t* oldPageDir = GetPageDir();
	ChangePageDirFake(task->pageDir);

	// Map the binary base address
	uint8_t binaryData[] = { 0xEB, 0xFE }; // jmp $
	size_t binarySize = sizeof(binaryData);

	size_t pagesCount = (binarySize + PAGE_SIZE - 1) / PAGE_SIZE;
	for (size_t i = 0; i < pagesCount; i++)
	{
		paging_MapPage((void*)(USER_BINARY_BASE + i * PAGE_SIZE), pmm_AllocatePage(), PF_RW | PF_USER);
	}

	// Copy the binary data
	memcpy((void*)USER_BINARY_BASE, binaryData, binarySize);

	ChangePageDirFake(oldPageDir);

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

	InitializePCI();

	Partition partition;
	InitializeDisk(&partition);

	if (!FAT32_Initialize(&partition))
	{
		panic("[KERNEL] Failed to initialize FAT32 driver!\n");
	}

	TestUserspace();

	// launchShell();

	halt();
}
