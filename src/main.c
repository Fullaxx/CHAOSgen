/*
	CHAOSgen is a random number generator based on hardware events
	Copyright (C) 2022 Brett Kuskie <fullaxx@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

int g_shutdown = 0;

uint64_t stone = 0;
struct timespec ts;

uint8_t pc = 0;
uint32_t pouch[256];
uint64_t pouch_roll = 0;

/*
void alarm_handler(int signum)
{
	//fflush(stdout);
	//(void) alarm(1);
}
*/

#ifdef DEBUG
static void print_pouch(void)
{
	int i;
	printf("0x");
	for(i=0; i<256; i++) {
		printf("%08X", pouch[i]);
	}
	putchar('\n');
}
#endif

static inline void save(uint32_t pval)
{
	pouch[pc++] = pval;
	if(pc == 0) { pouch_roll++; putchar('\n'); }
}

// calc 'base(16); (1e9-1)' = 0x3b9ac9ff
static uint16_t siphon(void)
{
	uint16_t stone_lo, nsec_lo, skipval;
	uint32_t pval;

	stone_lo = stone & 0xFFFF;
	nsec_lo = ts.tv_nsec & 0xFFFF;
	pval = (stone_lo << 16) | (nsec_lo);

	printf("%lu %lu.%9lu [0x%04X/0x%04X]\n",
		stone, ts.tv_sec, ts.tv_nsec, stone_lo, nsec_lo);

	save(pval);
	//print_pouch();

#ifdef TEST_POUCH_VALS
	printf("entropy:%10u\n", pval);
#endif

	skipval = ((nsec_lo & 0xF0) >> 4) + (nsec_lo & 0x0F) + 1;
	return skipval;
}

static void get_entropy(void)
{
	uint8_t i, start=0;

	i = start;
	do {
		i++;
	} while(i != start);
}

static void sig_handler(int signum)
{
	switch(signum) {
		case SIGHUP:
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			g_shutdown = 1;
			break;
	}
}

static void* increment(void *p)
{
	while(!g_shutdown) { stone++; clock_gettime(CLOCK_MONOTONIC_RAW, &ts); }
	return (NULL);
}

static int launch(void)
{
	pthread_t thr_id;

	if( pthread_create(&thr_id, NULL, &increment, NULL) ) goto bail;
	if( pthread_detach(thr_id) ) goto bail;

	return 0;

bail:
	return -1;
}

int main(int argc, char *argv[])
{
	unsigned char sleepy_time;
	int16_t skip;

	launch();

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	//signal(SIGALRM, alarm_handler);
	//(void) alarm(1);

	// Let the stone roll for a bit ...
	putchar('.'); fflush(stdout); sleep(1);
	putchar('.'); fflush(stdout); sleep(1);
	putchar('.'); fflush(stdout); sleep(1);
	putchar('.'); fflush(stdout); sleep(1);
	putchar('.'); fflush(stdout); sleep(1);
	putchar('\n');

	// Pick our first skip value
	skip = (stone & 0x0FFF);

	while(!g_shutdown) {
		sleepy_time = (stone & 0xFF);
		usleep(sleepy_time);
		if(skip <= 0) { skip = siphon(); get_entropy(); }
		skip--;
	}

	usleep(1000);	// Let the thread catch and die
	return 0;
}
