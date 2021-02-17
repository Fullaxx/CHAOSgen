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
