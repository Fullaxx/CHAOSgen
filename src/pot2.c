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
#include <endian.h>

#include <gcrypt.h>
#include "my_gcry_help.h"

#include "pouch.h"
#include "pot.h"

extern uint64_t stone;

// This must be free()'d
static char* collect_uints(digests_t *out)
{
	char *buf;
	int i, j, n, size;
	uint32_t *wp_ptr, *s2_ptr, *s3_ptr, *sb_ptr, *b2_ptr;
	uint32_t wp_be, s2_be, s3_be, sb_be, b2_be;
	uint32_t wp_le, s2_le, s3_le, sb_le, b2_le;

	i = n = 0;
	size = (HASHCOUNT*INTSPERHASH*BYTESPERINT)+1;
	buf = malloc(size);

	while(i < INTSPERHASH) {
		j = i*sizeof(uint32_t);
		wp_ptr = (uint32_t *)&out->wp_ptr[j];
		s2_ptr = (uint32_t *)&out->sha2_ptr[j];
		s3_ptr = (uint32_t *)&out->sha3_ptr[j];
		sb_ptr = (uint32_t *)&out->strb_ptr[j];
		b2_ptr = (uint32_t *)&out->blk2b_ptr[j];
		i++;

		wp_be = htobe32(*wp_ptr);
		s2_be = htobe32(*s2_ptr);
		s3_be = htobe32(*s3_ptr);
		sb_be = htobe32(*sb_ptr);
		b2_be = htobe32(*b2_ptr);

		wp_le = htole32(*wp_ptr);
		s2_le = htole32(*s2_ptr);
		s3_le = htole32(*s3_ptr);
		sb_le = htole32(*sb_ptr);
		b2_le = htole32(*b2_ptr);

		switch(stone % 240) {
			case   0: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s2_be, s3_be, sb_be, b2_be); break;
			case   1: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s2_be, s3_be, b2_be, sb_be); break;
			case   2: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s2_be, b2_be, sb_be, s3_be); break;
			case   3: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s2_be, b2_be, s3_be, sb_be); break;
			case   4: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s2_be, sb_be, s3_be, b2_be); break;
			case   5: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s2_be, sb_be, b2_be, s3_be); break;

			case   6: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s3_be, s2_be, sb_be, b2_be); break;
			case   7: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s3_be, s2_be, b2_be, sb_be); break;
			case   8: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s3_be, b2_be, sb_be, s2_be); break;
			case   9: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s3_be, b2_be, s2_be, sb_be); break;
			case  10: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s3_be, sb_be, s2_be, b2_be); break;
			case  11: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, s3_be, sb_be, b2_be, s2_be); break;

			case  12: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, sb_be, s2_be, s3_be, b2_be); break;
			case  13: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, sb_be, s2_be, b2_be, s3_be); break;
			case  14: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, sb_be, b2_be, s3_be, s2_be); break;
			case  15: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, sb_be, b2_be, s2_be, s3_be); break;
			case  16: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, sb_be, s3_be, s2_be, b2_be); break;
			case  17: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, sb_be, s3_be, b2_be, s2_be); break;

			case  18: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, b2_be, s2_be, s3_be, sb_be); break;
			case  19: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, b2_be, s2_be, sb_be, s3_be); break;
			case  20: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, b2_be, sb_be, s3_be, s2_be); break;
			case  21: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, b2_be, sb_be, s2_be, s3_be); break;
			case  22: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, b2_be, s3_be, s2_be, sb_be); break;
			case  23: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_be, b2_be, s3_be, sb_be, s2_be); break;

			case  24: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, wp_be, s3_be, sb_be, b2_be); break;
			case  25: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, wp_be, s3_be, b2_be, sb_be); break;
			case  26: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, wp_be, b2_be, sb_be, s3_be); break;
			case  27: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, wp_be, b2_be, s3_be, sb_be); break;
			case  28: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, wp_be, sb_be, s3_be, b2_be); break;
			case  29: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, wp_be, sb_be, b2_be, s3_be); break;

			case  30: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, s3_be, wp_be, sb_be, b2_be); break;
			case  31: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, s3_be, wp_be, b2_be, sb_be); break;
			case  32: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, s3_be, b2_be, sb_be, wp_be); break;
			case  33: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, s3_be, b2_be, wp_be, sb_be); break;
			case  34: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, s3_be, sb_be, wp_be, b2_be); break;
			case  35: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, s3_be, sb_be, b2_be, wp_be); break;

			case  36: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, sb_be, wp_be, s3_be, b2_be); break;
			case  37: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, sb_be, wp_be, b2_be, s3_be); break;
			case  38: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, sb_be, b2_be, s3_be, wp_be); break;
			case  39: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, sb_be, b2_be, wp_be, s3_be); break;
			case  40: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, sb_be, s3_be, wp_be, b2_be); break;
			case  41: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, sb_be, s3_be, b2_be, wp_be); break;

			case  42: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, b2_be, wp_be, s3_be, sb_be); break;
			case  43: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, b2_be, wp_be, sb_be, s3_be); break;
			case  44: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, b2_be, sb_be, s3_be, wp_be); break;
			case  45: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, b2_be, sb_be, wp_be, s3_be); break;
			case  46: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, b2_be, s3_be, wp_be, sb_be); break;
			case  47: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_be, b2_be, s3_be, sb_be, wp_be); break;

			case  48: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, s2_be, wp_be, sb_be, b2_be); break;
			case  49: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, s2_be, wp_be, b2_be, sb_be); break;
			case  50: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, s2_be, b2_be, sb_be, wp_be); break;
			case  51: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, s2_be, b2_be, wp_be, sb_be); break;
			case  52: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, s2_be, sb_be, wp_be, b2_be); break;
			case  53: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, s2_be, sb_be, b2_be, wp_be); break;

			case  54: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, wp_be, s2_be, sb_be, b2_be); break;
			case  55: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, wp_be, s2_be, b2_be, sb_be); break;
			case  56: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, wp_be, b2_be, sb_be, s2_be); break;
			case  57: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, wp_be, b2_be, s2_be, sb_be); break;
			case  58: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, wp_be, sb_be, s2_be, b2_be); break;
			case  59: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, wp_be, sb_be, b2_be, s2_be); break;

			case  60: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, sb_be, s2_be, wp_be, b2_be); break;
			case  61: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, sb_be, s2_be, b2_be, wp_be); break;
			case  62: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, sb_be, b2_be, wp_be, s2_be); break;
			case  63: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, sb_be, b2_be, s2_be, wp_be); break;
			case  64: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, sb_be, wp_be, s2_be, b2_be); break;
			case  65: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, sb_be, wp_be, b2_be, s2_be); break;

			case  66: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, b2_be, s2_be, wp_be, sb_be); break;
			case  67: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, b2_be, s2_be, sb_be, wp_be); break;
			case  68: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, b2_be, sb_be, wp_be, s2_be); break;
			case  69: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, b2_be, sb_be, s2_be, wp_be); break;
			case  70: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, b2_be, wp_be, s2_be, sb_be); break;
			case  71: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_be, b2_be, wp_be, sb_be, s2_be); break;

			case  72: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s2_be, s3_be, wp_be, b2_be); break;
			case  73: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s2_be, s3_be, b2_be, wp_be); break;
			case  74: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s2_be, b2_be, wp_be, s3_be); break;
			case  75: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s2_be, b2_be, s3_be, wp_be); break;
			case  76: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s2_be, wp_be, s3_be, b2_be); break;
			case  77: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s2_be, wp_be, b2_be, s3_be); break;

			case  78: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s3_be, s2_be, wp_be, b2_be); break;
			case  79: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s3_be, s2_be, b2_be, wp_be); break;
			case  80: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s3_be, b2_be, wp_be, s2_be); break;
			case  81: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s3_be, b2_be, s2_be, wp_be); break;
			case  82: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s3_be, wp_be, s2_be, b2_be); break;
			case  83: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, s3_be, wp_be, b2_be, s2_be); break;

			case  84: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, wp_be, s2_be, s3_be, b2_be); break;
			case  85: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, wp_be, s2_be, b2_be, s3_be); break;
			case  86: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, wp_be, b2_be, s3_be, s2_be); break;
			case  87: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, wp_be, b2_be, s2_be, s3_be); break;
			case  88: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, wp_be, s3_be, s2_be, b2_be); break;
			case  89: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, wp_be, s3_be, b2_be, s2_be); break;

			case  90: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, b2_be, s2_be, s3_be, wp_be); break;
			case  91: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, b2_be, s2_be, wp_be, s3_be); break;
			case  92: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, b2_be, wp_be, s3_be, s2_be); break;
			case  93: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, b2_be, wp_be, s2_be, s3_be); break;
			case  94: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, b2_be, s3_be, s2_be, wp_be); break;
			case  95: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_be, b2_be, s3_be, wp_be, s2_be); break;

			case  96: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s2_be, s3_be, sb_be, wp_be); break;
			case  97: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s2_be, s3_be, wp_be, sb_be); break;
			case  98: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s2_be, wp_be, sb_be, s3_be); break;
			case  99: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s2_be, wp_be, s3_be, sb_be); break;
			case 100: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s2_be, sb_be, s3_be, wp_be); break;
			case 101: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s2_be, sb_be, wp_be, s3_be); break;

			case 102: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s3_be, s2_be, sb_be, wp_be); break;
			case 103: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s3_be, s2_be, wp_be, sb_be); break;
			case 104: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s3_be, wp_be, sb_be, s2_be); break;
			case 105: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s3_be, wp_be, s2_be, sb_be); break;
			case 106: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s3_be, sb_be, s2_be, wp_be); break;
			case 107: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, s3_be, sb_be, wp_be, s2_be); break;

			case 108: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, sb_be, s2_be, s3_be, wp_be); break;
			case 109: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, sb_be, s2_be, wp_be, s3_be); break;
			case 110: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, sb_be, wp_be, s3_be, s2_be); break;
			case 111: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, sb_be, wp_be, s2_be, s3_be); break;
			case 112: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, sb_be, s3_be, s2_be, wp_be); break;
			case 113: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, sb_be, s3_be, wp_be, s2_be); break;

			case 114: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, wp_be, s2_be, s3_be, sb_be); break;
			case 115: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, wp_be, s2_be, sb_be, s3_be); break;
			case 116: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, wp_be, sb_be, s3_be, s2_be); break;
			case 117: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, wp_be, sb_be, s2_be, s3_be); break;
			case 118: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, wp_be, s3_be, s2_be, sb_be); break;
			case 119: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_be, wp_be, s3_be, sb_be, s2_be); break;

			case   0+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s2_le, s3_le, sb_le, b2_le); break;
			case   1+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s2_le, s3_le, b2_le, sb_le); break;
			case   2+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s2_le, b2_le, sb_le, s3_le); break;
			case   3+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s2_le, b2_le, s3_le, sb_le); break;
			case   4+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s2_le, sb_le, s3_le, b2_le); break;
			case   5+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s2_le, sb_le, b2_le, s3_le); break;

			case   6+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s3_le, s2_le, sb_le, b2_le); break;
			case   7+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s3_le, s2_le, b2_le, sb_le); break;
			case   8+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s3_le, b2_le, sb_le, s2_le); break;
			case   9+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s3_le, b2_le, s2_le, sb_le); break;
			case  10+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s3_le, sb_le, s2_le, b2_le); break;
			case  11+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, s3_le, sb_le, b2_le, s2_le); break;

			case  12+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, sb_le, s2_le, s3_le, b2_le); break;
			case  13+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, sb_le, s2_le, b2_le, s3_le); break;
			case  14+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, sb_le, b2_le, s3_le, s2_le); break;
			case  15+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, sb_le, b2_le, s2_le, s3_le); break;
			case  16+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, sb_le, s3_le, s2_le, b2_le); break;
			case  17+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, sb_le, s3_le, b2_le, s2_le); break;

			case  18+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, b2_le, s2_le, s3_le, sb_le); break;
			case  19+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, b2_le, s2_le, sb_le, s3_le); break;
			case  20+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, b2_le, sb_le, s3_le, s2_le); break;
			case  21+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, b2_le, sb_le, s2_le, s3_le); break;
			case  22+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, b2_le, s3_le, s2_le, sb_le); break;
			case  23+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", wp_le, b2_le, s3_le, sb_le, s2_le); break;

			case  24+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, wp_le, s3_le, sb_le, b2_le); break;
			case  25+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, wp_le, s3_le, b2_le, sb_le); break;
			case  26+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, wp_le, b2_le, sb_le, s3_le); break;
			case  27+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, wp_le, b2_le, s3_le, sb_le); break;
			case  28+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, wp_le, sb_le, s3_le, b2_le); break;
			case  29+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, wp_le, sb_le, b2_le, s3_le); break;

			case  30+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, s3_le, wp_le, sb_le, b2_le); break;
			case  31+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, s3_le, wp_le, b2_le, sb_le); break;
			case  32+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, s3_le, b2_le, sb_le, wp_le); break;
			case  33+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, s3_le, b2_le, wp_le, sb_le); break;
			case  34+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, s3_le, sb_le, wp_le, b2_le); break;
			case  35+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, s3_le, sb_le, b2_le, wp_le); break;

			case  36+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, sb_le, wp_le, s3_le, b2_le); break;
			case  37+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, sb_le, wp_le, b2_le, s3_le); break;
			case  38+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, sb_le, b2_le, s3_le, wp_le); break;
			case  39+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, sb_le, b2_le, wp_le, s3_le); break;
			case  40+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, sb_le, s3_le, wp_le, b2_le); break;
			case  41+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, sb_le, s3_le, b2_le, wp_le); break;

			case  42+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, b2_le, wp_le, s3_le, sb_le); break;
			case  43+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, b2_le, wp_le, sb_le, s3_le); break;
			case  44+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, b2_le, sb_le, s3_le, wp_le); break;
			case  45+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, b2_le, sb_le, wp_le, s3_le); break;
			case  46+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, b2_le, s3_le, wp_le, sb_le); break;
			case  47+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s2_le, b2_le, s3_le, sb_le, wp_le); break;

			case  48+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, s2_le, wp_le, sb_le, b2_le); break;
			case  49+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, s2_le, wp_le, b2_le, sb_le); break;
			case  50+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, s2_le, b2_le, sb_le, wp_le); break;
			case  51+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, s2_le, b2_le, wp_le, sb_le); break;
			case  52+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, s2_le, sb_le, wp_le, b2_le); break;
			case  53+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, s2_le, sb_le, b2_le, wp_le); break;

			case  54+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, wp_le, s2_le, sb_le, b2_le); break;
			case  55+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, wp_le, s2_le, b2_le, sb_le); break;
			case  56+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, wp_le, b2_le, sb_le, s2_le); break;
			case  57+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, wp_le, b2_le, s2_le, sb_le); break;
			case  58+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, wp_le, sb_le, s2_le, b2_le); break;
			case  59+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, wp_le, sb_le, b2_le, s2_le); break;

			case  60+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, sb_le, s2_le, wp_le, b2_le); break;
			case  61+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, sb_le, s2_le, b2_le, wp_le); break;
			case  62+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, sb_le, b2_le, wp_le, s2_le); break;
			case  63+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, sb_le, b2_le, s2_le, wp_le); break;
			case  64+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, sb_le, wp_le, s2_le, b2_le); break;
			case  65+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, sb_le, wp_le, b2_le, s2_le); break;

			case  66+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, b2_le, s2_le, wp_le, sb_le); break;
			case  67+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, b2_le, s2_le, sb_le, wp_le); break;
			case  68+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, b2_le, sb_le, wp_le, s2_le); break;
			case  69+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, b2_le, sb_le, s2_le, wp_le); break;
			case  70+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, b2_le, wp_le, s2_le, sb_le); break;
			case  71+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", s3_le, b2_le, wp_le, sb_le, s2_le); break;

			case  72+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s2_le, s3_le, wp_le, b2_le); break;
			case  73+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s2_le, s3_le, b2_le, wp_le); break;
			case  74+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s2_le, b2_le, wp_le, s3_le); break;
			case  75+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s2_le, b2_le, s3_le, wp_le); break;
			case  76+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s2_le, wp_le, s3_le, b2_le); break;
			case  77+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s2_le, wp_le, b2_le, s3_le); break;

			case  78+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s3_le, s2_le, wp_le, b2_le); break;
			case  79+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s3_le, s2_le, b2_le, wp_le); break;
			case  80+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s3_le, b2_le, wp_le, s2_le); break;
			case  81+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s3_le, b2_le, s2_le, wp_le); break;
			case  82+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s3_le, wp_le, s2_le, b2_le); break;
			case  83+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, s3_le, wp_le, b2_le, s2_le); break;

			case  84+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, wp_le, s2_le, s3_le, b2_le); break;
			case  85+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, wp_le, s2_le, b2_le, s3_le); break;
			case  86+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, wp_le, b2_le, s3_le, s2_le); break;
			case  87+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, wp_le, b2_le, s2_le, s3_le); break;
			case  88+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, wp_le, s3_le, s2_le, b2_le); break;
			case  89+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, wp_le, s3_le, b2_le, s2_le); break;

			case  90+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, b2_le, s2_le, s3_le, wp_le); break;
			case  91+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, b2_le, s2_le, wp_le, s3_le); break;
			case  92+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, b2_le, wp_le, s3_le, s2_le); break;
			case  93+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, b2_le, wp_le, s2_le, s3_le); break;
			case  94+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, b2_le, s3_le, s2_le, wp_le); break;
			case  95+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", sb_le, b2_le, s3_le, wp_le, s2_le); break;

			case  96+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s2_le, s3_le, sb_le, wp_le); break;
			case  97+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s2_le, s3_le, wp_le, sb_le); break;
			case  98+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s2_le, wp_le, sb_le, s3_le); break;
			case  99+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s2_le, wp_le, s3_le, sb_le); break;
			case 100+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s2_le, sb_le, s3_le, wp_le); break;
			case 101+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s2_le, sb_le, wp_le, s3_le); break;

			case 102+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s3_le, s2_le, sb_le, wp_le); break;
			case 103+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s3_le, s2_le, wp_le, sb_le); break;
			case 104+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s3_le, wp_le, sb_le, s2_le); break;
			case 105+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s3_le, wp_le, s2_le, sb_le); break;
			case 106+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s3_le, sb_le, s2_le, wp_le); break;
			case 107+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, s3_le, sb_le, wp_le, s2_le); break;

			case 108+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, sb_le, s2_le, s3_le, wp_le); break;
			case 109+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, sb_le, s2_le, wp_le, s3_le); break;
			case 110+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, sb_le, wp_le, s3_le, s2_le); break;
			case 111+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, sb_le, wp_le, s2_le, s3_le); break;
			case 112+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, sb_le, s3_le, s2_le, wp_le); break;
			case 113+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, sb_le, s3_le, wp_le, s2_le); break;

			case 114+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, wp_le, s2_le, s3_le, sb_le); break;
			case 115+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, wp_le, s2_le, sb_le, s3_le); break;
			case 116+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, wp_le, sb_le, s3_le, s2_le); break;
			case 117+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, wp_le, sb_le, s2_le, s3_le); break;
			case 118+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, wp_le, s3_le, s2_le, sb_le); break;
			case 119+120: n += snprintf(buf+n, size-n, "%010u\n%010u\n%010u\n%010u\n%010u\n", b2_le, wp_le, s3_le, sb_le, s2_le); break;
		}
	}

	return buf;
}

