/*
 * timer.c -- functions and callbacks for the 
 *            timer managed by SIGALRM
 *
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004, 2005  Francesco Stablum
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

#include "tcpick.h"
#include "extern.h"

#define CURRENT_CONN (prev_conn->next)

void set_timer()
{

#define TIMER_ERR_MSG err( "unable to set timer properly.\n" \
			   "\t-X option and EXPIRED status will be disabled" )

#ifdef SIGALRM

	extern void sigalrm_callback(int);

# if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (TEST_BSD)

	/* BSD stuff*/

	pid_t pid;
 	pid = getpid();

	if (fork() == 0) {
		/* child process */
		for(;;) {
			sleep(1);
			if( kill(pid, SIGALRM) == -1) {
				if( errno == ESRCH)
					_exit(EXIT_SUCCESS);
				else
					sorry("set_timer", "kill(%d, SIGALRM) returned -1");
			}
		}
	}
	
# else 
#  ifdef HAVE_SETITIMER
	
/* Every second will be called the function that checks if there are
 * expired connections.
 */
	{
		struct itimerval * expired_timer = (struct itimerval *) 
			malloc(sizeof(struct itimerval));
		
		expired_timer->it_interval.tv_sec=1;
		expired_timer->it_interval.tv_usec=1;
		expired_timer->it_value.tv_sec=1;
		expired_timer->it_value.tv_usec=1;
	
		if ( setitimer( ITIMER_REAL, expired_timer, NULL ) == -1 ) {
			sorry("main", "setitimer");
			TIMER_ERR_MSG ;
		}
	}
#  else
	TIMER_ERR_MSG ;
#  endif /* HAVE_SETITIMER */
# endif /* BSD stuff */

	signal_setup (SIGALRM, sigalrm_callback);
		
#else
	TIMER_ERR_MSG ;	
#endif /* SIGALRM */
	
}

void check_expired()
/* called by sigalrm_callback or got_packet.
   this is the engine to search expired connections (delete only 1) */
{
	register struct CONN * prev_conn;
	time_t now;
	
	now = time(NULL);
	
	prev_conn = first_conn;
	
	while ( CURRENT_CONN ) {
		if (now - CURRENT_CONN->lasttime > flags.timeout) {
			status_switch( prev_conn, EXPIRED );
			prev_conn = first_conn;
			continue;
		}
		prev_conn = prev_conn->next;
	}
}

void sigalrm_callback(int sig_type)
{
	if(expired_flag == CHECK_LISTENING) {
		/* we are listening to the socket
		   in the blocking mode now */
		check_expired();
	} else {
		expired_flag = CHECK_GOTPACKET;
	}
}
