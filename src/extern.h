/*
 * extern.h -- external variables
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

/* FIXME: it seems written by a ogre */
extern struct ip *ippacket;
extern struct tcphdr *tcppacket;
extern struct udphdr *udppacket;
extern u_char *payload;
extern int payload_len;
extern struct FLAGS flags;
extern int count_packets;
extern int count_conns;
extern int count_opened;

extern char * dev;
extern char * readfile;
extern char * filter;

extern int iplen;
extern int datalink_size;
extern int ip_size;
extern int tcp_size;
/* extern int data_trasl; */

extern struct CONN * first_conn;
extern struct CONN * last_conn;

extern enum EXPIRED_FLAG expired_flag;
