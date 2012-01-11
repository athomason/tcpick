/*
 * tcpick.c -- main project file
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
 *
 * Please read the COPYING file for the license details!
 */

/*
 * tcpick project needs somebody suggesting me bugs and new features!
 * If you want to contribute sending patches, finding bugs, compilation
 * errors, platform-specific incompatibilies you are invited to the tcpick
 * mailing-list:
 *
 * email address: 
 *   <tcpick-project@lists.sourceforge.net>
 * Archive:
 *   http://sourceforge.net/mailarchive/forum.php?forum=tcpick-project
 * Subscribe:
 *   http://lists.sourceforge.net/lists/listinfo/tcpick-project
 */

/*
 * in the code you will find many (sooo many) FIXME's: it means that
 * I (or somebody else) should improve the code.
 */

#include "tcpick.h"
#include "globals.h"
#include "datalink.h"

char *errbuf[PCAP_ERRBUF_SIZE];
struct bpf_program filter_compiled;
bpf_u_int32 netp; /* ip */
bpf_u_int32 maskp; /* subnet mask */
struct in_addr addr;
char *other_args = NULL;
pcap_t *descr;
const u_char *packet;
u_char *curr_char;
struct pcap_pkthdr hdr;
u_char *datalink_str;

extern void got_packet();
extern void exit_signal(int);

void signal_setup(int sig, void (*handler)(  ))
{
#ifdef HAVE_SIGACTION
	struct sigaction sa; 
	sa.sa_handler = handler;
	sigemptyset(&(sa.sa_mask));
	sigaddset(&(sa.sa_mask), sig);
	sa.sa_flags = 0;
	if( sigaction(sig, &sa, 0) == -1)
		sorry("signal_setup", "sigaction(%d,%p,0) returned -1");
#else
#ifdef HAVE_SIGNAL
	if (signal(sig, handler) == SIG_ERR)
		sorry("signal_setup", "signal(%d,%p) returned SIG_ERR");
#endif /* HAVE_SIGNAL */
#endif /* HAVE_SIGACTION */
}

