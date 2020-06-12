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
