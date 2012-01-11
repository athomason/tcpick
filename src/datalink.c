/*
 * datalink.c -- calculates datalink offsets
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

char *datalink2str(int dl_id)
{
	char *str = NULL;
	if (str)
	{
		S_free (str);
		str = NULL;
	}
	str = S_calloc(128 , 1);

	debug( "datalink number: %d" , dl_id );
	switch(dl_id) {
#ifdef DLT_NULL
	case DLT_NULL:
		strcpy(str, "DLT_NULL");
		break;
#endif
#ifdef DLT_EN10MB
	case DLT_EN10MB:
		strcpy(str, "DLT_EN10MB");
		break;
#endif
#ifdef DLT_IEEE802
	case DLT_IEEE802:
		strcpy(str, "DLT_IEEE802");
		break;
#endif
#ifdef DLT_ARCNET
	case DLT_ARCNET:
		strcpy(str, "DLT_ARCNET");
		break;
#endif
#ifdef DLT_FDDI
	case DLT_FDDI:
		strcpy(str, "DLT_FDDI");
		break;
#endif
#ifdef DLT_ATM_RFC1483
	case DLT_ATM_RFC1483:
		strcpy(str, "DLT_ATM_RFC1483");
		break;
#endif
#ifdef DLT_RAW  
	case DLT_RAW:  
		strcpy(str, "DLT_RAW"); 
		break; 
#endif
#ifdef DLT_PPP_SERIAL
	case DLT_PPP_SERIAL:
		strcpy(str, "DLT_PPP_SERIAL");
		break;
#endif
#ifdef DLT_PPP_ETHER
	case DLT_PPP_ETHER:
		strcpy(str, "DLT_PPP_ETHER");
		break;
#endif
#ifdef DLT_C_HDLC
	case DLT_C_HDLC:
		strcpy(str, "DLT_C_HDLC");
		break;
#endif
#ifdef DLT_IEEE802_11
	case DLT_IEEE802_11:
		strcpy(str, "DLT_IEEE802_11");
		break;
#endif
#ifdef DLT_LOOP
	case DLT_LOOP:
		strcpy(str, "DLT_LOOP");
		break;
#endif
#ifdef DLT_LINUX_SLL
	case DLT_LINUX_SLL:
		strcpy(str, "DLT_LINUX_SLL");
		break;
#endif
#ifdef DLT_LTALK
	case DLT_LTALK:
		strcpy(str, "DLT_LTALK");
		break;
#endif
#ifdef DLT_PFLOG
	case DLT_PFLOG:
		strcpy(str, "DLT_PFLOG");
		break;
#endif
#ifdef DLT_PPP
	case DLT_PPP:
		strcpy(str, "DLT_PPP");
		break;
#endif 
#ifdef DLT_SLIP
	case DLT_SLIP:
		strcpy(str, "DLT_SLIP");
		break;
#endif
#ifdef DLT_SLIP_BSDOS
	case DLT_SLIP_BSDOS:
		strcpy(str, "DLT_SLIP_BSDOS");
		break;
#endif
#ifdef DLT_PPP_BSDOS
	case DLT_PPP_BSDOS:
		strcpy(str, "DLT_PPP_BSDOS");
		break;
#endif

	default:
		sprintf(str, "UNKNOWN(0x%x)(%d)" , dl_id , dl_id);
		break;

	}
	return str;
}

int datalink2off(int dl_id)
{
	int dl_size = 0;
	switch(dl_id) {

#ifdef DLT_NULL /* not tested */
	case DLT_NULL:
		dl_size = 4;
		break;
#endif
#ifdef DLT_EN10MB
	case DLT_EN10MB:
		dl_size = 14;
		break;
#endif
#ifdef DLT_RAW
	case DLT_RAW:
		dl_size = 0;
		break;
#endif
#ifdef DLT_IEEE802_11 /* not tested */
	case DLT_IEEE802_11:
		dl_size = 24;
		break;
#endif
#ifdef DLT_LOOP /* not tested */
	case DLT_LOOP:
		dl_size = 4;
		break;
#endif
#ifdef DLT_PPP_ETHER
	case DLT_PPP_ETHER:
		dl_size = 8; 
		break;
#endif
#ifdef DLT_LINUX_SLL
	case DLT_LINUX_SLL:
		dl_size = 16; 
		break;
#endif
#ifdef DLT_PFLOG 
	case DLT_PFLOG:
		dl_size = 48;
		break;
#endif

/* credits for DLT_PPP, DLT_SLIP, DLT_SLIP_BSDOS
   and DLT_PPP_BSDOS offsets:
   SNiFf v0.3 by uLiX
   http://www.s0ftpj.org/bfi/online/bfi10/BFi10-05.html
*/
#ifdef DLT_PPP
	case DLT_PPP:
		dl_size = 4;
		break;
#endif
#ifdef DLT_SLIP
	case DLT_SLIP:
		dl_size = 16;
		break;
#endif
#ifdef DLT_SLIP_BSDOS
	case DLT_SLIP_BSDOS:
		dl_size = 24;
		break;
#endif
#ifdef DLT_PPP_BSDOS
	case DLT_PPP_BSDOS:
		dl_size = 24;
		break;
#endif

	default:
		suicide("datalink2off", 
			"Sorry, datalink %s type not recognized by %s !\n", 
			datalink2str(dl_id), PACKAGE_STRING);
		
	}
	return dl_size;
}
