/*
 * lookup_query.c -- the host lookup engine
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004 Francesco Stablum
 * Licensed under the GPL
 *
 */

/* 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at you option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111,
 * USA.
 */

#include "tcpick.h"
#include "extern.h"

char *
lookup_new(struct in_addr ia)
{
	register struct hostent * he;
	register struct _l_node * node;
	
	he = gethostbyaddr((char *)&ia, sizeof(struct in_addr), AF_INET);
	
	node = (struct _l_node *)_l_alloc(ia, (he == NULL) ? inet_ntoa(ia) : he->h_name );

	_l_insert( node );

	return node->name;
}

char *
lookup(struct in_addr ia)
/* WARNING: returned buffer is by default static! */

{
	register char * name;

	if(flags.lookup == 0) /* this is default */
		return inet_ntoa(ia);
	
	
	name = (char *)_l_get(ia);

	return ( (name == NULL)
		 ? (char *)lookup_new(ia) 
		 : (char *)name );
}

char * 
getportname(u_int16_t port)
/* FIXME: completely rewrite! */
{
	static char *service=NULL;
	static char *s_port=NULL;
	struct servent * ptr;

	if(service) {
		S_free(service);
		service=NULL;
	}
	if(s_port) {
		S_free(s_port);
		s_port=NULL;
	}
	
	if(!flags.numericalports)
		if(ptr=(struct servent *)getservbyport(port, "tcp"))
			if(ptr->s_name) {
				debug("service name resolved: %s",ptr->s_name);
				return service=(char *)strdup(ptr->s_name);
			}
	debug( "service name NOT resolved: %d",ntohs(port) );
	/* \n carachter should be inserted because this messages comes after 
	 *  a "printf" message not ended in \n in another function 
	 */
	s_port=(char *)S_calloc(128,1);
	sprintf(s_port,"%d",ntohs(port));
	return s_port;
}
