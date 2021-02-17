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

#ifndef __POT__
#define __POT__

#define HASHCOUNT (5)
#define INTSPERHASH (64/4) // 16 UINTs per digest
#define BYTESPERINT (10+1) // 10 digits + 1 newline

typedef struct {
	unsigned char *wp_ptr;
	unsigned char *sha2_ptr;
	unsigned char *sha3_ptr;	// 1.7.0
	unsigned char *strb_ptr;
	unsigned char *blk2b_ptr;	// 1.8.0
} digests_t;

#endif
