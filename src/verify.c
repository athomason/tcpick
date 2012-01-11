/*
 * verify.c -- the connection matching engine
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "tcpick.h"
#include "extern.h"

#define CURRENT_CONN (prev_conn->next)

#define ACK_OFF ntohl( tcppacket->ack_seq ) - Desc->oth->sip 
/* FIXME: to def.h? */

#define IS_CLIENT_SENDER \
 CURRENT_CONN->client.ip.s_addr == ippacket->ip_src.s_addr && \
 CURRENT_CONN->server.ip.s_addr == ippacket->ip_dst.s_addr && \
 CURRENT_CONN->client.port == tcppacket->source            && \
 CURRENT_CONN->server.port == tcppacket->dest 

#define IS_SERVER_SENDER \
 CURRENT_CONN->client.ip.s_addr == ippacket->ip_dst.s_addr && \
 CURRENT_CONN->server.ip.s_addr == ippacket->ip_src.s_addr && \
 CURRENT_CONN->client.port == tcppacket->dest              && \
 CURRENT_CONN->server.port == tcppacket->source 

#define IS_SYN_SENT \
 tcppacket->syn == 1 && \
 tcppacket->ack == 0

#define IS_SYN_RECEIVED \
 tcppacket->syn         == 1        && \
 tcppacket->ack         == 1        && \
 Desc->side             == SERVER   && \
 CURRENT_CONN->status   == SYN_SENT

#define IS_ESTABLISHING \
 tcppacket->syn           == 0            && \
 tcppacket->ack           == 1            && \
 Desc->side               == CLIENT       && \
 CURRENT_CONN->status     == SYN_RECEIVED && \
 CURRENT_CONN->client.sip == ntohl( tcppacket->seq )

#define IS_DATA_FLOW       1

#define IS_FIN_WAIT_1 \
 tcppacket->fin         == 1           && \
 CURRENT_CONN->status   == ESTABLISHED

#define IS_FIN_WAIT_2__CLOSE_WAIT \
 tcppacket->ack         == 1           && \
 tcppacket->fin         == 0           && \
 CURRENT_CONN->status   == FIN_WAIT_1  && \
 CURRENT_CONN->closer   != Desc->side 

#define IS_TIME_WAIT__LAST_ACK \
 tcppacket->ack         == 1                         && \
 tcppacket->fin         == 1                         && \
 ( CURRENT_CONN->status == FIN_WAIT_2__CLOSE_WAIT ||    \
 CURRENT_CONN->status == FIN_WAIT_1 )              && \
 CURRENT_CONN->closer   != Desc->side 

#define IS_CLOSING  \
 tcppacket->ack         == 1                         && \
 tcppacket->fin         == 0                         && \
 CURRENT_CONN->status   == TIME_WAIT__LAST_ACK       && \
 CURRENT_CONN->closer   == Desc->side 

#define IS_RESET  \
 tcppacket->rst         == 1


__inline__ int
established_packet ( struct CONN * conn_ptr, struct HOST_DESC * Desc)
/* called by verify()
   packets of established connections come here */
{
	register u_int32_t off;
	if( payload_len ) {

		off = ntohl( tcppacket->seq ) - Desc->sip ;
		
		/* unacknowledged data */
		addfr( & (Desc->unack),
		       Desc->wlen,
		       off, 
		       payload, 
		       payload_len );
	}

	if( payload_len == 0) {
		/* acknowledge data */
		flush_ack ( Desc->oth, conn_ptr, ACK_OFF  ); 
	}
	else if( tcppacket->psh ) {
		/* PUSH data */
		flush_ack ( Desc, conn_ptr, ACK_OFF  );
     }

}

__inline__ int  
verify()
/* called by got_packet().
   this is the engine that matches the packets with their connection */
{

	register struct HOST_DESC * Desc = NULL;

	register struct CONN * prev_conn;
	
	prev_conn = first_conn;
	
/* SYN-SENT: (outside of the loop because the 
 * loop is used for sessions just tracked) */	 
	if( IS_SYN_SENT ) {
		newconn( last_conn ); 
		return(1);
	}
	
	if(! CURRENT_CONN ) 
		return 0 ;

	while (! Desc ) {
/* cycle until it finds a connection that 
   matches on server or client side */

		if ( IS_CLIENT_SENDER ) 
			Desc = & (CURRENT_CONN->client);
		
		else if ( IS_SERVER_SENDER )
			Desc = & (CURRENT_CONN->server);
		else {
			prev_conn = prev_conn->next;
			if(! CURRENT_CONN ) 
				return 0 ;
		}
	} 
	
/* update time of last packet in connection */
	CURRENT_CONN->lasttime = time(NULL);

/* when this point is reached, we have found
   the right matching connection */
    
	if( IS_SYN_RECEIVED ) {
		CURRENT_CONN->server.sip = ntohl(tcppacket->seq) + 1;
		status_switch( prev_conn, SYN_RECEIVED );
		return 1;
	}
    

/* 3rd packet of the 3-way-handshake*/
	if( IS_ESTABLISHING ) {
		status_switch( prev_conn, ESTABLISHED );
		return 1;
	}
    
/* data or ack packets */
	if( IS_DATA_FLOW ) {
		if( REBUILD ) 
			established_packet ( CURRENT_CONN, Desc );
	}
	
/* first FIN sent */
	if( IS_FIN_WAIT_1 ) {
		status_switch( prev_conn, FIN_WAIT_1 );
		CURRENT_CONN->closer = Desc->side; 
		return 1;
	}

     
/* ACK of first FIN sent */
	if( IS_FIN_WAIT_2__CLOSE_WAIT ) {
		status_switch( prev_conn, FIN_WAIT_2__CLOSE_WAIT );
		return 1;
	}

     
/* second FIN sent */
	if( IS_TIME_WAIT__LAST_ACK ) {
		status_switch( prev_conn, TIME_WAIT__LAST_ACK );
		return 1;
	}

/* LAST-ACK sent */
	if( IS_CLOSING ) {
		status_switch( prev_conn, CLOSED );
		return 1;
	}
/* RST sent */
	if( IS_RESET ) {
		status_switch( prev_conn, RESET );
		return 1;
	}

/* nice work :^) */
}

