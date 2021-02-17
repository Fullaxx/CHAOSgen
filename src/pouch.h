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

#ifndef __POUCH__
#define __POUCH__

#include <stdint.h>

// Pouch Counter Type
// used for indexing pouch correctly
#ifdef LARGEPOUCH
// BE CAREFUL WHAT YOU WISH FOR
typedef uint16_t PCT;
#else
typedef uint8_t PCT;
#endif

// LARGEPOUCH 65536, otherwise 256
#define PSIZE (1<<(8*sizeof(PCT)))

// Pouch Block Type
// Pouch holds PSIZE amount of PBTs
typedef uint32_t PBT;

// the amount of bytes given up in each call to get_chaos()
#define CHAOSSIZE (PSIZE*sizeof(PBT))

typedef struct {
	uint8_t *entropy;
	char *numbers;
} chaos_t;

int start_your_engines(uint64_t saveacore);
void get_chaos(chaos_t *s);
// void get_chaos_amt(int, chaos_t[]);

#endif
