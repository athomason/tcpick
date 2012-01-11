/*
 * time.c -- manages timestamps
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

char * time_ascii(char * ret)
{

  struct timeval  *tp;
  struct timezone *tzp;
  struct tm * brokentime;

  if(flags.displaytime == NOTHING_TIME)
	  goto retNULL;

  tp  = (struct timeval  * ) S_malloc( sizeof(struct timeval) );
  tzp = (struct timezone * ) S_malloc( sizeof(struct timezone) );

  memset(tp,  0, sizeof(struct timeval));
  memset(tzp, 0, sizeof(struct timeval));
  
  if(gettimeofday(tp, tzp)) {

	  perror("gettimeofday returned not 0!");
	  goto retNULL;

  } else {
	  brokentime = localtime(&(tp->tv_sec));
	  switch (flags.displaytime) {
		  
	  case ONLYTIME:
		  sprintf(ret,"%2.2d:%2.2d:%2.2d.%6.6d",
			  brokentime->tm_hour,
			  brokentime->tm_min,
			  brokentime->tm_sec,
			  tp->tv_usec
			  );
		  break;

	  case TIMEDATE:
		  sprintf(ret,
			  "%2.2d-%2.2d-%2.2d " /* date */
			  "%2.2d:%2.2d:%2.2d.%6.6d", /* time */

			  /* date */
			  brokentime->tm_mday,
			  brokentime->tm_mon + 1,

			  brokentime->tm_year > 100 ? 
			  brokentime->tm_year - 100 :
			  brokentime->tm_year,

			  /* time */
			  brokentime->tm_hour,
			  brokentime->tm_min,
			  brokentime->tm_sec,
			  tp->tv_usec
			  );
		  break;
		  
	  default:

#ifdef TCPICK_DEBUG
		  suicide("time_ascii",
			  "unimplemented feature required.");
#endif
		  goto retNULL;
	  }
  }

  S_free ( tp ); 
  S_free ( tzp );

  return ret;

retNULL:
  sprintf(ret,"");
  return NULL;

}

