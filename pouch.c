#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

#include "pouch.h"

#ifdef STATISTICS
uint64_t g_clock_gettime_called = 0;
#endif

extern int g_shutdown;

uint64_t stone = 0;
struct timespec ts = { 0, 0 };
int siphon_locked = 1;

PCT pc = 0;
PBT pouch[PSIZE];
uint64_t pouch_roll = 0;
uint64_t new_data_available = 0;
pthread_mutex_t plock = PTHREAD_MUTEX_INITIALIZER;

static void siphon(void)
{
	uint32_t path, stone_lo, nsec_lo;
	PBT pval;

	if(siphon_locked) return;

	path = (stone + ts.tv_nsec) % 3;
	if(path == 0) {
		stone_lo = stone & 0x00000FFF;
		nsec_lo = ts.tv_nsec & 0x000FFFFF;
		pval = (stone_lo << (5*4)) | (nsec_lo);
	}
	if(path == 1) {
		stone_lo = stone & 0xFFFF;
		nsec_lo = ts.tv_nsec & 0xFFFF;
		pval = (stone_lo << (4*4)) | (nsec_lo);
	}
	if(path == 2) {
		stone_lo = stone & 0x000FFFFF;
		nsec_lo = ts.tv_nsec & 0x00000FFF;
		pval = (stone_lo << (3*4)) | (nsec_lo);
	}

/*
	printf("%lu %lu.%9lu [0x%04X/0x%04X]\n",
		stone, ts.tv_sec, ts.tv_nsec, stone_lo, nsec_lo);
*/

	// pval gets saved in the pouch
	pouch[pc++] = pval;
	if(pc == 0) { pouch_roll++; }

	// Anytime we get here we have new data (after the first full pouch)
	// The pouch can be hashed once
	if(pouch_roll > 0) {
		new_data_available++;
	}
}

/*
CLOCK_MONOTONIC
  A  nonsettable system-wide clock that represents monotonic time since--as described by POSIX--"some un-
  specified point in the past".  On Linux, that point corresponds to the number of seconds that the system
  has been running since it was booted.

  The CLOCK_MONOTONIC clock is not affected by discontinuous jumps in the system time (e.g., if the system
  administrator manually changes the clock), but is affected by the incremental adjustments  performed  by
  adjtime(3)  and  NTP.  This clock does not count time that the system is suspended.  All CLOCK_MONOTONIC
  variants guarantee that the time returned by consecutive calls will not  go  backwards,  but  successive
  calls may--depending on the architecture--return identical (not-increased) time values.

CLOCK_MONOTONIC_RAW (since Linux 2.6.28; Linux-specific)
  Similar to CLOCK_MONOTONIC, but provides access to a raw hardware-based time that is not  subject
  to  NTP  adjustments or the incremental adjustments performed by adjtime(3).  This clock does not
  count time that the system is suspended.
*/
static void* time_thread(void *p)
{
	struct timespec lts = { 0, 0 };

	while(!g_shutdown) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &lts);
		if(lts.tv_nsec != ts.tv_nsec) {
			// Make sure we got a new value
			ts.tv_sec  = lts.tv_sec;
			ts.tv_nsec = lts.tv_nsec;
			siphon(); // Continuously update the pouch
		}

#ifdef STATISTICS
		g_clock_gettime_called++;
#endif

	}
	return (NULL);
}

static void* int_thread(void *p)
{
	while(stone < 1e9) { stone++; }
	siphon_locked = 0;

/*
#ifdef STATISTICS
	printf("siphon_unlocked\n");
#endif
*/

	while(!g_shutdown) {
		stone++;
	}
	return (NULL);
}

// Start 2 independent threads, seen above
int start_your_engines(void)
{
	pthread_t thr_id;

	if( pthread_create(&thr_id, NULL, &int_thread, NULL) ) { return -1; }
	if( pthread_detach(thr_id) ) { return -1; }

	if( pthread_create(&thr_id, NULL, &time_thread, NULL) ) { return -1; }
	if( pthread_detach(thr_id) ) { return -1; }

	return 0;
}

// ABOVE IS SELF-CONTAINED MULTITHREAD CODE
////////////////////////////////////////
// BELOW IS CALLED FROM ORIGINAL THREAD

// buf must be of size CHAOSSIZE
int get_chaos(uint8_t *buf)
{
#ifndef NO_POUCH_RAND_START
	int n;
	PCT i, start;
#endif

	// Only 1 thread in here at a time
	// Only hand out the same chaos once
	pthread_mutex_lock(&plock);

	while(new_data_available == 0) {
		// check for shutdown flag while we wait for data to become available
		if(g_shutdown) { pthread_mutex_unlock(&plock); return 0; }
		sched_yield();	// Evidently this is more necessary that originally thought ...
	}

#ifdef NO_POUCH_RAND_START
	memcpy(buf, &pouch[0], CHAOSSIZE);
#else
	n = 0;
	i = start = (stone % PSIZE);
	do {
		memcpy(buf+n, &pouch[i++], sizeof(PBT));
		n += sizeof(PBT);
	} while(i != start);
#endif

	new_data_available = 0;
	pthread_mutex_unlock(&plock);
	return CHAOSSIZE;
}
