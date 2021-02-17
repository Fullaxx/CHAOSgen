#!/bin/bash

set -e

GCCFLAGS=`libgcrypt-config --cflags`
GCLIBS=`libgcrypt-config --libs`

# -DNO_POUCH_RAND_START
CFLAGS="-Wall ${GCCFLAGS}"
OPTCFLAGS="${CFLAGS} -O2"
DBGCFLAGS="${CFLAGS} -ggdb3"

rm -f *.dbg *.exe

# gcc -Wall -Ofast incr_int.c -o incr_int.exe
gcc -Wall -ggdb3 incr_int.c -o incr_int.dbg

# gcc -Wall -Ofast incr_time.c -o incr_time.exe
gcc -Wall -ggdb3 incr_time.c -o incr_time.dbg

gcc -Wall -ggdb3 main.c -o main.dbg -lpthread

gcc ${OPTCFLAGS} dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o dh.exe

gcc ${DBGCFLAGS} dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o dh.dbg

gcc ${OPTCFLAGS} -DLARGEPOUCH dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o dh_lp.exe

gcc ${DBGCFLAGS} -DLARGEPOUCH dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o dh_lp.dbg

gcc ${OPTCFLAGS} -DSTATISTICS dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o stats.exe

gcc ${DBGCFLAGS} -DSTATISTICS dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o stats.dbg

gcc ${OPTCFLAGS} -DSTATISTICS -DLARGEPOUCH dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o stats_lp.exe

gcc ${DBGCFLAGS} -DSTATISTICS -DLARGEPOUCH dh.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o stats_lp.dbg

gcc ${OPTCFLAGS} keygen.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o keygen.exe

gcc ${DBGCFLAGS} keygen.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -o keygen.dbg

strip *.exe
