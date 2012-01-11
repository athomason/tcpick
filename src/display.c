/*
 * display.c -- displays data and status banners
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

/* FIXME: most code is duplicated: find a better solution! */

#include "tcpick.h"
#include "lookup.h"
#include "extern.h"

char * 
status2str( enum STATUS status )
/* converts the status flag to a string */
{
	char *str = (char *)S_calloc(1,128);
	switch(status) {
	case SYN_SENT:
		strcpy( str, "SYN-SENT" );
		break;
	case SYN_RECEIVED:
		strcpy( str, "SYN-RECEIVED" );
		break;
	case ESTABLISHED:
		strcpy( str, "ESTABLISHED" );
		break;
	case FIN_WAIT_1:
		strcpy( str, "FIN-WAIT-1" );
		break;
	case FIN_WAIT_2__CLOSE_WAIT:
		strcpy( str, "FIN-WAIT-2" );
		/* should it be CLOSE-WAIT? */
		break;
	case TIME_WAIT__LAST_ACK:
		strcpy( str, "TIME-WAIT" );
		/* should it be LAST-ACK? */
		break;
	case CLOSED:
		strcpy( str, "CLOSED" );
		/* should it be TIME-WAIT? */
		break;
	case RESET:
		strcpy( str, "RESET" );
		break;
	case EXPIRED:
		strcpy( str, "EXPIRED" );
		break;
	default:
		strcpy( str, "ERROR" );
		break;
	}
	
	return str;
}

int 
display_status( FILE * out, struct CONN * conn, enum STATUS status )
/* display a "status banner" */
{
	char *client_name;
	char *server_name;
	char *s_time;
	char *status_string;
	
	if ( flags.notstatus ) {
		/* FIXME: sucks! and what if out will be a file? */
		return 0;
	}

	client_name=(char *)lookup(conn->client.ip);
	
	s_time = (char *)S_calloc( 128, 1 );

	if( time_ascii( s_time ) )
		color( c_TIME, out,"%-16s ",s_time ); /* FIXME: check */

	color( c_NUMBERING, out,"%-6d ",conn->num );
	S_free ( s_time );
	
	status_string = status2str( status );
	
	color( _STATUS_COLOR(status),
		     out,"%-14s",status_string );
	
	S_free( status_string );
	
	color( c_CLIENTNAME_STATUS, out, " %s:%s ",
		     client_name,
		     getportname(conn->client.port) );
	
	color( _STATUS_COLOR(status), out,"> " );

	server_name=(char *)lookup(conn->server.ip);

	color( c_SERVERNAME_STATUS, out,"%s",
		     server_name );
	
	fprintf( out,":" );
		
	color( c_SERVICE, out,"%s",
		     getportname(conn->server.port) );
	
	fprintf( out, "\n" );
	fflush( out );
}
int display_header( FILE * out )
/* displays a banner for the header of the packet sniffed */
{ /* FIXME: sucks */
	char * source = NULL;
	char * dest   = NULL;
	char * s_time = (char *)S_calloc(128,1);
	source=(char *)lookup(ippacket->ip_src);
	debug("[display_header]: source(%x)",source);
	debug("[display_header]: source=%s",source);

	if( time_ascii(s_time) )
		color ( c_TIME, out,"%-16s ",s_time );


	color( c_CLIENTNAME_HEADER, out,"%s",source );
  
	fprintf(out,":");

	color( c_SERVICE_HEADER, out,"%s ",getportname(tcppacket->source) );

	if(tcppacket->urg)
		color(c_FLAGS, out,"U");
	if(tcppacket->ack)
		color(c_FLAGS, out,"A");
	if(tcppacket->psh)
		color(c_FLAGS, out,"P");
	if(tcppacket->rst)
		color(c_FLAGS, out,"R");
	if(tcppacket->syn)
		color(c_FLAGS, out,"S");
	if(tcppacket->fin)
		color(c_FLAGS, out,"F");

  
	dest = (char *)lookup( ippacket->ip_dst );

	color(c_DIRECTION, out," > ");

	color(c_SERVERNAME_HEADER, out,"%s",dest);

	fprintf(out,":");

	color(c_SERVICE_HEADER, out,"%s",getportname(tcppacket->dest));

	fprintf(out," (%i)\n",payload_len);

	fflush(out);

}

