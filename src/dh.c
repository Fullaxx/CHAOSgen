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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define NEED_LIBGCRYPT_VERSION "1.8.0"
#include "my_gcry_help.h"
#include "pouch.h"
#include "getopts.h"

void parse_args(int argc, char **argv);
char* transmute_1(uint8_t *buf);
char* transmute_2(uint8_t *buf);

int g_shutdown = 0;
int g_chaos_threads = 1;
long g_chaos_count = 1;

#ifdef STATISTICS
#include "pot.h"
uint64_t g_chaos_collected = 0;
extern uint64_t g_clock_gettime_called;

static void alarm_handler(int signum)
{
	char *units;
	double amt;
	uint64_t l_uintspersec, l_bytespersec, l_bytesperhr;
	uint64_t l_clock = g_clock_gettime_called;
	uint64_t l_chaos = g_chaos_collected;

	g_clock_gettime_called = 0;
	g_chaos_collected = 0;

	units = NULL;
	printf("clock_gettime called: %lu\n", l_clock);
	l_uintspersec = l_chaos*HASHCOUNT*INTSPERHASH;
	l_bytespersec = l_uintspersec*BYTESPERINT;
	l_bytesperhr = l_bytespersec*3600;
	if(l_bytesperhr > 1000000000) {
		units = "GB";
		amt = l_bytesperhr/1000000000.0;
	} else if(l_bytesperhr > 1000000) {
		units = "MB";
		amt = l_bytesperhr/1000000.0;
	} else if(l_bytesperhr > 1000) {
		units = "KB";
		amt = l_bytesperhr/1000.0;
	} else {
		printf("chaos: %lu (%lu n/s) [%lu B/hr]\n", l_chaos, l_uintspersec, l_bytesperhr);
	}
	if(units) { printf("chaos: %lu (%lu n/s) [%3.3f %s/hr]\n", l_chaos, l_uintspersec, amt, units); }
	printf("\n");
	fflush(stdout);
	(void) alarm(1);
}
#endif

static void shutdown_message(char *what)
{
	g_shutdown = 1;
	usleep(10000);
	fprintf(stderr, "%s failure!\n", what);
	exit(1);
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

static void* collect_chaos(void *p)
{
	chaos_t s;

	while(!g_shutdown) {
		memset(&s, 0, sizeof(s));
		get_chaos(&s);
		if(s.entropy) {
			//s.numbers = transmute_1(s.entropy);
			s.numbers = transmute_2(s.entropy);

#ifdef STATISTICS
			g_chaos_collected++;
#else
			if(g_chaos_count-- > 0) {
				printf("%s", s.numbers);
			} else { g_shutdown = 1; }
#endif

			free(s.entropy);
			free(s.numbers);
		}

	}

	return NULL;
}

static int start_collection(void)
{
	int err;
	pthread_attr_t attr;
	pthread_t thr_id;

	err = pthread_attr_init(&attr);
	if(err) { perror("pthread_attr_init()"); return -1; }

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(err) { perror("pthread_attr_setdetachstate()"); return -2; }

	while(g_chaos_threads-- > 0) {
		err = pthread_create(&thr_id, &attr, &collect_chaos, NULL);
		if(err) { perror("pthread_create()"); return -3; }
		(void)pthread_setname_np(thr_id, "collect_chaos");
	}

	return 0;
}

#ifndef STATISTICS
static void print_dh_header(void)
{
	printf("#==================================================================\n");
	printf("# generator CHAOSgen  seed = 0000000000 \n");
	printf("#==================================================================\n");
	printf("type: d\n");
	printf("count: %ld\n", (64/4)*5*g_chaos_count);
	printf("numbit: 32\n");
}
#endif

int main(int argc, char *argv[])
{
	int err;

	parse_args(argc, argv);
	my_libgcrypt_init(NEED_LIBGCRYPT_VERSION);

	err = start_your_engines(0);
	if(err) { shutdown_message("start_your_engines()"); }

#ifndef STATISTICS
	print_dh_header();
#endif

	err = start_collection();
	if(err) { shutdown_message("start_collection()"); }

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
#ifdef STATISTICS
	signal(SIGALRM, alarm_handler);
	(void) alarm(1);
#endif

	while(!g_shutdown) {
		usleep(100);
	}

	usleep(10000);	// Let the thread(s) catch and die
	return 0;
}

struct options opts[] = 
{
	{ 1, "cores",	"the number of hashing cores to use",	"n",	1 },
#ifndef STATISTICS
	{ 2, "chaos",	"the amount of chaos you want",			"t",	1 },
#endif
	{ 0, NULL,		NULL,									NULL,	0 }
};

void parse_args(int argc, char **argv)
{
	int c;
	char *args;

	while ((c = getopts(argc, argv, opts, &args)) != 0) {
		switch(c) {
			case -2:
				// Special Case: Recognize options that we didn't set above.
				fprintf(stderr, "Unknown Getopts Option: %s\n", args);
				break;
			case -1:
				// Special Case: getopts() can't allocate memory.
				fprintf(stderr, "Unable to allocate memory for getopts().\n");
				exit(EXIT_FAILURE);
				break;
			case 1:
				g_chaos_threads = atoi(args);
				break;
#ifndef STATISTICS
			case 2:
				g_chaos_count = atol(args);
				break;
#endif
			default:
				fprintf(stderr, "Unexpected getopts Error! (%d)\n", c);
				break;
		}

		//This free() is required since getopts() automagically allocates space for "args" everytime it's called.
		free(args);
	}

	if(g_chaos_threads < 1) {
		fprintf(stderr, "g_chaos_threads must be > 0! (Fix with -n)\n");
		exit(1);
	}
}
