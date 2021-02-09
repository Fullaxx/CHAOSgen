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

// the amount of bytes returned in each get_chaos call
#define CHAOSSIZE (PSIZE*sizeof(PBT))

int start_your_engines(void);
int get_chaos(uint8_t *buf);

#endif