int main(int argc, char **argv) 
{
	char tbuf[128];
	struct tm *tm;
	time_t now;
		
	parse_args(argc, argv);

	/* set a timer that sends every second a SIGALRM signal */
	set_timer();
	
#ifdef HAVE_ATEXIT
	{
		extern void cleanup();
		
		if (atexit (cleanup) != 0 ) {
			sorry("main", "atexit returned a nonzero value");
		}
	}
	
#endif

#ifdef SIGINT
	signal_setup (SIGINT,  exit_signal);
#endif
#ifdef SIGTERM
	signal_setup (SIGTERM,  exit_signal);
#endif
#ifdef SIGQUIT
	signal_setup (SIGQUIT,  exit_signal);
#endif
#ifdef SIGHUP 
	/* TODO: maybe another use for this signal */
	signal_setup (SIGHUP,  exit_signal);
#endif

	/* time stuff stolen by Fyodor's nmap */
	now = time(NULL);
	if (!(tm = localtime(&now)))
		fault("main", "Unable to get current localtime()");
	if (strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M %Z", tm) <= 0)
		fault("main", "Unable to properly format time");
	
	msg( 1, c_WELCOME,
	     "Starting " PACKAGE_STRING " at %s", tbuf );

#ifdef TCPICK_DEBUG
	msg( 1, c_COMPILED,
	     "Compiled with -DTCPICK_DEBUG" );
#endif
	
	if( flags.trackonly > -1 ) {
		msg( 1, c_SETTING,
		     "Number of connections that will be tracked: %i",
		     flags.trackonly );
	}
	if( flags.timeout <= 0 ) { /* thank you Artyom Khafizov :^) */
		flags.timeout = 600;
	}
	msg( 1, c_SETTING, "Timeout for connections is %i", flags.timeout );


	if( flags.exitpackets != 0 ) {
		msg( 1, c_SETTING,
		     "when %d packets will be sniffed, "
		     "tcpick exits", flags.exitpackets);
	}
	if( flags.exitclosed ) {
		if( flags.exitclosed_first )
			msg(1, c_SETTING, 
			    "when _the first_ %i tracked "
			    "connections will be closed, tcpick exits",
			    flags.exitclosed );
		else
			msg(1, c_SETTING, 
			    "when %i tracked connections "
			    "will be closed, tcpick exits",
			    flags.exitclosed );
	}
	/* I hope this was clear enough */
			
	if( ( dev != NULL ) && ( readfile != NULL) ) {
		suicide( "main",
			 "Please specify either a file (with -r <filename>)\n"
			 "\t or an interface (with -i <int>), not both" );
	}
	
	if( ( dev == NULL ) && ( readfile == NULL ) ) {
		
		/* find first available network device */
		dev = pcap_lookupdev( (char*) errbuf );
		
		if( dev == NULL ) {
			suicide( "main", errbuf );
		}
		
	} else if( readfile != NULL ) {
		setuid(getuid());
		msg( 1, c_INTERFACE, 
		     "%s: reading from %s", 
		     TCPICK_NAME, readfile );

		descr = pcap_open_offline( readfile, (char *)errbuf );
	}
	
	if( dev != NULL) {

		debug( "Using device: %s", dev );

		msg( 1, c_INTERFACE, 
		     "%s: listening on %s", 
		     TCPICK_NAME, dev );

		/* ask pcap for the network address and mask of the device */
		ret = pcap_lookupnet( dev, &netp,
				      &maskp, (char *)errbuf);
		if( ret == -1 ) {
			netp = 0;
			maskp = 0;
			err( "%s", errbuf );
		}

		addr.s_addr = netp;
		net = (char *)strdup( inet_ntoa(addr) );
		if(! net )
			fault( "main", "inet_ntoa" );

		addr.s_addr = maskp;
		mask = (char *)strdup( inet_ntoa(addr) );
		if (! mask ) {
			fault( "main", "inet_ntoa" );
		}
		
		debug( "net: %s", net );
		debug( "mask: %s", mask ); 

		descr = pcap_open_live( dev,
					BUFSIZ, 
					flags.notpromisc ? 0 : 1 ,
					1000, 
					(char *)errbuf );
	}


	if( descr == NULL)
		suicide( "main", errbuf );

	/* compiling the filter */
	if( filter != NULL ) {
		msg(1, c_SETTING, "setting filter: \"%s\"",filter);
		
		if( ( pcap_compile( descr,
				    &filter_compiled,
				    filter, 
				    0, 
				    (int)net 
			      ) == -1) )
			err("error compiling filter \"%s\"",filter);

		pcap_setfilter( descr, &filter_compiled );
	}

	/* getting information about the datalink type of the device choosen 
	   (not all are supported) */
	datalink_id = pcap_datalink( descr );
	datalink_str = (char *)datalink2str( datalink_id );
	debug( "datalink: %s", datalink_str );
	datalink_size = datalink2off( datalink_id );
	debug ( "datalink header size: %d", datalink_size );

	/* setting entry point of linked-list of connections tracked */
	first_conn = (struct CONN *)S_calloc( sizeof(struct CONN), 1 );
	last_conn = first_conn;

	/* see timer.c for further details */
	expired_flag = CHECK_LISTENING;

	/* setting function that analyzes captured packets */
	pcap_loop( descr, -1, got_packet, NULL ); 
 
	/* this point should not be reached, 
	   unless errors or -r option */

	if( readfile ) {
		msg( 1, c_DONE,
		     "%s: done reading from %s", TCPICK_NAME, readfile );
		exit ( TCPICK_SUCCESS );
	} else {
		suicide( "main", "exited pcap loop" );
	}

	/* NOT REACHED */
	return TCPICK_FAILURE;
}
