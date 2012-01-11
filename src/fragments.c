/*
 * fragments.c -- manages unacknowledged data
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

/* 
 * Explanation of this code (fragments.c)
 * ======================================
 *
 * The stream of data of the tcp connection will be saved on 
 * the file, but we know that tcp connection aren't datagram-oriented, 
 * but stream-oriented! This means we *must* rebuild the disordered
 * flow. The problem is: we cannot store entire tcp connections in
 * memory! This is simply crazy! What if somebody is downloading cd
 * iso images, or dvd films! They will surely be corrupted.
 * Solution: write the rebuilded part of tcp flow in the file when
 * there are no missing pieces; so we will be able to store in memory
 * a linked list that reorders the tcp data flow and then, when data
 * are acknowledged by a "ack" packet, these data will be written to
 * the output file.
 *
 * Phase 1:
 *     Picking up the packets containing data from the interface.
 * 
 * Packets containing data fragments are stored into the linked list
 * conn_ptr->unack, managed by the insfr(...) function
 * These data are not yet acknowledged with the "ack"
 * packet. Duplicates are discarded and the flow will be completely
 * rebuilt.
 * 
 * Phase 2:
 *     Acks are arriving!
 * 
 * When a "ack" packet arrives, the function flush_ack(...) writes the
 * acknowledged fragments to the output file, and deleletes them from
 * the linked list.
 */

#include "tcpick.h"
#include "extern.h"

#define MATCH_OFF(fr, oth) fr->off + fr->len == oth->off

#define ATTACH_FR(prev, this){			\
						\
	this->next = prev->next;		\
	prev->next = this;			\
						\
	if (MATCH_OFF(this, prev->next))	\
		this->flag = CONTINUE;		\
	else					\
		this->flag = BREAK;             \
        }


#define FREE_FR(fr) S_free( fr->payload ); S_free( fr );

#define DELETE_FIRST_FR(list) {                         \
                        struct FRAGMENT * tmp;        \
                        tmp = * list;                   \
                        * list = ( * list )->next;      \
                        FREE_FR (tmp)                   \
                        }
__inline__ int 
addfr( struct FRAGMENT ** first,
       int wlen,
       u_int32_t data_off,
       u_char * payload,
       int payload_len )
/* called by established_packet().
 * Insert and reorder data fragments in a linked-list.
 * Duplicates are discarded.
 * 
 * return values: 
 * 0: fragment refused
 * 1: fragment accepted in the linked-list
 */

{
	struct FRAGMENT * new;
	struct FRAGMENT * curr;

	if ( data_off >= wlen ) {
 
		/* allocate and build the fragment */
		new = ( struct FRAGMENT * ) S_calloc ( 1, sizeof( struct FRAGMENT ));
		new->payload = ( u_char * ) S_calloc( 1, payload_len );
		new->off = data_off;
		memcpy( new->payload, payload, payload_len );
		new->len = payload_len;
		
		/* now insert the fragment in the flagged linked list */
		
		/* linked list was empty */
		if (! * first ) {
			* first = new;
			(* first)->next = NULL;
			return 1;
		}
		
		/* replace first */
		if ( (* first)->off > new->off) { 
			new->next = * first;
			* first = new;
			return 1;
		}
		
		curr = * first;
		
		while ( curr ) {
			if( curr->flag == BREAK &&
			    MATCH_OFF(curr, new) ) {
				/* the new data fragment is exactly next to curr */ 
				ATTACH_FR( curr, new );
				return 1;
			}
			else if ( curr->next ) {
				if( curr->next->off > new->off ) {
			/* the new data fragment is _not_ exactly next to curr,
			   but it must be there because the next
			   fragment has a bigger offset	*/
					ATTACH_FR( curr, new );
					return 1;
				}
			} else {
				/* it is the last */
				curr->next = new;
				return 1;
			}
			
			curr = curr->next;
		}
		return 0;
	}
}

__inline__ int
flush_ack ( struct HOST_DESC * desc,
	    struct CONN * conn_ptr,
	    int ack_num )
/* called by established_packet
 * when a "ack" packet comes to the network device,
 * data that are unacknowledged will be acknowledged and immediatly
 * sent to the write engine wrebuild 
 */
{
#define LIST (desc->unack)
	
	while( LIST &&
	       LIST->off <= ack_num ) {
		
		flowflush( conn_ptr, desc->oth, LIST->payload, LIST->len );

		debug("wrote off=%i wlen=%i len=%i ack=%i\n",
		      LIST->off, desc->wlen, LIST->len, ack_num);

		desc->wlen += LIST->len;
		DELETE_FIRST_FR ( & LIST );
	}
}
