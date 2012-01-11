#!/bin/sh
# Run this to generate all the initial makefiles, etc.
#

test -f configure.in || {
    echo "This directory does not look like the top-level directory. stop."
    exit 1
}

rm -rf config.cache autom4te*.cache aclocal.m4

echo "running "`autoheader --version | head -n 1` "..."
autoheader || {
    echo "autoheader failed. stop."
    exit 1
}

echo "running "`aclocal --version | head -n 1 ` "..."
aclocal --force $ACLOCAL_FLAGS 2> /dev/null || {
    echo "aclocal failed. stop."
    exit 1
}

echo "running "`automake --version | head -n 1` "..."
automake --foreign --include-deps --add-missing --copy || {
    echo "automake failed. stop."
    exit 1
}

echo "running "`autoconf --version | head -n 1` "..."
autoconf || {
    echo "autoconf failed. stop."
    exit 1
}
echo "running ./configure $* ..."
./configure > /dev/null $* || {
    echo "./configure failed. stop."
    exit 1
}

echo "Generating OPTIONS files"
man ./tcpick.8 | col -b > OPTIONS
man ./tcpick_italian.8 | col -b > OPTIONS.it

echo `cat configure.in | grep __VERSION | grep define| cut -f3 -d"[" | cut -f1 -d"]"` > VERSION
echo "tcpick version "`cat VERSION`" ready to be compiled"

