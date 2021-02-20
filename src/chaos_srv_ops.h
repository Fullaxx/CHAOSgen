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

#ifndef __CHAOS_SERVICE_OPERATIONS_H__
#define __CHAOS_SERVICE_OPERATIONS_H__

#include "searest.h"

typedef struct {
	char *http_ip;
	unsigned short http_port;
	int use_threads;
	char *certfile;
	char *keyfile;

	char *rdest;			// Redis Dest
	unsigned short rport;	// Redis Port

	int maxqty;
	unsigned int last_list;
} srv_opts_t;

// Found in chaos_srv.c
int shutting_down(void);

// Found in chaos_srv_uhd.c
void chaos_srv_start(srv_opts_t *);
void chaos_srv_stop(void);

// Found in chaos_srv_node.c
char* chaos(char *, int, srci_t *, void *, void *);
char* config(char *, int, srci_t *, void *, void *);
char* status(char *, int, srci_t *, void *, void *);

#endif
