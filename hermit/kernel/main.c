/*
 * Copyright (c) 2010, Stefan Lankes, RWTH Aachen University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the University nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <hermit/stddef.h>
#include <hermit/stdio.h>
#include <hermit/string.h>
#include <hermit/time.h>
#include <hermit/tasks.h>
#include <hermit/processor.h>
#include <hermit/tasks.h>
#include <hermit/syscall.h>
#include <hermit/memory.h>
#include <hermit/fs.h>
#include <asm/irq.h>
#include <asm/atomic.h>
#include <asm/page.h>

/*
 * Note that linker symbols are not variables, they have no memory allocated for
 * maintaining a value, rather their address is their value.
 */
extern const void kernel_start;
extern const void kernel_end;
extern const void bss_start;
extern const void bss_end;
extern const void percore_start;
extern const void percore_end0;
extern const void percore_end;
extern char __BUILD_DATE;

/* Page frame counters */
extern atomic_int64_t total_pages;
extern atomic_int64_t total_allocated_pages;
extern atomic_int64_t total_available_pages;

extern atomic_int32_t cpu_online;

static int foo(void* arg)
{
	int i;

	for(i=0; i<5; i++) {
		kprintf("hello from %s\n", (char*) arg);
		sleep(1);
	}

	return 0;
}

static int hermit_init(void)
{
	uint32_t i;
	size_t sz = (size_t) &percore_end0 - (size_t) &percore_start;

	// initialize .bss section
	memset((void*)&bss_start, 0x00, ((size_t) &bss_end - (size_t) &bss_start));

	// initialize .percore section => copy first section to all other sections
	for(i=1; i<MAX_CORES; i++)
		memcpy((char*) &percore_start + i*sz, (char*) &percore_start, sz);

	koutput_init();
	system_init();
	irq_init();
	timer_init();
	multitasking_init();
	memory_init();
	initrd_init();

	return 0;
}

#if MAX_CORES > 1
int smp_main(void)
{
	int32_t cpu = atomic_int32_inc(&cpu_online);

	kprintf("%d CPUs are now online\n", cpu);

#ifdef CONFIG_TICKLESS
	disable_timer_irq();
#endif

	create_kernel_task(NULL, foo, "foo2", NORMAL_PRIO);

	while(1) {
		check_workqueues();
		HALT;
	}

	return 0;
}
#endif

// init task => creates all other tasks an initialize the LwIP
static int initd(void* arg)
{
	char* argv1[] = {"/bin/hello", NULL};
	char* argv2[] = {"/bin/jacobi", NULL};
	char* argv3[] = {"/bin/stream", NULL};

	//create_kernel_task(NULL, foo, "foo1", NORMAL_PRIO);
	//create_kernel_task(NULL, foo, "foo2", NORMAL_PRIO);
	create_user_task(NULL, "/bin/hello", argv1, NORMAL_PRIO);
	create_user_task(NULL, "/bin/jacobi", argv2, NORMAL_PRIO);
	//create_user_task(NULL, "/bin/jacobi", argv2, NORMAL_PRIO);
	//create_user_task(NULL, "/bin/stream", argv3, NORMAL_PRIO);

	return 0;
}

int main(void)
{
	hermit_init();
	system_calibration(); // enables also interrupts

	atomic_int32_inc(&cpu_online);

	kprintf("This is Hermit %s, build date %u\n", VERSION, &__BUILD_DATE);
	kprintf("Kernel starts at %p and ends at %p\n", &kernel_start, &kernel_end);
	kprintf("Per core data starts at %p and ends at %p\n", &percore_start, &percore_end);
	kprintf("Per core size 0x%zd\n", (size_t) &percore_end0 - (size_t) &percore_start);
	kprintf("Processor frequency: %u MHz\n", get_cpu_frequency());
	kprintf("Total memory: %zd MiB\n", atomic_int64_read(&total_pages) * PAGE_SIZE / (1024ULL*1024ULL));
	kprintf("Current allocated memory: %zd KiB\n", atomic_int64_read(&total_allocated_pages) * PAGE_SIZE / 1024ULL);
	kprintf("Current available memory: %zd MiB\n", atomic_int64_read(&total_available_pages) * PAGE_SIZE / (1024ULL*1024ULL));

#if 1
	kputs("Filesystem:\n");
	list_fs(fs_root, 1);
#endif

#ifdef CONFIG_TICKLESS
	disable_timer_irq();
#endif

	create_kernel_task(NULL, initd, NULL, NORMAL_PRIO);

	while(1) {
		check_workqueues();
		HALT;
	}

	return 0;
}
