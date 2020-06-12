#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int g_shutdown = 0;
uint64_t gettime_count = 0;
struct timespec ts;

static void alarm_handler(int signum)
{
	printf("%lu\n", gettime_count);
	fflush(stdout);
	gettime_count = 0;
	(void) alarm(1);
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

int main(int argc, char *argv[])
{

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	signal(SIGALRM, alarm_handler);
	(void) alarm(1);

	while(!g_shutdown) { clock_gettime(CLOCK_MONOTONIC_RAW, &ts); gettime_count++; }

	return 0;
}
