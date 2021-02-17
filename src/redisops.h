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

#ifndef __REDIS_OPERATIONS__
#define __REDIS_OPERATIONS__

#include <stdint.h>
#include "pouch.h"

void push_random_numbers(int, chaos_t[]);
int doConnect(char *, unsigned short);
void doDisconnect(void);
void set_list_count(uint32_t);
void set_list_size(uint32_t);

#endif
