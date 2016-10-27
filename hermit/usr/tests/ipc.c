/*
 * Copyright (c) 2016, Daniel Krebs, RWTH Aachen University
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
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR AN
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
//#include <stdint.h>
//#include <signal.h>
#include <pthread.h>
//#include <syscall.h>

//#include <asm/stddef.h>
#include <asm/processor.h>
#include <asm/apic.h>
#include <asm/irq.h>
#include <hermit/signal.h>

#define IPI_IRQ (83)
#define RUNS	(100)

static pthread_barrier_t barrier;
static pthread_barrierattr_t attr;

volatile int start_measurement = 0;

volatile int ipi_caught = 0;
volatile int signal_caught = 0;

volatile uint64_t time_start, time_end;
volatile int64_t time_diff;

volatile int sink_alive = 1;
volatile unsigned int sink_core = -1;
volatile tid_t sink_tid = -1;

static int ipi_average = 0;
static int signal_os_average = 0;
static int signal_newlib_average = 0;

static void raw_ipi_handler(struct state* s)
{
	(void) s;
	time_end = rdtscp(NULL);

	ipi_caught = 1;
}

static void signal_handler(int signum)
{
	(void) signum;
	time_end = rdtscp(NULL);


	for(int i = 0; i < 100000; i++) {
		asm("");
	}

	signal_caught = 1;

//	printf("stop\n");
}

static void* signal_sink(void* arg)
{
	(void) arg;
	printf("[%d] Hello from signal_sink()\n", CORE_ID);

//	irq_install_handler(IPI_IRQ, raw_ipi_handler);
	hermit_signal(signal_handler);

	// communicate core id and tid to main thread
	sink_core = CORE_ID;
	sink_tid = sys_getpid();

	printf("[%d] My task id is: %d\n", CORE_ID, sink_tid);

	pthread_barrier_wait(&barrier);

	while(sink_alive) {

		// loop until IPI sending is started by other core
		while(!start_measurement) {
			time_start = rdtscp(NULL);
		}
//		printf("start\n");

		start_measurement = 0;

		// synchronize both threads again
		pthread_barrier_wait(&barrier);
	}

	printf("[%d] I'm done here, bye\n", CORE_ID);

	return NULL;
}

int main(int argc, const char* argv[])
{
	(void) argc;
	(void) argv;


}

int main_thread(void* arg)
{
	(void) arg;

	pthread_barrier_init(&barrier, &attr, 2);

	printf("[%d] Welcome to IPI latency benchmark\n", CORE_ID);

	pthread_t signal_sink_proc;
	pthread_create(&signal_sink_proc, NULL, signal_sink, NULL);

	printf("[%d] Waiting for signal_sink() to start\n", CORE_ID);
	pthread_barrier_wait(&barrier);

	if(sink_core == CORE_ID) {
		printf("Please allocate at least 2 cores\n");
		return -1;
	}

//	printf("[%d] Measuring IPI latency:\n", CORE_ID);
//	for(int i = 0; i < RUNS; i++) {
//		ipi_caught = 0;

//		start_measurement = 1;
//		apic_send_ipi(sink_core, IPI_IRQ);

//		// wait for IPI on other core
//		while(!ipi_caught);

//		time_diff = (int64_t)time_end - (int64_t)time_start;
//		printf("[%d]     %lu cycles\n", CORE_ID, time_diff);

//		ipi_average += time_diff;

//		// synchronize both threads again
//		pthread_barrier_wait(&barrier);
//	}
//	ipi_average /= RUNS;
//	printf("[%d]   average: %d cycles\n\n", CORE_ID, ipi_average);


	printf("[%d] Measuring OS signal latency:\n", CORE_ID);
	for(int i = 0; i < RUNS; i++) {

		sys_msleep(100);
		signal_caught = 0;
		sys_msleep(100);

		start_measurement = 1;
		hermit_kill(sink_tid, 42);

		// wait for signal to be handled on other core
		while(!signal_caught);

		time_diff = (int64_t)time_end - (int64_t)time_start;
		printf("[%d] OS signal latency was %lu cycles\n", CORE_ID, time_diff);
		printf("[%d] start: %9lu stop %9lu\n", CORE_ID, time_start, time_end);
		signal_os_average += time_diff;

		pthread_barrier_wait(&barrier);
	}
	printf("[%d]   average: %d cycles\n\n", CORE_ID, signal_os_average);

//	printf("[%d] Measuring Newlib signal latency:\n", CORE_ID);
//	for(int i = 0; i < RUNS; i++) {
//		signal_caught = 0;

//		time_start = rdtscp(NULL);
//		hermit_kill(sink_tid, 42);

//		// wait for signal to be handled on other core
//		while(!signal_caught);

//		time_diff = time_end - time_start;
//		printf("[%d] OS signal latency was %lu cycles\n", CORE_ID, time_diff);

//		signal_os_average += time_diff;
//	}
//	printf("[%d]   average: %d cycles\n\n", CORE_ID, signal_newlib_average);

	sys_msleep(3000);

	sink_alive = 0;

	return 0;
}
