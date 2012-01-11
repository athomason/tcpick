/*
 * text.h -- text definitions
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

#define AUTHOR "Francesco Stablum `duskdruid' < "PACKAGE_BUGREPORT">"
#define EXAMPLEARGS "-i ppp0 -yP -C -h \"not port 22\""

#define TERMS 								\
 " tcpick is released under the GPL license\n"				\
 " and comes with ABSOLUTELY NO WARRANTY.\n"				\
 " This is free software, and you are welcome to redistribute it\n"	\
 " under certain conditions; for details read COPYING file.\n"

#define OPTIONS							   \
       "tcpick [ -a ] [ -n ] [ -C ]				\n"\
       "       [ -i interface ]					\n"\
       "       [ -yH ] [ -yP ] [ -yR ] [ -yU ] [ -yx ] [ -yX ]	\n"\
       "       [ -bH ] [ -bP ] [ -bR ] [ -bU ] [ -bx ] [ -bX ]	\n"\
       "       [ -wH ] [ -wP ] [ -wR ] [ -wU ]			\n"\
       "       [ -v  [ verbosity ]]				\n"\
       "       [ -S ] [ -h ] [ --separator ]			\n"\
       "       [  \"filter\" ] [ -r  file ]			\n"\
       "       [ --help ] [ --version ]                         \n"

#define MAILING_LIST \
 "mailing-list address:\n"			         			\
 "\t<tcpick-project@lists.sourceforge.net>\n"					\
 "Archive:\n"									\
 "\thttp://sourceforge.net/mailarchive/forum.php?forum=tcpick-project\n"	\
 "Subscribe:\n"									\
 "\thttp://lists.sourceforge.net/lists/listinfo/tcpick-project\n"		\

#define BUGREPORT 								\
 "Please send bugreports and patches to the following mailing-list:\n"   	\
 MAILING_LIST                                                                   \
 "don't forget to specify OS, kernel version and tcpick version!\n"

#define USAGE										\
 "\t" PACKAGE_STRING " is a sniffer tool written using libpcap.\n"			\
 "tcpick can keep track of tcp connection, sniff all tcp streams\n"			\
 "and store them to files, to show you what is happening on a network interface\n\n"	\
 "Usage: " OPTIONS								\
 "Example: tcpick " EXAMPLEARGS "\n\n"							\
 "for an updated list of options see tcpick(1) manpage\n"				\
 "to see version and license informations try `tcpick --version'\n"			\
 "or read the `COPYING' file, released with the package\n\n"				\
 "tcpick homepage: http://tcpick.sourceforge.net\n\n"					\
 MAILING_LIST										\
 "thank you for using tcpick!\n"

