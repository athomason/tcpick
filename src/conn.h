/*
 * conn.h -- enums and structs that describe a connection:
 * enum PART, struct CONN, struct HOST_DESC and enum STATUS
 * 
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

enum PART 
/* enum PART describes the side of a connection */
{
	NOTHING = 0,
	CLIENT,
	SERVER,
	BOTH
};

struct HOST_DESC { 
/* HOST_DESC describes the client or server part of a tcp session */
	u_int16_t port;
        struct in_addr ip;

	u_int32_t sip;
    /* why have I called them SIP and not SIN?
       well, SIP means Sequence Initial Pointer
       that is exactly the SIN plus one
    */
	FILE * file;     /* NULL means closed */
	char * filename;
	char * lockfilename;
	enum PART side;
	int wlen;
	
	struct FRAGMENT * unack;
	
	struct HOST_DESC * oth;
};

enum STATUS {
/* STATUS describes the status of a tcp session */
	NIHL = 0,
	SYN_SENT,
	SYN_RECEIVED,
	ESTABLISHED,
	FIN_WAIT_1,
	FIN_WAIT_2__CLOSE_WAIT,
	TIME_WAIT__LAST_ACK,
	LAST_ACK,
	CLOSED,
	RESET,
	EXPIRED
};

struct CONN
/* the struct CONN describes a tcp connection */
{
	enum STATUS status; 

	FILE *both_file; /* FIXME: wtf??? */

	int closer; /* when a connection is int open state this must be set to 0
		       when the client has sent the closing FIN, then closer=1
		       when it is the server closer=2
		    */
	int num; /* the number of this connection (useful for 
		    calculate the color tracked)
		 */

	struct HOST_DESC client;
	struct HOST_DESC server;
	
	struct CONN * next; /* pointer for the next ring of the chain */
	
	time_t lasttime; /* time of last packet in connection */ 
};


struct ip * ip_last_header; /* FIXME: check */
struct tcphdr * tcp_last_header;
