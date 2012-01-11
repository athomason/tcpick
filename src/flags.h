/*
 * flags.h -- the struct for the command-line flags of the program
 *
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

/* 
 * Copyright (C) 2003, 2004 Francesco Stablum (duskdruid)
 * <duskdruid@despammed.com> 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
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

enum FLAVOUR {
	RAW = 'R',
	PRINTABLE = 'P',
	HEX = 'H',
	HEX_NON_PRINT = 'U',
	HEX_DUMP = 'x',
	HEX_ASCII_DUMP = 'X' ,
	SILENT = 's',
	OTHER /* i.e. packet and status banners */
};

enum DISPLAYTIME {
	NOTHING_TIME = 0, /* default */
	ONLYTIME,
	TIMEDATE,
	ONLYDATE /* not implemented */
};

struct FLAGS {
	int verbosity;
	int file;
	int filenaming;
	int dirs;
	int maxconns; /* FIXME: should be developed; 
			 in the sense of SYN_SENT ?? */
	int trackonly;
	int trackonly_first;
	int exitpackets;
	int exitclosed;
	int exitclosed_first;
	enum DISPLAYTIME displaytime;
	int header;
	int separator;
	int displayoffsets;
	int notpromisc;
	int notstatus;
	enum FLAVOUR display_payload;
	struct {
		enum FLAVOUR flavour;
		enum PART side;
	} display_rebuild;
	struct {
		enum FLAVOUR flavour;
		enum {SEPARATE = 0, UNIQUE} type;
		int banner;
		enum PART side;
	} writer;
	int helpandquit;
	int versionandquit;
	int lookup;
	int numericalports;
	int color;
	int colortrack;
	int timeout;
};


