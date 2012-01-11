/*
 * write.c -- chooses output data
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

__inline__ char * 
avail_filename(struct CONN * conn_ptr, 
	       enum PART side, char * ext)
/*
  finds an available name for the connection file,
  i.e. sometimes there are same filenames, this is useful
  to invent a different filename
*/
{
	char * client_server;
	char * base_name;
	char * dir_name;
	char * test_name;
	char * ret;
	struct stat stat_struct;
	/* this gets the name of the host */
	char * clientname;
	char * servername;
	int server_port=conn_ptr->server.port;
	int num = 0;
  
	client_server = (char *)S_calloc(MAX_FILENAME_LENGHT,1);
	base_name     = (char *)S_calloc(MAX_FILENAME_LENGHT,1);
	dir_name      = (char *)S_calloc(MAX_FILENAME_LENGHT,1);
	test_name     = (char *)S_calloc(MAX_FILENAME_LENGHT,1);
	
	if (flags.dirs > 0) {
		/* -D option */
		sprintf(dir_name,"%06u", conn_ptr->num / flags.dirs);
		if( stat(dir_name, &stat_struct) == -1 ) {
			switch (errno) {
			case ENOENT:
				/* directory don't exist: create it! */
				if( mkdir(dir_name, 666) == -1 )
					fault("avail_filename", "mkdir");
				break;
			default:
				fault("avail_filename", "stat");
			}
		}

	} else {
		sprintf(dir_name,".");
	}

	clientname = (char *)lookup(conn_ptr->client.ip);
	sprintf(client_server,"%s", clientname);

	servername=(char *)lookup(conn_ptr->server.ip);
	sprintf(client_server,"%s_%s_%s",
		client_server,
		servername,
		getportname(server_port)
		);

	if(flags.writer.type == UNIQUE) {
		/* this was Saumil Shah's wish ;^) */
		switch( flags.filenaming ) {
		case 1:
			sprintf(base_name, "%s/tcpick_%s.both", dir_name, client_server);
			break;
		case 2:
			sprintf(base_name, "%s/tcpick_%06u_%s.both", dir_name, conn_ptr->num, client_server);
			break;
		}
	} else { /* flags.writer.type == SEPARATE,
		  * so, give different filenames! 
		  * this is by default.
		  */

		switch( flags.filenaming ) {
		case 1:
			switch( side ) {
			case CLIENT:

				sprintf(base_name, "%s/tcpick_%s.clnt", dir_name, client_server);
				break;
				
			case SERVER:
				
				sprintf(base_name,"%s/tcpick_%s.serv", dir_name, client_server);
				break;
			}
			break;
			
		case 2:
			switch( side ) {
			case CLIENT:
				
				sprintf(base_name, "%s/tcpick_%06u_%s.clnt", dir_name, conn_ptr->num, client_server);
				break;
				
			case SERVER:
				
				sprintf(base_name,"%s/tcpick_%06u_%s.serv", dir_name, conn_ptr->num, client_server);
				break;
			}
			break;
		}
	}

	sprintf( test_name, "%s.%s", base_name, ext );
	while(! stat( test_name, &stat_struct )) {
		num++;
		sprintf( test_name, "%s.%x.%s", base_name, num, ext );
	}

	/* returning the right pointer and freeing wasted memory */
	
#ifdef HAVE_STRNDUP
	ret = (char *)strndup( test_name, MAX_FILENAME_LENGHT );
#else
	ret = (char *)strdup( test_name );
#endif
	S_free( test_name );
	S_free( dir_name );
	S_free( base_name );
	S_free( client_server );
	return ret;
}

void
open_file( struct CONN * conn_ptr, 
		  struct HOST_DESC * desc )
{
	static char * s;
	FILE * lockfile;
	/* preparing for the connection side file */
	if(flags.writer.type == UNIQUE 
	   && desc->oth->file != NULL ) {
		/* user want  a unique output file, 
		   and it was just opened! */
		desc->file = desc->oth->file;
		desc->filename = desc->oth->filename;
	} else {
		s = (char *)avail_filename( conn_ptr, desc->side, "dat" );
		
		desc->file = fopen( s , "a" );
		if( desc->file == NULL )
			fault("open_file", "fopen returned NULL");
		desc->filename = s;

		s = (char *)avail_filename( conn_ptr, desc->side, "lck");
		
		lockfile = fopen( s , "a" );
		if( lockfile == NULL )
			fault("open_file", "fopen returned NULL");
		fclose(lockfile);
		desc->lockfilename = s;
	}
	return;
}

__inline__ int
flowflush ( struct CONN * conn_ptr, 
	    struct HOST_DESC * desc, 
	    u_char * buf,
	    int buflen )
/* data that are acknowledged come here in order to be written to
   their file, or to stdout */
{
	/* -b set of options */
	if ( desc->side == flags.display_rebuild.side )
		
		out_flavour ( flags.display_rebuild.flavour,
			      stdout, buf, buflen );
	
	else if ( flags.display_rebuild.side == BOTH )
		
		out_flavour ( flags.display_rebuild.flavour,
			      stdout, buf, buflen );
	
	/* -w set of options */
	if ( desc->side == flags.writer.side ) {
		
		if(! desc->file ) 
			open_file( conn_ptr, desc ); 
		
		out_flavour ( flags.writer.flavour,
			      desc->file, buf, buflen );
	}
	else if ( flags.writer.side == BOTH ) {
		
		if(! desc->file )
			open_file( conn_ptr, desc ); 

		if ( flags.writer.banner == 1 ) {
		/* -wb */
			/* FIXME: add it to `-b' too? Do a function for this? */
			fprintf(desc->file, 
				(desc->side == CLIENT)
				? "\n[client] %d:%d (%d)\n" 
				: "\n[server] %d:%d (%d)\n",
				desc->wlen,
				desc->wlen + buflen,
				buflen);
		}
		
		out_flavour ( flags.writer.flavour,
			      desc->file, buf, buflen );
	}
}

int 
out_flavour( enum FLAVOUR flavour, 
	     FILE * out, 
	     u_char * buf, 
	     int buflen )
/* chooses the function to call, according to the specified flavour */
{
	if( flags.separator && ( out == stdout ) ) /* FIXME: sucks? */
		color( c_SEPARATOR, stdout, SEPARATOR "\n" );

	switch ( flavour ) {
	case HEX_ASCII_DUMP:
		out_xa( out, buf, buflen );
		break;
	case HEX_DUMP:
		out_x( out, buf, buflen );
		break;
	case RAW:
		fwrite( buf, sizeof(u_char), buflen, out );
		if( DISPLAY_NL )
			fprintf( out, "\n" );
		break;
	case PRINTABLE:
		out_p( out, buf, buflen );
		break;
	case HEX:
		out_h( out, buf, buflen );
		break;
	case HEX_NON_PRINT:
		out_hn( out, buf, buflen );
		break;
	default: /* SILENT ??? */
		break;
	}
}
