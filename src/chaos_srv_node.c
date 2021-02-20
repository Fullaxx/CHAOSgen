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
//#include <unistd.h>
//#include <ctype.h>

#include "chaos_srv_ops.h"
#include "hiredis/hiredis.h"

unsigned int g_listcount = 10;

//#include <errno.h>
static char* handle_null_reply(srci_t *ri, redisContext *rc)
{
	char *etype = NULL;
	switch(rc->err) {
		case REDIS_ERR_IO:
			//fprintf(stderr, "REDIS_ERR_IO: %s\n", strerror(errno));
			perror("REDIS_ERR_IO");
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
	//g_shutdown = 1;
	srci_set_return_code(ri, MHD_HTTP_INTERNAL_SERVER_ERROR);
	return strdup("NULL REPLY\n");
}

static char* get_numbers(srci_t *ri, srv_opts_t *so, redisContext *rc, int quantity)
{
	int rcode;
	unsigned int list;
	size_t n, size;
	//size_t cmd_count;
	size_t list_miss;
	char *page, *errstr;
	redisReply *reply;

	n = size = 0;
	//cmd_count = 0;
	list_miss = 0;
	page = errstr = NULL;
	rcode = MHD_HTTP_OK;

	while(quantity > 0) {
		list = so->last_list++;
		reply = redisCommand(rc, "LPOP RANDLIST:%u", list % g_listcount);
		if(!reply) {
			if(page) { free(page); }
			return handle_null_reply(ri, rc);
		}

		// Process the request
		if(reply->type == REDIS_REPLY_ERROR) {
			rcode = MHD_HTTP_BAD_REQUEST;
			errstr = "REDIS REPLY ERROR\n";
			quantity = 0;
		} else if(reply->type == REDIS_REPLY_NIL) {
			// LIST IS EMPTY
			list_miss++;
		} else if(reply->type == REDIS_REPLY_STRING) {
			list_miss = 0;
			size += 10+1+1;
			page = realloc(page, size);
			if(n == 0)	{ n += snprintf(page+n, size-n, "%s", reply->str); }
			else		{ n += snprintf(page+n, size-n, " %s", reply->str); }
			quantity--;
		} /*else if(reply->type == REDIS_REPLY_ARRAY) {
			// COME BACK TO THIS WHEN WE DO LPOP <LIST> [COUNT] on REDIS 6.2
			//list_miss = 0;
		}*/ else {
			rcode = MHD_HTTP_INTERNAL_SERVER_ERROR;
			errstr = "UNKNOWN ERROR\n";
			quantity = 0;
		}
		//cmd_count++;
		freeReplyObject(reply);
		if(list_miss > g_listcount) { quantity = 0; }
	}

	if(rcode != MHD_HTTP_OK) {
		// If we came across an error
		// free any page that we started
		if(page) { free(page); }
		page = strdup(errstr);
	}

	if(!page) {
		// If we got here then we had no errors AND never found any numbers
		rcode = MHD_HTTP_NOT_FOUND;
		page = strdup("SUPPLY EMPTY\n");
	}

	srci_set_return_code(ri, rcode);
	return page;
}

static inline char* redisConnectionFailure(srci_t *ri, redisContext *rc)
{
	char *page;
	page = malloc(1024);
	snprintf(page, 1024, "Connection error: %s\n", rc->errstr);
	srci_set_return_code(ri, MHD_HTTP_SERVICE_UNAVAILABLE);
	redisFree(rc);
	return page;
}

static inline char* redisAllocationFailure(srci_t *ri)
{
	char *page;
	page = strdup("Connection error: failed to allocate redis context\n");
	srci_set_return_code(ri, MHD_HTTP_INTERNAL_SERVER_ERROR);
	return page;
}

static inline void get_listcount(redisContext *rc)
{
	redisReply *reply;
	reply = redisCommand(rc, "GET LISTCOUNT");
	if(reply->type == REDIS_REPLY_STRING) {
		if(reply->str) {
			g_listcount = atoi(reply->str);
		}
	}
	freeReplyObject(reply);
}

static char* get_chaos(srci_t *ri, srv_opts_t *so, int quantity)
{
	char *page;
	redisContext *rc;

	// Connect to redis
	if(so->rport) { rc = redisConnect(so->rdest, so->rport); }
	else		{ rc = redisConnectUnix(so->rdest); }
	if(!rc) { return redisAllocationFailure(ri); }
	if(rc->err) { return redisConnectionFailure(ri, rc); }

	get_listcount(rc);
	page = get_numbers(ri, so, rc, quantity);
	redisFree(rc);
	return page;
}

static inline char* badmethod(srci_t *ri)
{
	srci_set_return_code(ri, MHD_HTTP_METHOD_NOT_ALLOWED);
	return strdup("method not allowed");
}

static inline char* shutdownmsg(srci_t *ri)
{
	srci_set_return_code(ri, MHD_HTTP_SERVICE_UNAVAILABLE);
	return strdup("service unavailable: shutting down");
}

char* chaos(char *url, int urllen, srci_t *ri, void *sri_user_data, void *node_user_data)
{
	int quantity;
	char *collection;
	srv_opts_t *so;

	if(shutting_down()) { return shutdownmsg(ri); }
	if(METHOD(ri) != METHOD_GET) { return badmethod(ri); }
	so = (srv_opts_t *)sri_user_data;

	quantity = atoi(url);
	if(quantity < 1) { quantity = 1; }
	if(quantity > so->maxqty) { quantity = so->maxqty; }

	collection = get_chaos(ri, so, quantity);

	// CHECK FOR JSON OUTPUT

	return collection;
}

char* config(char *url, int urllen, srci_t *ri, void *sri_user_data, void *node_user_data)
{
	//srv_opts_t *so;

	if(shutting_down()) { return shutdownmsg(ri); }
	if(METHOD(ri) != METHOD_GET) { return badmethod(ri); }
	//so = (srv_opts_t *)sri_user_data;

	srci_set_return_code(ri, MHD_HTTP_OK);
	return strdup("OK");
}

char* status(char *url, int urllen, srci_t *ri, void *sri_user_data, void *node_user_data)
{
	//srv_opts_t *so;

	if(shutting_down()) { return shutdownmsg(ri); }
	if(METHOD(ri) != METHOD_GET) { return badmethod(ri); }
	//so = (srv_opts_t *)sri_user_data;

	srci_set_return_code(ri, MHD_HTTP_OK);
	return strdup("OK");
}
