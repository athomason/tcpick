/*
 * debug.c -- debug functions
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
 * License, or (at your option) any later version.
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

void
print_conn_chain (struct CONN * f)
{
	while (f) {
		printf ("%x->",f);
		
		if (f == f->next) {
			printf ("inf\n");
			return;
		}
		f = f->next;
	}
	printf("NULL\n");
	return;
}

int
print_fragment_chain(struct FRAGMENT * f)
{
	while (f) {
		printf ("%x(%i,%i)->",f,f->off,f->len);

		if (f == f->next) {
			printf ("inf\n");
			return 1;
		}
		f = f->next;
	}
	printf("NULL\n");
	return 0;
}


