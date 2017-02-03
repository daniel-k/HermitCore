/* Copyright (c) 2015, IBM
 * Author(s): Dan Williams <djwillia@us.ibm.com>
 *            Ricardo Koller <kollerr@us.ibm.com>
 * Copyright (c) 2017, RWTH Aachen University
 * Author(s): Stefan Lankes <slankes@eonerc.rwth-aachen.de>
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* We used several existing projects as guides
 * kvmtest.c: http://lwn.net/Articles/658512/
 * lkvm: http://github.com/clearlinux/kvmtool
 */

/* 
 * 15.1.2017: extend original version (https://github.com/Solo5/solo5)
 *            for HermitCore
 */

#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <limits.h>
#include <assert.h>
#include <pthread.h>
#include <elf.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/const.h>
#include <linux/kvm.h>
#include <asm/msr-index.h>

#include "uhyve-cpu.h"
#include "proxy.h"

#define GUEST_OFFSET		0x0
#define CPUID_FUNC_PERFMON	0x0A
#define GUEST_PAGE_SIZE		0x200000   /* 2 MB pages in guest */

#define BOOT_GDT	0x1000
#define BOOT_INFO	0x2000
#define BOOT_PML4	0x10000
#define BOOT_PDPTE	0x11000
#define BOOT_PDE	0x12000

#define BOOT_GDT_NULL	0
#define BOOT_GDT_CODE	1
#define BOOT_GDT_DATA	2
#define BOOT_GDT_MAX	3

#define KVM_32BIT_MAX_MEM_SIZE	(1ULL << 32)
#define KVM_32BIT_GAP_SIZE	(768 << 20)
#define KVM_32BIT_GAP_START	(KVM_32BIT_MAX_MEM_SIZE - KVM_32BIT_GAP_SIZE)

#define UHYVE_PORT_WRITE	0x499
#define UHYVE_PORT_OPEN		0x500
#define UHYVE_PORT_CLOSE	0x501
#define UHYVE_PORT_READ		0x502
#define UHYVE_PORT_EXIT		0x503
#define UHYVE_PORT_LSEEK	0x504

#define kvm_ioctl(fd, cmd, arg) ({ \
	int ret = ioctl(fd, cmd, arg); \
	if(ret == -1) \
	    err(1, "KVM: ioctl " #cmd " failed"); \
	ret; \
	})

static uint8_t* guest_mem = NULL;
static uint8_t* klog = NULL;
static size_t guest_size = 0x20000000ULL;
static uint64_t elf_entry;
//static pthread_t vcpu_thread;
static volatile uint8_t done = 0;

struct kvm_vm {
	int kvm_fd;
	int vm_fd;
};

static struct kvm_vm kvm_vm;

typedef struct {
	int fd;
	const char* buf;
	size_t len;
} __attribute__((packed)) uhyve_write_t;

typedef struct {
	const char* name;
	int flags;
	int mode;
	int ret;
} __attribute__((packed)) uhyve_open_t;

typedef struct {
	int fd;
	int ret;
} __attribute__((packed)) uhyve_close_t;

typedef struct {
	int fd;
	char* buf;
	size_t len;
	ssize_t ret;
} __attribute__((packed)) uhyve_read_t;

typedef struct {
	int fd;
	off_t offset;
	int whence;
} __attribute__((packed)) uhyve_lseek_t;

static void uhyve_exit(void)
{
	char* str = getenv("HERMIT_VERBOSE");

	if (done == 0) {
		done = 1;
		//pthread_kill(vcpu_thread, SIGINT);
	}

	if (klog && str && (strcmp(str, "0") != 0))
	{
		puts("\nDump kernel log:");
		puts("================\n");
		printf("%s\n", klog);
	}

//	if (vcpufd != -1)
//		close(vcpufd);
	if (kvm_vm.vm_fd != -1)
		close(kvm_vm.vm_fd);
	if (kvm_vm.kvm_fd != -1)
		close(kvm_vm.kvm_fd);
}

