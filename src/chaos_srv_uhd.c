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

#include "chaos_srv_ops.h"
#include "futils.h"

// The searest instance
sri_t *g_srv = NULL;

#ifdef SRNODECHRONOMETRY
#include "chronometry.h"
void print_avg_nodecb_time(void)
{
	long avg;

	if(!g_srv) { return; }

	avg = searest_node_get_avg_duration(g_srv, "/chaos/");
	if(avg > 0) printf("%3s: %ldns\n", "chaos", avg);

	avg = searest_node_get_avg_duration(g_srv, "/config/");
	if(avg > 0) printf("%3s: %ldns\n", "config", avg);

	avg = searest_node_get_avg_duration(g_srv, "/status/");
	if(avg > 0) printf("%3s: %ldns\n", "status", avg);
}
#endif

#if 0
// return SR_IP_DENY to DENY a new incoming connection based on IP address
// return SR_IP_ACCEPT to ACCEPT a new incoming connection based on IP address
static int ws_addr_check(char *inc_ip, void *sri_user_data)
{
	int z;
	wsrt_t *lrt = (wsrt_t *)sri_user_data;

	// Blindly accept w/o logging localhost
	if(strcmp(inc_ip, "127.0.0.1") == 0) { return SR_IP_ACCEPT; }

	z = allow_ip(lrt, inc_ip);
	if(z == 0) { return SR_IP_DENY; }

	return SR_IP_ACCEPT;
}
#endif

static void activate_https(srv_opts_t *so)
{
	char *cert, *key;

	cert = get_file(so->certfile);
	if(!cert) { exit(EXIT_FAILURE); }
	key = get_file(so->keyfile);
	if(!key) { exit(EXIT_FAILURE); }
	searest_set_https_cert(g_srv, cert);
	searest_set_https_key(g_srv, key);
	free(cert);
	free(key);
}

void chaos_srv_start(srv_opts_t *so)
{
	int z;

	// Initialize the server
	g_srv = searest_new(7, 32, 0);
	searest_node_add(g_srv, "/chaos/",	&chaos_node, NULL);
	searest_node_add(g_srv, "/config/",	&config_node, NULL);
	searest_node_add(g_srv, "/status/",	&status_node, NULL);

	// Configure Multithread
	if(so->use_threads == 0) { searest_set_internal_select(g_srv); }

	// Configure HTTPS
	if(so->certfile && so->keyfile) { activate_https(so); }

	// Start the server
	z = searest_start(g_srv, so->http_ip, so->http_port, so);
	if(z) {
		if(!so->http_ip) { so->http_ip="*"; }
		fprintf(stderr, "searest_start() failed to bind to %s:%u!\n", so->http_ip, so->http_port);
		exit(EXIT_FAILURE);
	}
}

void chaos_srv_stop(void)
{
	if(g_srv) {
		searest_stop(g_srv);
		searest_del(g_srv);
	}
}
