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
