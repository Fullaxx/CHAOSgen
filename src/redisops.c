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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <hiredis/hiredis.h>

#include "pouch.h"

#ifdef CHRONOMETRY
#include "chronometry.h"
#endif

extern int g_shutdown;
pthread_mutex_t rlock = PTHREAD_MUTEX_INITIALIZER;
redisContext *rc = NULL;

// Default values, 10 lists and 100000 random numbers each
uint32_t g_maxlists = 10;
uint32_t g_listsize = 100000;
uint32_t g_index = 0;

#include <errno.h>
static inline void handle_redis_error(void)
{
	char *etype = NULL;
	switch(rc->err) {
		case REDIS_ERR_IO:
			fprintf(stderr, "REDIS_ERR_IO: %s\n", strerror(errno));
			break;
		case REDIS_ERR_EOF:
			etype = "REDIS_ERR_EOF";
			break;
		case REDIS_ERR_PROTOCOL:
			etype = "REDIS_ERR_PROTOCOL";
			break;
		case REDIS_ERR_OOM:
			etype = "REDIS_ERR_OOM";
			break;
		case REDIS_ERR_OTHER:
			etype = "REDIS_ERR_OTHER";
			break;
		default:
			etype = "UNKNOWN";
	}
	if(etype) {
		fprintf(stderr, "%s: %s\n", etype, rc->errstr);
	}
	g_shutdown = 1;
}

#ifdef USE_MULTI_COMMAND
static inline void do_redis_lpush(char *cmd, char *list)
{
	redisReply *reply;

	freeReplyObject(redisCommand(rc, "MULTI"));
	freeReplyObject(redisCommand(rc, cmd));
	freeReplyObject(redisCommand(rc, "LTRIM %s 0 %u", list, g_listsize-1));
	reply = (redisCommand(rc, "EXEC"));
	if(!reply) { handle_redis_error(); return; }
	freeReplyObject(reply);
}
#else
static inline void do_redis_ltrim(char *list)
{
	redisReply *reply;
	reply = redisCommand(rc, "LTRIM %s 0 %u", list, g_listsize-1);
	if(!reply) { handle_redis_error(); return; }
/*#ifdef DEBUG
	if(reply->type == REDIS_REPLY_STATUS) { printf("LTRIM: %s\n", reply->str); }
#endif*/
	freeReplyObject(reply);
}

static inline void do_redis_lpush(char *cmd, char *list)
{
	uint64_t count;
	redisReply *reply;

	reply = redisCommand(rc, cmd);
	if(!reply) { handle_redis_error(); return; }
	if(reply->type == REDIS_REPLY_INTEGER) {
		count = (uint64_t)reply->integer;
		//printf("LPUSH: %9lu\n", count);
		if(count > g_listsize) { do_redis_ltrim(list); }
	}
	freeReplyObject(reply);
}
#endif

static void update(int snum, chaos_t sarr[])
{
	int i;
	size_t n, size;
	char liststr[64];
	char *cmd;

	n = 0;
	size = 64;
	for(i=0; i<snum; i++) {
		if(!sarr[i].numbers) { return; }
		size += 1+strlen(sarr[i].numbers);
	}

	cmd = malloc(size);
	n += snprintf(cmd+n, size-n, "LPUSH");

	if(g_index == g_maxlists) {
		snprintf(liststr, sizeof(liststr), "ADMIN");
		n += snprintf(cmd+n, size-n, " ADMIN");
		g_index = 0;
	} else {
		snprintf(liststr, sizeof(liststr), "RANDLIST:%u", g_index);
		n += snprintf(cmd+n, size-n, " RANDLIST:%u", g_index);
		g_index++;
	}

	for(i=0; i<snum; i++) {
		n += snprintf(cmd+n, size-n, "%s", sarr[i].numbers);
	}

	do_redis_lpush(cmd, &liststr[0]);
	free(cmd);
}

void push_random_numbers(int snum, chaos_t sarr[])
{
#ifdef TIME_REDIS_UPDATE
	stopwatch_t watch;
#endif

	// Only 1 thread in here at a time
	pthread_mutex_lock(&rlock);

	if((g_shutdown) || (!rc)) {
		// If we are done, bail out
		pthread_mutex_unlock(&rlock);
		return;
	}

#ifdef TIME_REDIS_UPDATE
	chron_start(&watch, -1);
#endif

	update(snum, sarr);

#ifdef TIME_REDIS_UPDATE
	printf("push_random_numbers(): %ld\n", chron_stop(&watch));
#endif

	pthread_mutex_unlock(&rlock);
}

// return -1 means you have an open redis handle already (or it appears that way), bail
// return -2 means redisConnect() failed, bail
// return -3 means there was an connetion error during redisConnect()
int doConnect(char *dest, unsigned short port)
{
	if(rc) { return -1; }

	if(port) { rc = redisConnect(dest, port); }
	else	{ rc = redisConnectUnix(dest); }

	if(!rc) {
		fprintf(stderr, "Connection error: failed to allocate redis context\n");
		return -2;
	}

	if(rc->err) {
		fprintf(stderr, "Connection error: %s\n", rc->errstr);
		redisFree(rc);
		rc = NULL;
		return -3;
	}

	return 0;
}

void doDisconnect(void)
{
	if(rc) {
		pthread_mutex_lock(&rlock);
		redisFree(rc);
		rc = NULL;
		pthread_mutex_unlock(&rlock);
	}
}

void set_list_count(uint32_t count)
{
	if(count == 0) {
		fprintf(stderr, "List count can not be zero!\n");
		exit(1);
	}
	g_maxlists = count;
}

void set_list_size(uint32_t size)
{
	if(size == 0) {
		fprintf(stderr, "List size can not be zero!\n");
		exit(1);
	}
	g_listsize = size;
}
