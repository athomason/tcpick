/*
 * fragment.h -- the fragment_t linked-list:
 *                 used to store unacknowledged data
 *
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

enum fr_flag {
	BREAK = 0,
	CONTINUE,
};

struct FRAGMENT
{
	int off;
	u_char * payload;
	int len;
	enum fr_flag flag;
	struct FRAGMENT * next;
};


