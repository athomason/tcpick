/* 
   snip code for the terminal colors is taken from 
   http://linuxgazette.net/issue65/padala.html 

   Copyright (c)2001, Pradeep Padala.
   Copying license http://www.linuxgazette.com/copying.html
   Published in Issue 65 of Linux Gazette, April 2001

   Note: Francesco Stablum has modified the code on 15/01/04
   to take it useful for my program (tcpick) released under the GPL license.
*/

#include "tcpick.h" 
#include "extern.h"

void textcolor(FILE * out, int attr, int fg, int bg) 
{
	char command[13];
	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%dm", 0x1B, attr, fg + 30);
	fprintf(out,"%s", command);
}

void resetcolor(FILE * out) 
{
	/* works on gnome-terminal, xterm and rxvt */
	fprintf(out,"\x1b[0;0m");
}

void
color( int attr, int fg, FILE * out, char *fmt, ... )
{

#define COLOR_CONDITION ( ( out == stdout || out == stderr ) && flags.color )

	va_list list;

	if( COLOR_CONDITION )
		textcolor ( out, attr, fg, GREEN );

	va_start ( list, fmt );
	vfprintf ( out, fmt, list );
	va_end ( list );

	if( COLOR_CONDITION )
		resetcolor(out);
}


