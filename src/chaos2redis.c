#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define NEED_LIBGCRYPT_VERSION "1.8.0"
#include "my_gcry_help.h"
#include "pouch.h"
#include "redisops.h"
#include "getopts.h"

#ifdef CHRONOMETRY
#include "chronometry.h"
#endif

#define CHAOSAMTMAX (1024)

void parse_args(int argc, char **argv);
char* transmute_1(uint8_t *buf);
char* transmute_2(uint8_t *buf);

int g_shutdown = 0;
char *g_rsock = NULL;
char *g_rhost = NULL;
uint16_t g_rport = 0;
uint16_t g_chaos_threads = 1;
uint16_t g_chaos_amt = 4;

uint64_t g_chaos_collected = 0;
uint64_t g_numbers_submitted = 0;
static void alarm_handler(int signum)
{
	uint64_t l_chaos = g_chaos_collected;
	uint64_t l_num = g_numbers_submitted;
	g_chaos_collected = g_numbers_submitted = 0;
	printf("%9lu/%9lu\n", l_chaos, l_num);
	(void) alarm(1);
}

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
		case SIGPIPE:
			fprintf(stderr, "SIGPIPE\n");
		case SIGHUP:
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			g_shutdown = 1;
			break;
	}
}

static void convert_numbers(chaos_t *sp)
{
	size_t n, size;
	char *dhstr;
	char *token, *saveptr;
	uint32_t count;
	uint64_t rval;

	if(!sp->entropy) { return; }
	dhstr = transmute_2(sp->entropy);

	n = 0;
	count = 0;
	size = 64+strlen(dhstr);
	sp->numbers = malloc(size);

	token = strtok_r(dhstr, "\n", &saveptr);
	while(token) {
		rval = strtoul(token, NULL, 10);
#ifdef DEBUG
		if(rval > 0xFFFFFFFF) { fprintf(stderr, "rval > 0xFFFFFFFF\n"); exit(1); }
#endif
		n += snprintf(sp->numbers+n, size-n, " %lu", rval);
		count++;
		token = strtok_r(NULL, "\n", &saveptr);
	}

	free(dhstr);
	g_numbers_submitted += count;
}

static void* collect_chaos(void *p)
{
	int i;
	chaos_t s[CHAOSAMTMAX];

#ifdef TIME_NUM_CONVERSION
	stopwatch_t watch;
#endif

	while(g_shutdown == 0) {
		memset(&s[0], 0, sizeof(s));

		for(i=0; i<g_chaos_amt; i++) {
			get_chaos(&s[i]);
		}

#ifdef TIME_NUM_CONVERSION
		chron_start(&watch, -1);
#endif

		if(g_shutdown == 0) {
			for(i=0; i<g_chaos_amt; i++) {
				convert_numbers(&s[i]);
			}
		}

#ifdef TIME_NUM_CONVERSION
		printf("convert_numbers(): %ld\n", chron_stop(&watch));
#endif

		if(g_shutdown == 0) {
			push_random_numbers(g_chaos_amt, &s[0]);
			g_chaos_collected += g_chaos_amt;
		}

		for(i=0; i<g_chaos_amt; i++) {
			if(s[i].entropy) { free(s[i].entropy); }
			if(s[i].numbers) { free(s[i].numbers); }
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

	if(g_rport > 0) {
		err = doConnect(g_rhost, g_rport);
	} else {
		err = doConnect(g_rsock, 0);
	}
	if(err) { shutdown_message("doConnect()"); }

	err = start_your_engines();
	if(err) { shutdown_message("engine()"); }

	while(g_chaos_threads-- > 0) {
		if( pthread_create(&thr_id, NULL, &collect_chaos, NULL) ) { shutdown_message("pthread_create()"); }
		if( pthread_detach(thr_id) )  { shutdown_message("pthread_detach()"); }
	}

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	signal(SIGPIPE,	sig_handler);
	signal(SIGALRM, alarm_handler);
	(void) alarm(1);

	// Wait for the sweet release of death
	while(!g_shutdown) {
		usleep(100);
	}

	// Let the thread(s) catch and die
	usleep(10000);
	doDisconnect();
	if(g_rsock) { free(g_rsock); }
	if(g_rhost) { free(g_rhost); }
	return 0;
}

struct options opts[] = 
{
	{ 1, "rsock",	"the Redis Socket to connect to",			"S",	1 },
	{ 2, "rhost",	"the Redis Hostname to connect to",			"H",	1 },
	{ 3, "rport",	"the Redis Port number",					"P",	1 },
	{ 4, "lists",	"the number of redis lists to use",			NULL,	1 },
	{ 5, "lsize",	"the size of each redis list",				NULL,	1 },
	{ 6, "cores",	"the number of hashing cores to use",		"n",	1 },
	{ 7, "chaos",	"the amount of chaos per thread to aquire",	NULL,	1 },
	{ 0, NULL,		NULL,										NULL,	0 }
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
				g_rsock = strdup(args);
				break;
			case 2:
				g_rhost = strdup(args);
				break;
			case 3:
				g_rport = atoi(args);
				break;
			case 4:
				set_list_count(atoi(args));
				break;
			case 5:
				set_list_size(atoi(args));
				break;
			case 6:
				g_chaos_threads = atoi(args);
				break;
			case 7:
				g_chaos_amt = atoi(args);
				break;
			default:
				fprintf(stderr, "Unexpected getopts Error! (%d)\n", c);
				break;
		}

		//This free() is required since getopts() automagically allocates space for "args" everytime it's called.
		free(args);
	}

	if(!g_rsock && !g_rhost) {
		fprintf(stderr, "I need to connect to redis! (Fix with -S/-H)\n");
		exit(1);
	}

	if(g_rsock && g_rhost) {
		fprintf(stderr, "I need either a file socket or hostname! (Fix with -S/-H)\n");
		exit(1);
	}

	if(g_rhost && (g_rport == 0)) {
		fprintf(stderr, "I need a port number! (Fix with -P)\n");
		exit(1);
	}

	if((g_rport > 0) && (!g_rhost)) {
		fprintf(stderr, "I need a hostname to accompany this port number! (Fix with -H)\n");
		exit(1);
	}

	if(g_chaos_amt > CHAOSAMTMAX) {
		fprintf(stderr, "g_chaos_amt > 1024! (Fix with --chaos)\n");
		exit(1);
	}

	if(g_chaos_amt == 0) { g_chaos_amt++; }
}