static uint32_t get_cpufreq(void)
{
#if 1
	char line[2048];
	uint32_t freq = 0;

	FILE* fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
	if (!fp) {
		perror("Unable to open /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq\n");
		return freq;
	}

	if (fgets(line, 2048, fp))
		freq = atoi(line) / 1000;

	return freq;
#else
	uint32_t freq = 0;
	char line[2048];
	char* match;
	char* point;

	FILE* fp = fopen("/proc/cpuinfo", "r");
	if (!fp)
                return freq;

	while(fgets(line, 2048, fp)) {
		if ((match = strstr(line, "cpu MHz")) == NULL)
			continue;

		// scan strinf for the next number
		for(; (*match < 0x30) || (*match > 0x39); match++)
			;

		for(point = match; ((*point != '.') && (*point != '\0')); point++)
			;
		*point = '\0';

		freq = atoi(match);
		fclose(fp);

                return freq;
	}

	return freq;
#endif
}

static ssize_t pread_in_full(int fd, void *buf, size_t count, off_t offset)
{
	ssize_t total = 0;
	char *p = buf;

	if (count > SSIZE_MAX) {
		errno = E2BIG;
		return -1;
	}

	while (count > 0) {
		ssize_t nr;

		nr = pread(fd, p, count, offset);
		if (nr == 0)
			return total;
		else if (nr == -1 && errno == EINTR)
			continue;
		else if (nr == -1)
			return -1;

		count -= nr;
		total += nr;
		p += nr;
		offset += nr;
	}

	return total;
}

static int load_kernel(uint8_t* mem, char* path, uint32_t cpu_count)
{
	Elf64_Ehdr hdr;
	Elf64_Phdr *phdr = NULL;
	size_t buflen;
	int fd, ret;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		perror("Unable to open file");
		return -1;
	}

	ret = pread_in_full(fd, &hdr, sizeof(hdr), 0);
	if (ret < 0)
		goto out;

	//  check if the program is a HermitCore file
	if (hdr.e_ident[EI_MAG0] != ELFMAG0
	    || hdr.e_ident[EI_MAG1] != ELFMAG1
	    || hdr.e_ident[EI_MAG2] != ELFMAG2
	    || hdr.e_ident[EI_MAG3] != ELFMAG3
	    || hdr.e_ident[EI_CLASS] != ELFCLASS64
	    || hdr.e_ident[EI_OSABI] != HERMIT_ELFOSABI
	    || hdr.e_type != ET_EXEC || hdr.e_machine != EM_X86_64) {
		fprintf(stderr, "Inavlide HermitCore file!\n");
		goto out;
	}

	elf_entry = hdr.e_entry;

	buflen = hdr.e_phentsize * hdr.e_phnum;
	phdr = malloc(buflen);
	if (!phdr) {
		fprintf(stderr, "Not enough memory\n");
		goto out;
	}

	ret = pread_in_full(fd, phdr, buflen, hdr.e_phoff);
	if (ret < 0)
		goto out;

	/*
	 * Load all segments with type "LOAD" from the file at offset
	 * p_offset, and copy that into in memory.
	 */
	for (Elf64_Half ph_i = 0; ph_i < hdr.e_phnum; ph_i++)
	{
		uint64_t paddr = phdr[ph_i].p_paddr;
		size_t offset = phdr[ph_i].p_offset;
		size_t filesz = phdr[ph_i].p_filesz;
		size_t memsz = phdr[ph_i].p_memsz;

		if (phdr[ph_i].p_type != PT_LOAD)
			continue;

		//printf("Kernel location 0x%zx, file size 0x%zx\n", paddr, filesz);

		ret = pread_in_full(fd, mem+paddr-GUEST_OFFSET, filesz, offset);
		if (ret < 0)
			goto out;
		memset(mem+paddr+filesz-GUEST_OFFSET, 0x00, memsz - filesz);
		if (!klog)
			klog = mem+paddr+0x5000-GUEST_OFFSET;

		// initialize kernel
		*((uint64_t*) (mem+paddr-GUEST_OFFSET + 0x08)) = paddr; // physical start address
		*((uint64_t*) (mem+paddr-GUEST_OFFSET + 0x10)) = guest_size;   // physical limit
		*((uint32_t*) (mem+paddr-GUEST_OFFSET + 0x18)) = get_cpufreq();
		*((uint32_t*) (mem+paddr-GUEST_OFFSET + 0x24)) = cpu_count; // number of used cpus
		*((uint32_t*) (mem+paddr-GUEST_OFFSET + 0x30)) = 0; // apicid
		*((uint64_t*) (mem+paddr-GUEST_OFFSET + 0x38)) = filesz;
		*((uint32_t*) (mem+paddr-GUEST_OFFSET + 0x60)) = 1; // numa nodes
		*((uint32_t*) (mem+paddr-GUEST_OFFSET + 0x94)) = 1; // announce uhyve
	}
