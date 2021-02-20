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
//wsrt_t g_rt;

#ifdef SRNODECHRONOMETRY
#include "chronometry.h"
void print_avg_nodecb_time(void)
{
	long avg;

	if(!g_srv) { return; }

	avg = searest_node_get_avg_duration(g_srv, "/store/128/");
	if(avg > 0) printf("%3s: %ldns\n", "128", avg);

	avg = searest_node_get_avg_duration(g_srv, "/store/160/");
	if(avg > 0) printf("%3s: %ldns\n", "160", avg);

	avg = searest_node_get_avg_duration(g_srv, "/store/224/");
	if(avg > 0) printf("%3s: %ldns\n", "224", avg);

	avg = searest_node_get_avg_duration(g_srv, "/store/256/");
	if(avg > 0) printf("%3s: %ldns\n", "256", avg);

	avg = searest_node_get_avg_duration(g_srv, "/store/384/");
	if(avg > 0) printf("%3s: %ldns\n", "384", avg);

	avg = searest_node_get_avg_duration(g_srv, "/store/512/");
	if(avg > 0) printf("%3s: %ldns\n", "512", avg);
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

#if 0
	// Connect to Redis
	memset(&g_rt, 0, sizeof(wsrt_t));
	g_rt.multithreaded = so->use_threads;
	z = rai_connect(&g_rt.rc, so->rdest, so->rport);
	if(z) {
		if(so->rport) { fprintf(stderr, "Failed to connect to %s:%u!\n", so->rdest, so->rport); }
		else { fprintf(stderr, "Failed to connect to %s!\n", so->rdest); }
		exit(EXIT_FAILURE);
	}
#endif

	// Initialize the server
	g_srv = searest_new(7, 32, 0);
	searest_node_add(g_srv, "/config/",	&config, NULL);
	searest_node_add(g_srv, "/chaos/",	&chaos, NULL);
	searest_node_add(g_srv, "/status/",	&status, NULL);

	// Configure Multithread
	if(so->use_threads == 0) { searest_set_internal_select(g_srv); }

#if 0
	// Configure Connection Limiting
	if(getenv("REQPERIOD")) { g_rt.reqperiod = atoi(getenv("REQPERIOD")); }
	if(getenv("REQCOUNT")) { g_rt.reqcount = atol(getenv("REQCOUNT")); }
	if((g_rt.reqperiod > 0) && (g_rt.reqcount > 0)) {
		searest_set_addr_cb(g_srv, &ws_addr_check);
	}

	// Configure Redis Key Expiration
	if(getenv("EXPIRATION")) { g_rt.expiration = atol(getenv("EXPIRATION")); }
	if(g_rt.expiration < 0) { g_rt.expiration = 0; }

	// Configure immutable messages
	if(getenv("IMMUTABLE")) { g_rt.immutable = 1; }

	// Configure [B]urn [A]fter [R]eading (DELETE after GET)
	if(getenv("BAR")) { g_rt.bar = 1; }
#endif

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
		//rai_disconnect(&g_rt.rc);
	}
}
