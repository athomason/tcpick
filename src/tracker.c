/*
 * tracker.c -- connection status handler
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "tcpick.h"
#include "extern.h"

struct CONN * first_conn = NULL; /*all connections, ends on a 0*/
struct CONN * last_conn  = NULL; 

int 
status_switch(struct CONN * prev, enum STATUS status) 
{
#define CURR (prev->next)
	display_status( stdout, CURR, status );
	if ( status == CLOSED || 
	     status == RESET  || 
	     status == EXPIRED )
		rmconn( prev );
	else
		CURR->status = status;
}

int 
newconn( struct CONN * prev_ring )
/* allocates a new connection */
{
	struct CONN * conn;

	if( flags.trackonly != -1 ) {
		if( flags.trackonly_first &&
		    ( count_conns >= flags.trackonly ) )
			return -1;
		else if( count_opened >= flags.trackonly )
			return -1;
		else
			count_opened++;
	}
	count_conns++;

	conn = ( struct CONN * )S_calloc( sizeof( struct CONN ), 1 );
	
	prev_ring->next = conn; /* and the chain becomes longer... :)
				   thank you mainman for this good idea! */
	last_conn = conn;

	conn->client.side = CLIENT;
	conn->server.side = SERVER;

	conn->client.ip.s_addr = ippacket->ip_src.s_addr;
	conn->server.ip.s_addr = ippacket->ip_dst.s_addr;
	conn->client.port      = tcppacket->source;
	conn->server.port      = tcppacket->dest;
	
	conn->client.sip  = ntohl(tcppacket->seq) + 1;
	
	conn->client.oth = & (conn->server);
	conn->server.oth = & (conn->client);
	
	conn->num = count_conns;

	memcpy(&conn->firsttime, &pcapheader->ts, sizeof(struct timeval));
	conn->lasttime = time(NULL);
	
	status_switch( prev_ring, SYN_SENT );
}

int rmconn( struct CONN * prev_ring )
/* removes from the linked-list and deallocates a connection
 * prev_ring 
 */
{
	struct CONN * curr = prev_ring->next;
	struct CONN * conn;
	
	prev_ring->next = curr->next;
	if( curr->next == NULL )
		last_conn = prev_ring;
	
	free_desc( &(curr->client), 1, curr);
	if (flags.writer.type == UNIQUE) free_desc( &(curr->server), 0, curr);
	else free_desc( &(curr->server), 1, curr);
	S_free( curr );

	conn = first_conn;
	
	if( flags.exitclosed ) {
		if( flags.exitclosed_first ) { /* -Ef */
			while( conn->num > flags.exitclosed ) {
				if(! (conn->next) )
					exit( EXIT_SUCCESS ); 
				conn = conn->next;
			}
		} else { /* -E */
			--flags.exitclosed;
			if( flags.exitclosed == 0)
				exit( EXIT_SUCCESS ); 
		}
	}

	if( flags.trackonly &&
	    (! flags.trackonly_first) )
		count_opened--;
}

int free_desc( struct HOST_DESC * desc, int freedescfilename, struct CONN * conn )
/* frees the host descriptor and closes the file */
{
	struct FRAGMENT * tmp;

	if( desc->file ) {
		fclose( desc->file );
		if (flags.writer.type == UNIQUE)
			desc->oth->file = NULL;
		struct timeval t[2] = { conn->firsttime, conn->firsttime };
		utimes( desc->filename, t );
	}
	if( desc->filename && freedescfilename ) {
		S_free( desc->filename );
		desc->filename = NULL;
	}

	if( desc->lockfilename ) {
		unlink( desc->lockfilename );
		S_free( desc->lockfilename );
		desc->lockfilename = NULL;
	}
	
	while( desc->unack ) { /* FIXME: desc->unack should be just NULL when closes! 
				* I should check it! */
		S_free( desc->unack->payload );
		desc->unack->payload = NULL;
		tmp = desc->unack;
		desc->unack = desc->unack->next;
		S_free( tmp );
	}
}
