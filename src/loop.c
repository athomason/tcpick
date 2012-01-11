/*
 * loop.c -- the main loop of the capture engine
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

int 
got_packet (u_char * useless,
	    const struct pcap_pkthdr* hdr,
	    const u_char* packet) 
/* called by pcap_loop.
   this is the main loop for the packet engine. */

{ /* FIXME: this function is too long */

	/* check if the flag for checking for expired connections 
	   is turned on */
	
	if(expired_flag == CHECK_GOTPACKET) {
		check_expired();
	}
	expired_flag = CHECK_NOT;

	if(! packet)
		fault( "got_packet", "didn't grab packet" );

	count_packets++;
        
	ippacket=(struct ip *)(packet + datalink_size);

#if __BYTE_ORDER == __LITTLE_ENDIAN
	iplen = (ippacket->ip_len % 256) * 256 + (ippacket->ip_len) / 256;
#elif __BYTE_ORDER == __BIG_ENDIAN
	iplen = ippacket->ip_len;
#else
# error "Please fix <bits/endian.h>"
#endif

	switch(ippacket->ip_p) {
	case IPPROTO_TCP:

		tcppacket = (struct tcphdr *)(packet + datalink_size + IP_SIZE);
		tcp_size = tcppacket->doff * 4;
		payload = (u_char *)(packet + datalink_size + IP_SIZE + tcp_size);
		payload_len = iplen - IP_SIZE - tcp_size;

#ifdef TCPICK_DEBUG
		if( payload_len != (hdr->len - (int)( payload - packet ) ) ) {
		    suicide( "got_packet", 
			     "payload lenght calculated with iplen and hdr->len\n"
			     "differs by %d bytes\n"
			     "hdr->len = %d\n"
			     "datalink_size  = %d\n"
			     "IP_SIZE  = %d\n"
			     "iplen    = %d\n"
			     "tcp_size = %d\n"
			     "iplen - IP_SIZE - tcp_size = %d\n"
			     "(hdr->len - (int)( payload - packet ) = %d\n",
			     payload_len - (hdr->len - (int)( payload - packet )),
			     hdr->len,
			     datalink_size,
			     IP_SIZE,
			     iplen,
			     tcp_size,
			     payload_len,
			     (hdr->len - (int)( payload - packet ))
			    );
		}

#endif /* TCPICK_DEBUG */

		if( flags.header > 0 )
			display_header( stdout, ippacket, tcppacket, 
					payload_len );
		
		verify(); /* call the core to manage the packet */

		if( payload_len == 0 || SILENCE )
			goto end;

		out_flavour( flags.display_payload, 
			     stdout, payload, payload_len );
		
		break;

 	case IPPROTO_UDP:
 		udppacket = (struct udphdr *)(packet + datalink_size + IP_SIZE);
 		payload = (u_char *)(packet + datalink_size + IP_SIZE + sizeof(*udppacket)); 
 		payload_len = hdr->len - (int)( payload - packet );
 
 		if( payload_len == 0 || SILENCE )
			goto end;
 
 		out_flavour( flags.display_payload, 
 			     stdout, payload, payload_len );
 
 		break;
  				
		
	default: /* Packet with an unidentified protocol */
		debug( "Packet with unidentified protocol caught: %s > %s",
		       inet_ntoa(ippacket->ip_src),
		       inet_ntoa(ippacket->ip_dst) );
	}

end:
	
	if( flags.exitpackets != 0 ) {
		if (count_packets == flags.exitpackets)
			exit(EXIT_SUCCESS);
	}

	/* Now the callback is able to check! */
	expired_flag = CHECK_LISTENING;
}
