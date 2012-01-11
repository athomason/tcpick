/*
 * quit.c -- code for program exiting
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

void
fault( char * func, char * desc )
/* function for error exiting by an
   _external_ problem (calls perror) */
{
	char * msg;
	char len;

	err( BUGREPORT );

	color( c_ERR, stderr, "FAULT:\t" );
	len = strlen(func) + strlen(desc) + 16;
	msg = S_malloc( len );
	sprintf( msg, "[%s] %s", func, desc );

	perror( msg );

	exit(TCPICK_FAILURE);
}

void
suicide( char * func, char * fmt, ... )
/* called for internal problems,
   or user errors */
{
	va_list list;

	color( c_ERR, stderr, "SUICIDE: " );
	
	fprintf( stderr, "[%s] ", func );

	va_start (list, fmt);
	vfprintf (stderr, fmt, list);
	va_end (list);

	fprintf (stderr, "\n");

	exit(TCPICK_FAILURE);
}

void
print_statistics()
{
	msg( 1, c_STATISTICS, "\n%d packets captured", count_packets);
	msg( 1, c_STATISTICS, "%d tcp sessions detected", count_conns);
}

void
cleanup()
/* cleanup function called by exit() for normal 
   process termination (set by atexit) */
{
	print_statistics();
}

void
exit_signal(int sig_type)
{
	debug("Signal type: %s", sys_siglist[sig_type]);
#ifdef HAVE_ATEXIT
	exit (EXIT_SUCCESS);
#else 
	print_statistics()
	exit (EXIT_SUCCESS);
#endif
}