// Transmute the chaos into usable numbers
char* transmute_2(uint8_t *buf)
{
	gcry_md_hd_t h;
	gcry_error_t err;
	digests_t output;
	char *entropy;

	//err = gcry_md_open (&h, GCRY_MD_WHIRLPOOL, GCRY_MD_FLAG_SECURE); NEED MORE MEMORY
	err = gcry_md_open (&h, GCRY_MD_WHIRLPOOL, 0);
	if(err) my_error("gcry_md_open()", err, 3);
	err = gcry_md_enable (h, GCRY_MD_SHA512);
	if(err) my_error("gcry_md_enable()", err, 4);
	err = gcry_md_enable (h, GCRY_MD_SHA3_512);
	if(err) my_error("gcry_md_enable()", err, 5);
	err = gcry_md_enable (h, GCRY_MD_STRIBOG512);
	if(err) my_error("gcry_md_enable()", err, 6);
	err = gcry_md_enable (h, GCRY_MD_BLAKE2B_512);
	if(err) my_error("gcry_md_enable()", err, 7);

	gcry_md_write (h, buf, CHAOSSIZE);

	output.wp_ptr    = gcry_md_read (h, GCRY_MD_WHIRLPOOL);
	output.sha2_ptr  = gcry_md_read (h, GCRY_MD_SHA512);
	output.sha3_ptr  = gcry_md_read (h, GCRY_MD_SHA3_512);
	output.strb_ptr  = gcry_md_read (h, GCRY_MD_STRIBOG512);
	output.blk2b_ptr = gcry_md_read (h, GCRY_MD_BLAKE2B_512);

	entropy = collect_uints(&output);
	gcry_md_close(h);
	return entropy;
}
