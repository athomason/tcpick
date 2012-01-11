/*
 * colors.h -- color codes definitions
 * Part of the tcpick project
 *
 * Author: Francesco Stablum <duskdruid @ despammed.com>
 *
 * Copyright (C) 2003, 2004  Francesco Stablum
 * Licensed under the GPL
 *
 */

#define RESET     0
#define BRIGHT    1
#define DIM       2
#define UNDERLINE 3
#define BLINK     4
#define REVERSE   7
#define HIDDEN    8

/* black and white should NOT be used because of 
   black or white backgrounds of terminals */

//#define BLACK     0
#define   RED       1
#define   GREEN     2
#define   YELLOW    3
#define   BLUE      4
#define   MAGENTA   5
#define   CYAN      6
//#define WHITE     7

#define _STATUS_COLOR(s) BRIGHT, s == ESTABLISHED ? GREEN : \
                                 s == CLOSED      ? RED   : \
                                 s == RESET       ? RED   : \
                                 s == EXPIRED     ? BLUE  : \
                                                    YELLOW
#define	c_WELCOME           BRIGHT, CYAN
#define	c_COMPILED          BRIGHT, RED
#define c_INTERFACE         BRIGHT, GREEN
#define c_SETTING           BRIGHT, MAGENTA
#define c_DONE              BRIGHT, MAGENTA
#define c_USAGE             BRIGHT, BLUE
#define	c_ERR               BRIGHT, RED
#define	c_NUMBERING         BRIGHT, MAGENTA
#define c_CLIENTNAME_STATUS BRIGHT, GREEN
#define c_SERVERNAME_STATUS BRIGHT, CYAN
#define c_SERVICE           BRIGHT, BLUE
#define c_TIME              BRIGHT, CYAN
#define c_CLIENTNAME_HEADER BRIGHT, GREEN
#define c_SERVERNAME_HEADER RESET , CYAN
#define c_SERVICE_HEADER    BRIGHT, BLUE
#define c_HEXOFF            BRIGHT, CYAN
#define c_HEXOFF0           BRIGHT, BLUE
#define c_NONPRINT          RESET,  RED
#define c_FLAGS             BRIGHT, RED
#define c_DIRECTION         BRIGHT, MAGENTA
#define c_SEPARATOR         RESET,  GREEN
#define c_STATISTICS        BRIGHT, CYAN
