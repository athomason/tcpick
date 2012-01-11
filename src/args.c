/*
 * args.c -- parse command-line arguments
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

#include "tcpick.h"
#include "extern.h"

#define GNU_OPTIONS    "ab::c:C::D::e:E::F:hi:npP:r:St::T::v::w::X:y::"
#define NONGNU_OPTIONS  "ab:c:CD:e:E:F:hi:npP:r:St:T:v:w:X:y:"

int 
usage()
{
	color(c_USAGE, stderr, USAGE);
	exit( TCPICK_SUCCESS );
}

int
parse_flav ( char c, enum FLAVOUR * flav )
{
	switch( c ) {
	case 'H':
	case 'P':
	case 'R':
	case 's':
	case 'U':
	case 'x':
	case 'X':
		if ( * flav == SILENT )
			* flav = c;
		return 1;
		break;
	default:
		return 0;
	}
}

void 
parse_write ( char * s )
{
#define WRITE_SIDE flags.writer.side
#define WRITE_FLAV flags.writer.flavour

	int i;
	char c;
	for(i = 0; i < strlen( s ) ; i++) {
		switch( c = *( s + i ) ) {
		case 'C':
			WRITE_SIDE = ( WRITE_SIDE == SERVER ) ? BOTH : CLIENT;
			break;
		case 'S':
			WRITE_SIDE = ( WRITE_SIDE == CLIENT ) ? BOTH : SERVER;
			break;
		case 's':
			flags.writer.type = SEPARATE;
			break;
		case 'u':
			flags.writer.type = UNIQUE;
			break;
		case 'b':
			flags.writer.banner = 1;
			break;
		default:
			if (! parse_flav( c, &(flags.writer.flavour) ) ) {
				ERR_FLAV( c );
			}
			break;
		}
	}

	if ( WRITE_FLAV == SILENT )
		WRITE_FLAV = RAW;
	if ( WRITE_SIDE == NOTHING )
		WRITE_SIDE = BOTH;
}

void 
parse_display_rebuild ( char * s )
{
#define REBUILD_SIDE flags.display_rebuild.side
#define REBUILD_FLAV flags.display_rebuild.flavour

	int i;
	char c;
	for(i = 0; i < strlen( s ) ; i++) {
		c = *( s + i );
		switch( c )	{
		case 'C':
			REBUILD_SIDE = ( REBUILD_SIDE == SERVER ) ? BOTH : CLIENT;
			break;
		case 'S':
			REBUILD_SIDE = ( REBUILD_SIDE == CLIENT ) ? BOTH : SERVER;
			break;
		default:
			if (! parse_flav( c, &(flags.display_rebuild.flavour) ) ) {
				ERR_FLAV( c );
			}
			break;
			
		}
	}

	if ( REBUILD_FLAV == SILENT )
		REBUILD_FLAV = RAW;
	if ( REBUILD_SIDE == NOTHING )
		REBUILD_SIDE = BOTH;
}

void
parse_args(int argc, char ** argv)
{
	int opt;

#ifdef HAVE_GETOPT_LONG
	static struct option long_options[]=
		{
			{"version",           no_argument,       &(flags.versionandquit),  1   },
			{"help",              no_argument,       &(flags.helpandquit),     1   },
			{"show-header",       no_argument,       0,                        'h' },
			{"separator",         no_argument,       &(flags.separator),       1   },
			{"interface",         required_argument, 0,                        'i' },
			{"readfile",          required_argument, 0,                        'r' },
			{"dirs",              required_argument, 0,                        'D' },
			{"filenaming",        required_argument, 0,                        'F' },
			{"show-offsets",      no_argument,       &(flags.displayoffsets),  1   },
			{"resolv-names",      no_argument,       0,                        'a' },
			{"numerical-services",no_argument,       0,                        'n' },
			{"colors",            no_argument,       0,                        'C' },
			{"pipe",              required_argument, 0,                        'P' },
			{0,0,0,0} /* end */
		};
#endif

	int option_index = 0;

	/* default values */
	memset( &flags, 0, sizeof(struct FLAGS) );

	flags.writer.flavour  = SILENT;
	flags.display_payload = SILENT;
	flags.display_rebuild.side = NOTHING;
	flags.display_rebuild.flavour = SILENT;
	flags.verbosity = 1;
	flags.trackonly = -1; /* infinite (-1) is default; -T0 means "don't track
				  anything" 
			       */
	flags.filenaming = 1;
  
	if ( argc <= 1 )
		printf("important: `man 8 tcpick' explains all options available\n");

#if HAVE_GETOPT_LONG /* GNU/Linux systems */
	while (( opt = getopt_long(argc,argv,
				   GNU_OPTIONS,
				   long_options, &option_index) ) != -1 )
#else /* BSD systems */
		printf("Your system doesn't support getopt_long() function; you will be "
		       "unable to use the long options\n"
		       " i.e. `% tcpick --interface ppp0' DOESN'T WORK! \n");
	while ( ( opt = getopt( argc, argv, NONGNU_OPTIONS ) ) !=-1 )