int
out_h( FILE * out, u_char * buf, int buflen )
/* hex-spaced mode */
{
#define CHAR (int)( * buf )
	while ( buflen ) {
		fprintf(out,"%2.2x ", CHAR);
		buf++;
		buflen--;
	}
	if ( DISPLAY_NL )
		fputc( '\n' , out );
}

int
out_hn( FILE * out, u_char * buf, int buflen )
/* non printable carachters are displayed as <hex> */
{
#define CHAR (int)( * buf )
	while ( buflen ) {
		if( ( isascii( CHAR ) && !iscntrl( CHAR ) ) || 
		    ( CHAR == '\n' ) || (CHAR == '\r') || (CHAR == '\t') )
			fputc( CHAR , out );
		else
			color(c_NONPRINT, out, "<%2.2x>", CHAR);
		buf++;
		buflen--;
	}
	if ( * ( --buf ) != '\n' )
		if ( DISPLAY_NL )
			fputc( '\n' , out );
}

int
out_p( FILE * out, u_char * buf, int buflen )
/* non printable carachters are displayed as dots */
{
#define CHAR (int)( * buf )
	while (buflen) {
		if( ( isascii( CHAR ) && !iscntrl( CHAR ) ) || 
		    ( CHAR == '\n' ) || (CHAR == '\r') || (CHAR == '\t') )
			fputc( CHAR , out );
		else
			color( c_NONPRINT, out, "." );
		buf++;
		buflen--;
	}
	if ( * ( --buf ) != '\n' )
		if ( DISPLAY_NL )
			fputc( '\n' , out );
}

/* WARNING: 
   if you want to modify out_xa, see carefully if it 
   affects out_x too */

int
out_xa (FILE * out, u_char * buf, int buflen)
{
	register int pos = 0; /* first character is position 0 */
	register int max;
	register int i;
	char a[16] = "................" ;

	color(c_HEXOFF0, out, "0x%-8.4x", 0);

	while( pos < buflen ) {
		do {
			if( pos < buflen ) {
				/* print hex stuff */
				if ( isgraph( *( buf + pos ) ) )
					fprintf(out, pos % 2 ? "%2.2x " : "%2.2x", *( buf + pos ) );
				else
					color( c_NONPRINT, out, pos % 2 ? "%2.2x " : "%2.2x", *( buf + pos ) );
				
				/* raw data */
				a[ pos % 16 ] = *( buf + pos );

			}
			else {
				/* hex space fill */
				fprintf(out, pos % 2 ? "   " : "  ");
			}
			pos++;
		} while ( pos % 16 );

		max = ( pos <= buflen ) ? 15 : buflen % 16 ;

		for(i = 0; max >= 0; max--) {
			if ( isgraph( a[i] ) )
				fputc( a[i], out );
			else
				color( c_NONPRINT, out, "." );
			i++;
		}

		fputc( '\n', out );

		if ( pos >= buflen )
			break;
		/* print offset at beginning of line */
		color(c_HEXOFF, out,"0x%-8.4x", pos);
	}
}

int
out_x (FILE * out, u_char * buf, int buflen)
/* hexdump */
{
	register int pos = 0; /* first character is position 0 */

	color(c_HEXOFF0, out, "0x%-8.4x", 0);

	while( pos < buflen ) {
		do {
			if( pos < buflen )
				/* print hex stuff */
				if ( isgraph( *( buf + pos ) ) )
					fprintf(out, pos % 2 ? "%2.2x " : "%2.2x", *( buf + pos ) );
				else
					color( c_NONPRINT, out, pos % 2 ? "%2.2x " : "%2.2x", *( buf + pos ) );
			else
				/* hex space fill */
				fprintf(out, pos % 2 ? "   " : "  ", *( buf + pos ) );
			pos++;
		} while ( pos % 16 );
		
		fprintf(out, "\n");
		
		if ( pos >= buflen )
			break;
		/* print offset at beginning of line */
		color(c_HEXOFF, out,"0x%-8.4x", pos);
	}
}
