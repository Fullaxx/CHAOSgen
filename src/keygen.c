/*
	CHAOSgen is a random number generator based on hardware events
	Copyright (C) 2021 Brett Kuskie <fullaxx@gmail.com>

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
long g_filesize = 0;
char *g_filename = NULL;

FILE *f = NULL;
pthread_mutex_t fwlock = PTHREAD_MUTEX_INITIALIZER;

/*
static void alarm_handler(int signum)
{
	(void) alarm(1);
}
*/

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

static void write_binary_entropy(char *numbers)
{
	char *token, *saveptr;
	uint32_t rval;

	// Only 1 thread in here at a time
	pthread_mutex_lock(&fwlock);

	if((g_shutdown) || (g_filesize <= 0)) {
		// If we are done, bail out
		pthread_mutex_unlock(&fwlock);
		return;
	}

	token = strtok_r(numbers, "\n", &saveptr);
	while(token) {
		rval = strtoul(token, NULL, 10);
		//printf("%s: %u\n", token, rval);
		if(g_filesize > 4) {
			fwrite(&rval, 4, 1, f);
			g_filesize -= 4;
		} else if(g_filesize > 0) {
			fwrite(&rval, g_filesize, 1, f);
			g_filesize = 0;
		}

		if(g_filesize <= 0) { g_shutdown = 1; break; }
		token = strtok_r(NULL, "\n", &saveptr);
	}

	pthread_mutex_unlock(&fwlock);
}

static void* collect_chaos(void *p)
{
	chaos_t s;

	while(g_shutdown == 0) {
		memset(&s, 0, sizeof(s));
		get_chaos(&s);
		if(s.entropy) {
			//s.numbers = transmute_1(s.entropy);
			s.numbers = transmute_2(s.entropy);
			write_binary_entropy(s.numbers);
			free(s.entropy);
			free(s.numbers);
		}
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	int err;
	pthread_t thr_id;

	parse_args(argc, argv);
	my_libgcrypt_init(NEED_LIBGCRYPT_VERSION);

	f = fopen(g_filename, "w");
	if(!f) {
		fprintf(stderr, "fopen(%s, w) failed\n", g_filename);
		exit(1);
	}

	err = start_your_engines(0);
	if(err) { shutdown_message("start_your_engines()"); }

	while(g_chaos_threads-- > 0) {
		if( pthread_create(&thr_id, NULL, &collect_chaos, NULL) ) { shutdown_message("pthread_create()"); }
		(void)pthread_setname_np(thr_id, "collect_chaos");
		if( pthread_detach(thr_id) )  { shutdown_message("pthread_detach()"); }
	}

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	//signal(SIGALRM, alarm_handler);
	//(void) alarm(1);

	// Wait for the sweet release of death
	while(!g_shutdown) {
		usleep(100);
	}

	// Let the thread(s) catch and die
	usleep(10000);
	if(g_filename) { free(g_filename); }
	fclose(f);
	return 0;
}

struct options opts[] = 
{
	{ 1, "cores",	"the number of hashing cores to use",	"n",	1 },
	{ 2, "name",	"the filename of the key to make",		"f",	1 },
	{ 3, "size",	"the size in Bytes to make the key",	"b",	1 },
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
			case 2:
				g_filename = strdup(args);
				break;
			case 3:
				g_filesize = strtol(args, NULL, 10);
				break;
			default:
				fprintf(stderr, "Unexpected getopts Error! (%d)\n", c);
				break;
		}

		//This free() is required since getopts() automagically allocates space for "args" everytime it's called.
		free(args);
	}

	if(!g_filename) {
		fprintf(stderr, "I need a filename to make! (Fix with -f)\n");
		exit(1);
	}

	if(g_filesize <= 0) {
		fprintf(stderr, "How many bytes in this file! (Fix with -b)\n");
		exit(1);
	}
}
