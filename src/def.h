/*
 * def.h -- definitions
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

/* Do you wanna debug tcpick? Oh, thank you very much :^)
 * Uncomment the following line to add 
 * _really_ noisy debug messages
 */

//#define TCPICK_DEBUG 1

#define TCPICK_NAME "tcpick"

#define MAX_FILENAME_LENGHT 512

#define MAXCONN 16384
#define MAXMALLOC 16777216

#define S_calloc calloc
#define S_malloc malloc
#define S_free   free

#define SEPARATOR "----------------"

#define REBUILD \
 ( ( flags.display_rebuild.flavour != SILENT ) || \
   ( flags.writer.flavour != SILENT  ) )

/* FIXME: mumble mumble */
#define DISPLAY_NL \
( (( flags.header ) || ( flags.separator ) || (! flags.notpromisc )) && ( out == stdout ) ) /* lisp? */

#define TCPICK_SUCCESS EXIT_SUCCESS
#define TCPICK_FAILURE EXIT_FAILURE

#define SILENCE (flags.verbosity == 0) /* FIXME MAY WRONG!! */
#define WRITE_SOME_BANNER ((flags.writer.flavour == OTHER) || (flags.writer.headers == 1) || (flags.writer.headers == 1))
#define WFLAV (flags.writer.flavour)

#define ERR_FLAV(c) \
 err( "Unrecognized option: %c\n", c );

#ifndef MAX
# define MAX(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef TCPICK_DEBUG
# define debug(fmt, ...) /* see msg.c for #ifdef TCPICK_DEBUG */
#endif /* TCPICK_DEBUG */

#define IP_SIZE 20
