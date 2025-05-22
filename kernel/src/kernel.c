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

static volatile LIMINE_BASE_REVISION(3);

void test1()
{
	for (int i = 0; i < 10; i++)
	{
		debugf("g");
	}

	while (true)
	{
	}
}

void test2()
{
	for (int i = 0; i < 10; i++)
	{
		debugf("f");
	}

	while (true)
	{
	}
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

	// Testing printf()
	printf("Hello World!\n");
	printf("It works because it can print out 0x%x\n", 0x123);

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

	// Test
	task_t* task1 = TaskCreate((uint64_t)test1, paging_AllocatePD(), 0);
	taskName(task1, "task1", sizeof("task1"));
	task1->status = TASK_STATUS_READY;
	task_t* task2 = TaskCreate((uint64_t)test2, paging_AllocatePD(), 0);
	taskName(task2, "task2", sizeof("task2"));
	task2->status = TASK_STATUS_READY;

	while (true)
	{
		char key = GetKey();

		if (key == '\r')
		{
			printf("\n");
		}

		putc(key);
	}

	halt();
}