#endif
    
		switch(opt) {

		case 'a':
			flags.lookup = 1;
			break;
		case 'b':
			if( optarg != NULL )
				parse_display_rebuild ( optarg );
			else
				parse_display_rebuild ( "R" );
			break;
		case 'c':
			if (( flags.maxconns = atoi(optarg)) == 0)
				suicide( "parse_args", "%s: invalid number of maximum connections",optarg );
			else
				flags.maxconns = 1;
			break;
		case 'C':
			flags.color = 1;
			break;
		case 'D':
			if( optarg != NULL ) {
				flags.dirs = atoi(optarg);
				if( flags.dirs == 0 )
					suicide ("parse_args", "%s: invalid number of files per connection", optarg );
			} else {
				flags.dirs = 100;
			}
							       
			break;
		case 'e':
			if( optarg != NULL ) {
				flags.exitpackets = atoi(optarg);
				if ( flags.exitpackets == 0 ) {
					suicide ("parse_args",
						 "%s: invalid number of packets to exit", 
						 optarg);
				}
			}
			break;
			
		case 'E': /* WARNING: if you modify this section, you
			     should modify section case 'T' too */
			flags.exitclosed = 1; /* default value */

			if( optarg != NULL ) {
				if ( *optarg == 'f' ) {
					flags.exitclosed_first = 1; /* -Ef */
					if ( strlen(optarg) == 1 )  
                                        /* -Ef without number */
						break;
					else
						optarg++;           
                                        /* -Ef with number */
				}

				if (! ( flags.exitclosed = atoi(optarg) ) )
					suicide( "parse_args", 
						 "%s: invalid number of closed connection"
						 " to track before exiting", optarg );
			}
			break;
		case 'F': 
			if( optarg != NULL ) {
				flags.filenaming = atoi(optarg);
				if(flags.filenaming == 0) {
					suicide( "parse_args",
						 "%s: invalid file naming system", optarg );
				}
			} else {
				suicide( "parse_args",
					 "-F option needs a number\n"
					 "i.e. -F1 or -F2" );
			}
			
		case 'h':
			flags.header = 1;
			break;
		case 'i':
			dev = optarg;
			break;
		case 'n':
			flags.numericalports = 1;
			break;
		case 'p':
			flags.notpromisc = 1;
			break;
		case 'P':
			switch( *optarg ) {
			case 'c':
			case 'C':
				parse_display_rebuild ( "RC" ); /* -bRC */
				break;
			case 's':
			case 'S':
				parse_display_rebuild ( "RS" ); /* -bRS */
				break;
			default:
				suicide("parse_args", "%s: invalid --pipe (-P) option",optarg);
			}
			flags.notstatus        = 1; /* -S  */ 
			flags.verbosity        = 0; /* -v0 */
			flags.trackonly        = 1; /* -T1 */
			flags.trackonly_first  = 1; /* -Tf */
			flags.exitclosed       = 1; /* -E1 */
			flags.exitclosed_first = 1; /* -Ef */

			break;
		case 'r':
			readfile = optarg;
			break;
		case 'S':
			flags.notstatus = 1;
			break;
		case 't': /* timestamp */
			if( optarg ) {
				if( *optarg == 'd' ) {
					flags.displaytime = TIMEDATE;
				} else {
					suicide( "parse_args",
						 "%s: invalid timestamp option", optarg );
				}
			} else {
				flags.displaytime = ONLYTIME;
			}

			break;
				
				
					
		case 'T':
			/* WARNING: if you modify this section, you
			   should modify section case 'E' too */
			flags.trackonly = 1; /* default value */

			if( optarg ) {
				if ( *optarg == 'f' ) {
					flags.trackonly_first = 1; /* -Tf */
					if ( strlen(optarg) == 1 )  
                                        /* -Tf without number */
						break;
					else
						optarg++;           
                                        /* -Tf with number */
				}

				if (! ( flags.trackonly = atoi(optarg) ) ) {
					suicide( "parse_args",
						 "%s: invalid number of connections to track", optarg );
				}
			}
			break;
		case 'v':
			if ( optarg ) {
				flags.verbosity = strtol(optarg,(char **)NULL, 10);
				if ( flags.verbosity < 0 || flags.verbosity > 5 )
					suicide( "%s: invalid verbosity number",optarg );
			} else
				flags.verbosity = 1;
			break;
		case 'w':
			if( optarg )
				parse_write( optarg );
			else
				parse_write( "R" );
			break;
		case 'y':
			if( optarg ) { 
				if (! parse_flav( *optarg, &(flags.display_payload) ) )
					ERR_FLAV( *optarg );
			} else
				parse_flav( 'P', &(flags.display_payload) );
			break;
		case 'X':
			flags.timeout = atoi(optarg);
			break;

		default:
			; /* UNREACHED */
		}

	filter = argv[optind];

	if(flags.versionandquit) {
		color( c_USAGE, stdout, PACKAGE_STRING "\n"
			     " Author: " AUTHOR "\n"
			     " for other info try `man tcpick', or `%s --help'\n\n"
			     TERMS ,TCPICK_NAME);
		exit( TCPICK_SUCCESS );
	}

	if(flags.helpandquit) {
		usage();
		exit( TCPICK_SUCCESS );
	}
}
