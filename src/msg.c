/*
 * exit.c -- code for program exiting
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

#ifdef TCPICK_DEBUG
void
debug( char *fmt, ... )
/* this function has been provided by ShackaN
   thank you very much :-) */
{
	va_list list;
	printf ("debug: ");
	va_start (list, fmt);
	vprintf (fmt, list);
	va_end (list);
	printf ("\n");
}
#else
# define debug(fmt, ...)
#endif /* TCPICK_DEBUG */

void
err( char *fmt, ... )
{
	va_list list;
	color( c_ERR, stderr, "ERROR: " );
	va_start( list, fmt );
	vfprintf( stderr, fmt, list );
	va_end ( list );
	fprintf ( stderr, "\n" );
}

void
msg( int v, int attr, int fg, char *fmt, ... )
{
	va_list list;

	if( flags.verbosity < v )
		return;

	if( flags.color )
		textcolor ( stdout, attr, fg, GREEN );

	va_start ( list, fmt );
	vfprintf ( stdout, fmt, list );
	va_end ( list );

	if( flags.color )
		resetcolor( stdout );
	fprintf( stdout, "\n" );
}

void
sorry ( char * func, char * desc )
/* function for error by an
   _external_ problem (calls perror) 
   this is like fault(..) but doesn't exit.
   The sorry() function is used for light problems
   that don't compromise program execution.
*/
{
	char * msg;
	char len;

	color( c_ERR, stderr, "SORRY:\t" );
	len = strlen(func) + strlen(desc) + 16;
	msg = S_malloc( len );
	sprintf( msg, "[%s] %s", func, desc );

	perror( msg );

	free(msg);
}

