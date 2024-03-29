/*
	CHAOSgen is a random number generator based on hardware events 
	Copyright (C) 2022 Brett Kuskie <fullaxx@gmail.com>

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

#include "cJSON.h"
#include "chaos_srv_ops.h"
#include "hiredis/hiredis.h"

unsigned int g_listcount = 10;

static char* handle_null_reply(srci_t *ri, redisContext *rc)
{
	char *etype = NULL;
	switch(rc->err) {
		case REDIS_ERR_IO:
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
	srci_set_return_code(ri, MHD_HTTP_INTERNAL_SERVER_ERROR);
	return strdup("ERROR: NULL REPLY\n");
}

static char* get_numbers(srci_t *ri, srv_opts_t *so, redisContext *rc, int quantity)
{
	int rcode;
	unsigned int list;
	size_t n, size;
	size_t list_miss;
	char *page, *errstr;
	redisReply *reply;

	// Set default values
	n = size = 0;
	list_miss = 0;
	page = errstr = NULL;
	rcode = MHD_HTTP_OK;

	// If we need to bail out for any reason,
	// We will just set quantity = 0
	while(quantity > 0) {
		list = so->last_list++ % g_listcount;
		reply = redisCommand(rc, "LPOP RANDLIST:%u", list);
		if(!reply) {
			// This should not happen
			if(page) { free(page); }
			return handle_null_reply(ri, rc);
		}

		// Process the numbers that we pulled from the list
		if(reply->type == REDIS_REPLY_ERROR) {
			rcode = MHD_HTTP_BAD_REQUEST;
			errstr = "ERROR: REDIS REPLY ERROR\n";
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
			// COME BACK TO THIS WHEN WE CAN DO LPOP <LIST> [COUNT] on REDIS 6.2
			//list_miss = 0;
		}*/ else {
			rcode = MHD_HTTP_INTERNAL_SERVER_ERROR;
			errstr = "ERROR: UNKNOWN\n";
			quantity = 0;
		}
		freeReplyObject(reply);
		if(list_miss > g_listcount) { quantity = 0; }
	}

	if(rcode != MHD_HTTP_OK) {
		// If we came across an error
		// free any page that we started
		// return an error string
		if(page) { free(page); }
		page = strdup(errstr);
	}

	if(!page) {
		// If we made it here then
		// We had no errors AND never found any numbers
		rcode = MHD_HTTP_NOT_FOUND;
		page = strdup("ERROR: SUPPLY EMPTY\n");
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

static char* convert2json(char *numstr)
{
	char *token, *saveptr;
	double rval;
	//uint32_t rval;
	cJSON *root_obj;
	char *page;

	// Process errors before we allocate any memory
	if(strncmp(numstr, "ERROR: SUPPLY EMPTY", 19) == 0) { return strdup("[]"); }
	if(strncmp(numstr, "ERROR:", 6) == 0) { return NULL; }

	root_obj = cJSON_CreateArray();
	token = strtok_r(numstr, " ", &saveptr);
	while(token) {
		rval = strtod(token, NULL);
		//rval = strtoul(token, NULL, 10);
		//printf("%s: %u\n", token, rval);
		//cJSON_AddItemToArray(root_obj, cJSON_CreateString(token));
		cJSON_AddItemToArray(root_obj, cJSON_CreateNumber(rval));
		token = strtok_r(NULL, " ", &saveptr);
	}

	page = cJSON_Print(root_obj);
	cJSON_Delete(root_obj);
	cJSON_Minify(page);

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

char* chaos_node(char *url, int urllen, srci_t *ri, void *sri_user_data, void *node_user_data)
{
	int quantity;
	char *page, *jsonstr;
	srv_opts_t *so;

	if(shutting_down()) { return shutdownmsg(ri); }
	if(METHOD(ri) != METHOD_GET) { return badmethod(ri); }
	so = (srv_opts_t *)sri_user_data;

	quantity = atoi(url);
	if(quantity < 1) { quantity = 1; }
	if(quantity > so->maxqty) { quantity = so->maxqty; }

	// Go get a collection of numbers from redis
	page = get_chaos(ri, so, quantity);

	// Convert to JSON output if requested
	if(srci_browser_requests_json(ri)) {
		jsonstr = convert2json(page);
		if(jsonstr) {
			free(page); page = jsonstr;
			srci_set_response_content_type(ri, MIMETYPEAPPJSONSTR);
		}
	}

	return page;
}

char* config_node(char *url, int urllen, srci_t *ri, void *sri_user_data, void *node_user_data)
{
	//srv_opts_t *so;

	if(shutting_down()) { return shutdownmsg(ri); }
	if(METHOD(ri) != METHOD_GET) { return badmethod(ri); }
	//so = (srv_opts_t *)sri_user_data;

	srci_set_return_code(ri, MHD_HTTP_OK);
	return strdup("OK");
}

static inline long get_status_val(redisContext *rc, char *key)
{
	long retval = 0;
	redisReply *reply;
	reply = redisCommand(rc, "GET %s", key);
	if(reply->type == REDIS_REPLY_STRING) {
		if(reply->str) {
			retval = atol(reply->str);
		}
	}
	freeReplyObject(reply);
	if(retval < 0) { retval = 0; }
	return retval;
}

static char* status2json(long chaos, long rnum)
{
	cJSON *root_obj;
	char *page;

	root_obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(root_obj, "Chaos/s", chaos);
	cJSON_AddNumberToObject(root_obj, "Numbers/s", rnum);
	page = cJSON_Print(root_obj);
	cJSON_Delete(root_obj);
	cJSON_Minify(page);

	return page;
}

static char* get_status(srci_t *ri, srv_opts_t *so)
{
	char *statstr;
	redisContext *rc;
	long chaos, rnum;

	// Connect to redis
	if(so->rport) { rc = redisConnect(so->rdest, so->rport); }
	else		{ rc = redisConnectUnix(so->rdest); }
	if(!rc) { return redisAllocationFailure(ri); }
	if(rc->err) { return redisConnectionFailure(ri, rc); }

	chaos = get_status_val(rc, "CHAOSPERSEC");
	rnum = get_status_val(rc, "RNUMPERSEC");

	// Convert to JSON output if requested
	if(srci_browser_requests_json(ri)) {
		statstr = status2json(chaos, rnum);
		srci_set_response_content_type(ri, MIMETYPEAPPJSONSTR);
	} else {
		statstr = malloc(128);
		snprintf(statstr, 128, "Chaos: %lu\nNumbers: %lu\n", chaos, rnum);
	}

	redisFree(rc);
	return statstr;
}

char* status_node(char *url, int urllen, srci_t *ri, void *sri_user_data, void *node_user_data)
{
	char *page;
	srv_opts_t *so;

	if(shutting_down()) { return shutdownmsg(ri); }
	if(METHOD(ri) != METHOD_GET) { return badmethod(ri); }
	so = (srv_opts_t *)sri_user_data;

	// Get Status values from redis and return a status page
	page = get_status(ri, so);

	srci_set_return_code(ri, MHD_HTTP_OK);
	return page;
}
