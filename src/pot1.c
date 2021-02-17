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

static char* prep_uints_top_down_fw(digests_t *out)
{
	char *buf;
	int i, j, n, size;
	uint32_t *ptrs[HASHCOUNT];
	uint32_t values[HASHCOUNT][INTSPERHASH];

	i = 0;
	size = (HASHCOUNT*INTSPERHASH*BYTESPERINT)+1;
	buf = malloc(size);

	while(i < INTSPERHASH) {
		j = i*sizeof(uint32_t);
		ptrs[0] = (uint32_t *)&out->wp_ptr[j];
		ptrs[1] = (uint32_t *)&out->sha2_ptr[j];
		ptrs[2] = (uint32_t *)&out->sha3_ptr[j];
		ptrs[3] = (uint32_t *)&out->strb_ptr[j];
		ptrs[4] = (uint32_t *)&out->blk2b_ptr[j];
		values[0][i] = htobe32(*ptrs[0]);
		values[1][i] = htobe32(*ptrs[1]);
		values[2][i] = htobe32(*ptrs[2]);
		values[3][i] = htobe32(*ptrs[3]);
		values[4][i] = htobe32(*ptrs[4]);
		i++;
	}

	n = 0;
	for(i=0; i<INTSPERHASH; i++) {	//WHIRLPOOL
		n += snprintf(buf+n, size-n, "%010u\n", values[0][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA2-512
		n += snprintf(buf+n, size-n, "%010u\n", values[1][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA3-512
		n += snprintf(buf+n, size-n, "%010u\n", values[2][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//STRIBOG
		n += snprintf(buf+n, size-n, "%010u\n", values[3][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//BLAKE-2B
		n += snprintf(buf+n, size-n, "%010u\n", values[4][i]);
	}

	return buf;
}

static char* prep_uints_bottom_up_fw(digests_t *out)
{
	char *buf;
	int i, j, n, size;
	uint32_t *ptrs[HASHCOUNT];
	uint32_t values[HASHCOUNT][INTSPERHASH];

	i = 0;
	size = (HASHCOUNT*INTSPERHASH*BYTESPERINT)+1;
	buf = malloc(size);

	while(i < INTSPERHASH) {
		j = i*sizeof(uint32_t);
		ptrs[0] = (uint32_t *)&out->wp_ptr[j];
		ptrs[1] = (uint32_t *)&out->sha2_ptr[j];
		ptrs[2] = (uint32_t *)&out->sha3_ptr[j];
		ptrs[3] = (uint32_t *)&out->strb_ptr[j];
		ptrs[4] = (uint32_t *)&out->blk2b_ptr[j];
		values[0][i] = htobe32(*ptrs[0]);
		values[1][i] = htobe32(*ptrs[1]);
		values[2][i] = htobe32(*ptrs[2]);
		values[3][i] = htobe32(*ptrs[3]);
		values[4][i] = htobe32(*ptrs[4]);
		i++;
	}

	n = 0;
	for(i=0; i<INTSPERHASH; i++) {	//BLAKE-2B
		n += snprintf(buf+n, size-n, "%010u\n", values[4][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//STRIBOG
		n += snprintf(buf+n, size-n, "%010u\n", values[3][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA3-512
		n += snprintf(buf+n, size-n, "%010u\n", values[2][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA2-512
		n += snprintf(buf+n, size-n, "%010u\n", values[1][i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//WHIRLPOOL
		n += snprintf(buf+n, size-n, "%010u\n", values[0][i]);
	}

	return buf;
}

static char* prep_uints_top_down_rev(digests_t *out)
{
	char *buf;
	int i, j, n, size;
	uint32_t *ptrs[HASHCOUNT];
	uint32_t values[HASHCOUNT][INTSPERHASH];

	i = 0;
	size = (HASHCOUNT*INTSPERHASH*BYTESPERINT)+1;
	buf = malloc(size);

	while(i < INTSPERHASH) {
		j = i*sizeof(uint32_t);
		ptrs[0] = (uint32_t *)&out->wp_ptr[j];
		ptrs[1] = (uint32_t *)&out->sha2_ptr[j];
		ptrs[2] = (uint32_t *)&out->sha3_ptr[j];
		ptrs[3] = (uint32_t *)&out->strb_ptr[j];
		ptrs[4] = (uint32_t *)&out->blk2b_ptr[j];
		values[0][i] = htobe32(*ptrs[0]);
		values[1][i] = htobe32(*ptrs[1]);
		values[2][i] = htobe32(*ptrs[2]);
		values[3][i] = htobe32(*ptrs[3]);
		values[4][i] = htobe32(*ptrs[4]);
		i++;
	}

	n = 0;
	for(i=0; i<INTSPERHASH; i++) {	//WHIRLPOOL
		n += snprintf(buf+n, size-n, "%010u\n", values[0][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA2-512
		n += snprintf(buf+n, size-n, "%010u\n", values[1][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA3-512
		n += snprintf(buf+n, size-n, "%010u\n", values[2][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//STRIBOG
		n += snprintf(buf+n, size-n, "%010u\n", values[3][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//BLAKE-2B
		n += snprintf(buf+n, size-n, "%010u\n", values[4][15-i]);
	}

	return buf;
}

static char* prep_uints_bottom_up_rev(digests_t *out)
{
	char *buf;
	int i, j, n, size;
	uint32_t *ptrs[HASHCOUNT];
	uint32_t values[HASHCOUNT][INTSPERHASH];

	i = 0;
	size = (HASHCOUNT*INTSPERHASH*BYTESPERINT)+1;
	buf = malloc(size);

	while(i < INTSPERHASH) {
		j = i*sizeof(uint32_t);
		ptrs[0] = (uint32_t *)&out->wp_ptr[j];
		ptrs[1] = (uint32_t *)&out->sha2_ptr[j];
		ptrs[2] = (uint32_t *)&out->sha3_ptr[j];
		ptrs[3] = (uint32_t *)&out->strb_ptr[j];
		ptrs[4] = (uint32_t *)&out->blk2b_ptr[j];
		values[0][i] = htobe32(*ptrs[0]);
		values[1][i] = htobe32(*ptrs[1]);
		values[2][i] = htobe32(*ptrs[2]);
		values[3][i] = htobe32(*ptrs[3]);
		values[4][i] = htobe32(*ptrs[4]);
		i++;
	}

	n = 0;
	for(i=0; i<INTSPERHASH; i++) {	//BLAKE-2B
		n += snprintf(buf+n, size-n, "%010u\n", values[4][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//STRIBOG
		n += snprintf(buf+n, size-n, "%010u\n", values[3][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA3-512
		n += snprintf(buf+n, size-n, "%010u\n", values[2][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//SHA2-512
		n += snprintf(buf+n, size-n, "%010u\n", values[1][15-i]);
	}
	for(i=0; i<INTSPERHASH; i++) {	//WHIRLPOOL
		n += snprintf(buf+n, size-n, "%010u\n", values[0][15-i]);
	}

	return buf;
}

// Transmute the chaos into usable numbers
char* transmute_1(uint8_t *buf)
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

	switch(stone % 4) {
		case 0: entropy = prep_uints_top_down_fw(&output); break;
		case 1: entropy = prep_uints_bottom_up_fw(&output); break;
		case 2: entropy = prep_uints_top_down_rev(&output); break;
		case 3: entropy = prep_uints_bottom_up_rev(&output); break;
	}
	gcry_md_close(h);
	return entropy;
}
