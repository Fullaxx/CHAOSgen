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

int start_your_engines(void);
void get_chaos(chaos_t *s);

#endif