out:
	if (phdr)
		free(phdr);

	close(fd);

	return 0;
}

static void filter_cpuid(struct kvm_cpuid2 *kvm_cpuid)
{
	unsigned int i;

	/*
	 * Filter CPUID functions that are not supported by the hypervisor.
	 */
	for (i = 0; i < kvm_cpuid->nent; i++) {
		struct kvm_cpuid_entry2 *entry = &kvm_cpuid->entries[i];

		switch (entry->function) {
		case 1: // CPUID to define basic cpu features
			entry->ecx = entry->ecx | (1 << 31); // propagate that we are running on a hypervisor
			entry->ecx = entry->ecx & ~(1 << 21); // disable X2APIC support
			entry->edx = entry->edx | (1 << 5); // enable msr support
			break;
		case CPUID_FUNC_PERFMON:
			entry->eax	= 0x00;	/* disable it */
			break;
		default:
			/* Keep the CPUID function as -is */
			break;
		};
	}
}

static void setup_system_64bit(struct kvm_sregs *sregs)
{
	sregs->cr0 |= X86_CR0_PE;
	sregs->efer |= EFER_LME;
}


static void setup_system_page_tables(struct kvm_sregs *sregs, uint8_t *mem)
{
	uint64_t *pml4 = (uint64_t *) (mem + BOOT_PML4);
	uint64_t *pdpte = (uint64_t *) (mem + BOOT_PDPTE);
	uint64_t *pde = (uint64_t *) (mem + BOOT_PDE);
	uint64_t paddr;

	/*
	 * For simplicity we currently use 2MB pages and only a single
	 * PML4/PDPTE/PDE.  Sanity check that the guest size is a multiple of the
	 * page size and will fit in a single PDE (512 entries).
	 */
	assert((guest_size & (GUEST_PAGE_SIZE - 1)) == 0);
	assert(guest_size <= (GUEST_PAGE_SIZE * 512));

	memset(pml4, 0x00, 4096);
	memset(pdpte, 0x00, 4096);
	memset(pde, 0x00, 4096);

	*pml4 = BOOT_PDPTE | (X86_PDPT_P | X86_PDPT_RW);
	*pdpte = BOOT_PDE | (X86_PDPT_P | X86_PDPT_RW);
	for (paddr = 0; paddr < guest_size; paddr += GUEST_PAGE_SIZE, pde++)
		*pde = paddr | (X86_PDPT_P | X86_PDPT_RW | X86_PDPT_PS);

	sregs->cr3 = BOOT_PML4;
	sregs->cr4 |= X86_CR4_PAE;
	sregs->cr0 |= X86_CR0_PG;
}

static void setup_system_gdt(struct kvm_sregs *sregs,
                             uint8_t *mem,
                             uint64_t off)
{
	uint64_t *gdt = (uint64_t *) (mem + off);
	struct kvm_segment data_seg, code_seg;

	/* flags, base, limit */
	gdt[BOOT_GDT_NULL] = GDT_ENTRY(0, 0, 0);
	gdt[BOOT_GDT_CODE] = GDT_ENTRY(0xA09B, 0, 0xFFFFF);
	gdt[BOOT_GDT_DATA] = GDT_ENTRY(0xC093, 0, 0xFFFFF);

	sregs->gdt.base = off;
	sregs->gdt.limit = (sizeof(uint64_t) * BOOT_GDT_MAX) - 1;

	GDT_TO_KVM_SEGMENT(code_seg, gdt, BOOT_GDT_CODE);
	GDT_TO_KVM_SEGMENT(data_seg, gdt, BOOT_GDT_DATA);

	sregs->cs = code_seg;
	sregs->ds = data_seg;
	sregs->es = data_seg;
	sregs->fs = data_seg;
	sregs->gs = data_seg;
	sregs->ss = data_seg;
}

static void setup_system(int vcpufd, uint8_t *mem)
{
	struct kvm_sregs sregs;

	kvm_ioctl(vcpufd, KVM_GET_SREGS, &sregs);

	/* Set all cpu/mem system structures */
	setup_system_gdt(&sregs, mem, BOOT_GDT);
	setup_system_page_tables(&sregs, mem);
	setup_system_64bit(&sregs);

	kvm_ioctl(vcpufd, KVM_SET_SREGS, &sregs);
}


