#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

int g_shutdown = 0;
uint64_t stone = 0;
uint64_t last = 0;

static void alarm_handler(int signum)
{
	printf("%lu", stone);
	if(last > 0) { printf(" (%lu)", stone-last); }
	printf("\n");
	last = stone;
	fflush(stdout);
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

	while(!g_shutdown) { stone++; }

	return 0;
}
