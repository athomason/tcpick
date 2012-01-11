/*
 * tcpick.h -- main header
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#ifdef HAVE_NET_IF_H
# include <net/if.h>
#endif

#ifdef HAVE_NET_BPF_H
# include <net/bpf.h>
#else
# ifdef HAVE_PCAP_BPF_H
#  include <pcap-bpf.h>
# endif /* HAVE_PCAP_BPF_H */
#endif /* HAVE_NET_BPF_H  */

#ifdef HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif

#ifdef HAVE_NETINET_IF_ETHER_H
# include <netinet/if_ether.h>
#endif

#ifdef HAVE_NETINET_IP_H
# include <netinet/ip.h>
#else
# include "ip.h"
#endif /* HAVE_NETINET_IP_H */

#ifdef HAVE_NETINET_UDP_H
# include <netinet/udp.h>
#else
# include "udp.h"
#endif /* HAVE_NETINET_UDP_H */

#include <netdb.h>

#ifdef HAVE_PCAP_PCAP_H
# include <pcap/pcap.h>
#else
# ifdef HAVE_PCAP_H
#  include <pcap.h>
# endif /* HAVE_PCAP_H */
#endif /* HAVE_PCAP_PCAP_H */

#ifdef HAVE_TIME_H
# include <time.h>
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#include "def.h"
#include "tcp.h"
#include "fragment.h"
#include "conn.h"
#include "text.h"
#include "colors.h"
#include "flags.h"
#include "lookup.h"
#include "timer.h"
