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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "getopts.h"
#include "chaos_srv_ops.h"

static void parse_args(int argc, char **argv);

int g_shutdown = 0;

char *g_rsock = NULL;
char *g_rip = NULL;
unsigned short g_rport = 0;

srv_opts_t g_so;

int shutting_down(void) { return g_shutdown; }

#ifdef SRNODECHRONOMETRY
int g_alarm_stats = 0;
void print_avg_nodecb_time(void);
#endif

static void alarm_handler(int signum)
{
	(void) alarm(1);
}

static void sig_handler(int signum)
{
	switch(signum) {
		case SIGPIPE:
			fprintf(stderr, "SIGPIPE\n");
			break;
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
	int z;

	memset(&g_so, 0, sizeof(srv_opts_t));
	g_so.maxqty = 1000000;
	parse_args(argc, argv);

	if(g_rsock) {
		g_so.rdest = g_rsock;
		chaos_srv_start(&g_so);
	} else if(g_rip && (g_rport > 0)) {
		g_so.rdest = g_rip;
		g_so.rport = g_rport;
		chaos_srv_start(&g_so);
	} else {
		fprintf(stderr, "webstore_start() failed!\n");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	signal(SIGPIPE,	sig_handler);
	signal(SIGALRM, alarm_handler);
	(void) alarm(1);

	z=0;	// Wait for the sweet release of death
	while(!g_shutdown) {
		if(z>999) { z=0; }
		usleep(1000);
		z++;
	}
	usleep(1000);

	// Stop Services
	chaos_srv_stop();

	// OCD requires this
	if(g_rsock) { free(g_rsock); }
	if(g_rip) { free(g_rip); }
	if(g_so.http_ip) { free(g_so.http_ip); }
	if(g_so.certfile) { free(g_so.certfile); }
	if(g_so.keyfile) { free(g_so.keyfile); }
	return 0;
}

struct options opts[] = 
{
	{ 1, "ip",		"HTTP IP to bind to",			"I",  1 },
	{ 2, "port",	"HTTP Port to bind to",			"P",  1 },
	{ 3, "tpc",		"UHD Multithread",				"t",  0 },
	{ 5, "rsock",	"Connect to Redis file socket",	NULL, 1 },
	{ 6, "rtcp",	"Connect to Redis over tcp",	NULL, 1 },
	{ 7, "cert",	"Use this HTTPS cert",			NULL, 1 },
	{ 8, "key",		"Use this HTTPS key",			NULL, 1 },
#ifdef SRNODECHRONOMETRY
	{ 10, "stats",	"Show stats every second",		NULL, 0 },
#endif
	{ 0, NULL,		NULL,							NULL, 0 }
};

static void parse_args(int argc, char **argv)
{
	char *args, *colon;
	int c;

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
				g_so.http_ip = strdup(args);
				break;
			case 2:
				g_so.http_port = atoi(args);
				break;
			case 3:
				g_so.use_threads = 1;
				break;
			/*case 4:
				g_logfile = strdup(args);
				break;*/
			case 5:
				g_rsock = strdup(args);
				break;
			case 6:
				colon = strchr(args, ':');
				if(colon) {
					*colon = 0;
					g_rip = strdup(args);
					g_rport = atoi(colon+1);
				}
				break;
			case 7:
				g_so.certfile = strdup(args);
				break;
			case 8:
				g_so.keyfile = strdup(args);
				break;
#ifdef SRNODECHRONOMETRY
			case 10:
				g_alarm_stats = 1;
				break;
#endif
			default:
				fprintf(stderr, "Unexpected getopts Error! (%d)\n", c);
				break;
		}

		//This free() is required since getopts() automagically allocates space for "args" everytime it's called.
		free(args);
	}

	if(!g_rsock && !g_rip) {
		fprintf(stderr, "I need to connect to redis! (Fix with --rsock/--rtcp)\n");
		exit(EXIT_FAILURE);
	}

	if(g_rip && !g_rport) {
		fprintf(stderr, "Invalid redis tcp port! (Fix with --rsock IP:PORT)\n");
		exit(EXIT_FAILURE);
	}

	if(!g_so.http_port) {
		fprintf(stderr, "I need a port to listen on! (Fix with -P)\n");
		exit(EXIT_FAILURE);
	}

	if(g_so.certfile && !g_so.keyfile) {
		fprintf(stderr, "I need a key to enable HTTPS operations! (Fix with --key)\n");
		exit(EXIT_FAILURE);
	}

	if(!g_so.certfile && g_so.keyfile) {
		fprintf(stderr, "I need a cert to enable HTTPS operations! (Fix with --cert)\n");
		exit(EXIT_FAILURE);
	}
}
