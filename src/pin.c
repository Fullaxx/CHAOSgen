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
#include <sched.h>
#include <errno.h>

int pinme(int cpuid)
{
	int err;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpuid, &mask);

	//err = sched_setaffinity(getpid(), sizeof(mask), &mask);
	//err = sched_setaffinity(gettid(), sizeof(mask), &mask);
	err = sched_setaffinity(0, sizeof(mask), &mask);
	if(err) { fprintf(stderr, "sched_setaffinity(0) error: %s\n", strerror(errno)); }
	return err;
}