static void setup_cpuid(int kvm, int vcpufd)
{
	struct kvm_cpuid2 *kvm_cpuid;
	int max_entries = 100;

	kvm_cpuid = calloc(1, sizeof(*kvm_cpuid) + max_entries * sizeof(*kvm_cpuid->entries));
	kvm_cpuid->nent = max_entries;
	kvm_ioctl(kvm, KVM_GET_SUPPORTED_CPUID, kvm_cpuid);

	filter_cpuid(kvm_cpuid);

	kvm_ioctl(vcpufd, KVM_SET_CPUID2, kvm_cpuid);
}

static void* vcpu_loop(int vcpufd, struct kvm_run *run)
{
	int ret;

	while (!done) {
		ret = ioctl(vcpufd, KVM_RUN, NULL);
		if(ret == -1) {
			switch(errno) {
			case EINTR:
				continue;

			case EFAULT: {
				struct kvm_regs regs;
				kvm_ioctl(vcpufd, KVM_GET_REGS, &regs);
				err(1, "KVM: host/guest translation fault: rip=0x%llx", regs.rip);
			}

			default:
				err(1, "KVM: ioctl KVM_RUN in vcpu_loop failed");
				break;
			}
		}

		/* handle requests */
		switch (run->exit_reason) {
		case KVM_EXIT_HLT:
			fprintf(stderr, "Guest has halted the CPU, this is considered as a normal exit.\n");
			return NULL;

		case KVM_EXIT_MMIO:
			err(1, "KVM: unhandled KVM_EXIT_MMIO at 0x%llx", run->mmio.phys_addr);
			break;

		case KVM_EXIT_IO:
			//printf("port 0x%x\n", run->io.port);
			switch (run->io.port) {
			case UHYVE_PORT_WRITE: {
					unsigned data = *((unsigned*)((size_t)run+run->io.data_offset));
					uhyve_write_t* uhyve_write = (uhyve_write_t*) (guest_mem+data);

					uhyve_write->len = write(uhyve_write->fd, guest_mem+(size_t)uhyve_write->buf, uhyve_write->len);
					break;
				}

			case UHYVE_PORT_READ: {
					unsigned data = *((unsigned*)((size_t)run+run->io.data_offset));
					uhyve_read_t* uhyve_read = (uhyve_read_t*) (guest_mem+data);

					uhyve_read->ret = read(uhyve_read->fd, guest_mem+(size_t)uhyve_read->buf, uhyve_read->len);
					break;
				}

			case UHYVE_PORT_EXIT: {
					unsigned data = *((unsigned*)((size_t)run+run->io.data_offset));

					done = 1;
					exit(*(int*)(guest_mem+data));
					break;
				}

			case UHYVE_PORT_OPEN: {
					unsigned data = *((unsigned*)((size_t)run+run->io.data_offset));
					uhyve_open_t* uhyve_open = (uhyve_open_t*) (guest_mem+data);

					uhyve_open->ret = open((const char*)guest_mem+(size_t)uhyve_open->name, uhyve_open->flags, uhyve_open->mode);
					break;
				} 

			case UHYVE_PORT_CLOSE: {
					unsigned data = *((unsigned*)((size_t)run+run->io.data_offset));
					uhyve_close_t* uhyve_close = (uhyve_close_t*) (guest_mem+data);

					if (uhyve_close->ret > 2)
						uhyve_close->ret = close(uhyve_close->fd);
					break;
				}

			case UHYVE_PORT_LSEEK: {
					unsigned data = *((unsigned*)((size_t)run+run->io.data_offset));
					uhyve_lseek_t* uhyve_lseek = (uhyve_lseek_t*) (guest_mem+data);

					uhyve_lseek->offset = lseek(uhyve_lseek->fd, uhyve_lseek->offset, uhyve_lseek->whence); 
					break;
				}
			default:
				err(1, "KVM: unhandled KVM_EXIT_IO at port 0x%x, direction %d", run->io.port, run->io.direction);
				break;
			}
			break;

		case KVM_EXIT_FAIL_ENTRY:
			err(1, "KVM: entry failure: hw_entry_failure_reason=0x%llx",
				run->fail_entry.hardware_entry_failure_reason);
			break;

		case KVM_EXIT_INTERNAL_ERROR:
			err(1, "KVM: internal error exit: suberror = 0x%x", run->internal.suberror);
			break;

		case KVM_EXIT_SHUTDOWN:
			err(1, "KVM: receive shutdown command");
			break;

		default:
			fprintf(stderr, "KVM: unhandled exit: exit_reason = 0x%x\n", run->exit_reason);
			exit(EXIT_FAILURE);
		}
	}

	return NULL;
}

