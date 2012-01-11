/*
 * globals.h -- global variables
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

struct pcap_pkthdr *pcapheader;
struct ip *ippacket;
struct tcphdr *tcppacket;
struct udphdr *udppacket;
u_char *payload;
int payload_len = 0;
struct FLAGS flags;

int count_packets = 0;
int count_conns; /* number of active connections */
int count_opened;



char *dl_str   = NULL;
char *dev      = NULL; /* name of the device to use */
char *readfile = NULL; /* name of the file to read */
char *net      = NULL; /* dot notation of the network address */
char *mask     = NULL; /* dot notation of the network mask */
char *filter   = NULL;

int ret   = 0;     /* return code*/
int datalink_id = 0;     /* datalink id */


int iplen    = 0;
int datalink_size  = 0;
int ip_size  = 0;
int tcp_size = 0;

struct CONN * first_conn;
struct CONN * last_conn;

enum EXPIRED_FLAG expired_flag;