static void* uhyve_thread(void* arg)
{
	size_t mmap_size;
	struct kvm_run *run;
	int vcpu_fd;

	struct kvm_regs vcpu_regs = (struct kvm_regs) {
		.rflags = 0x2	// required by x86 architecture
	};


	/* Map the shared kvm_run structure and following data. */
	mmap_size = (size_t) kvm_ioctl(kvm_vm.kvm_fd, KVM_GET_VCPU_MMAP_SIZE, NULL);

	if (mmap_size < sizeof(*run))
		err(1, "KVM: invalid VCPU_MMAP_SIZE: %zd", mmap_size);

	int cpu_number = (int) arg;
	printf("Init vcpu %i\n", cpu_number);

	vcpu_fd = kvm_ioctl(kvm_vm.vm_fd, KVM_CREATE_VCPU, cpu_number);

	/* Setup registers and memory. */
	setup_system(vcpu_fd, guest_mem);

	if(cpu_number == 0) {
		// set instruction pointer only for first CPU
		vcpu_regs.rip = elf_entry;
	}

	kvm_ioctl(vcpu_fd, KVM_SET_REGS, &vcpu_regs);

	// map kvm_run structure into user-space
	run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);
	if (run == MAP_FAILED)
		err(1, "KVM: VCPU mmap failed");

	setup_cpuid(kvm_vm.kvm_fd, vcpu_fd);

	return vcpu_loop(vcpu_fd, run);
}



int init_uhyve(char *path)
{	
	// register routine to close the VM
	atexit(uhyve_exit);

	char* mem_str = getenv("HERMIT_MEM");
	if (mem_str)
		printf("We want to use %s memory\n", mem_str);

	kvm_vm.kvm_fd = open("/dev/kvm", O_RDWR | O_CLOEXEC);
	if (kvm_vm.kvm_fd < 0)
		err(1, "Could not open: /dev/kvm");

	/* Make sure we have the stable version of the API */
	int kvm_api_version = kvm_ioctl(kvm_vm.kvm_fd, KVM_GET_API_VERSION, NULL);
	if (kvm_api_version != 12)
		err(1, "KVM: API version is %d, uhyve requires version 12", kvm_api_version);

	/* Create the virtual machine */
	kvm_vm.vm_fd = kvm_ioctl(kvm_vm.kvm_fd, KVM_CREATE_VM, 0);

	// TODO: we have to create a gap  for PCI
	assert(guest_size < KVM_32BIT_GAP_SIZE);

	/* Allocate page-aligned guest memory. */
	guest_mem = mmap(NULL, guest_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (guest_mem == MAP_FAILED)
		err(1, "mmap failed");


	/* Map it to the second page frame (to avoid the real-mode IDT at 0). */
	struct kvm_userspace_memory_region kvm_region = {
		.slot = 0,
		.guest_phys_addr = GUEST_OFFSET,
		.memory_size = guest_size,
		.userspace_addr = (uint64_t) guest_mem,
	};

	kvm_ioctl(kvm_vm.vm_fd, KVM_SET_USER_MEMORY_REGION, &kvm_region);
	kvm_ioctl(kvm_vm.vm_fd, KVM_CREATE_IRQCHIP, NULL);

	long cpus = 1;
	int max_vcpus = kvm_ioctl(kvm_vm.kvm_fd, KVM_CHECK_EXTENSION, KVM_CAP_MAX_VCPUS);

	char* cpus_str = getenv("HERMIT_CPUS");
	if(cpus_str)
		cpus = strtol(cpus_str, NULL, 10);

	if(cpus > max_vcpus)
		err(1, "More CPUs requested (%lu) than available: %i\n", cpus, max_vcpus);

	// load HermitCore application into memory
	if (load_kernel(guest_mem, path, (uint32_t) cpus) != 0)
		exit(EXIT_FAILURE);

	pthread_t vcpu_threads[cpus];

	for(int i = 0; i < cpus; i++) {
		pthread_create(&vcpu_threads[i], NULL, uhyve_thread, (void*) i);
	}

	for(int i = 0; i < cpus; i++) {
		pthread_join(vcpu_threads[i], NULL);
	}

	exit(EXIT_SUCCESS);
	return 0;
}
