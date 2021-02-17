#!/bin/bash

set -e

GCCFLAGS=`libgcrypt-config --cflags`
GCLIBS=`libgcrypt-config --libs`

CFLAGS="-Wall ${GCCFLAGS}"
OPTCFLAGS="${CFLAGS} -O2"
DBGCFLAGS="${CFLAGS} -ggdb3 -DDEBUG"
#DBGCFLAGS+=" -DCHRONOMETRY -DTIME_NUM_CONVERSION"
#DBGCFLAGS+=" -DCHRONOMETRY -DTIME_REDIS_UPDATE"

rm -f *.dbg *.exe

gcc ${OPTCFLAGS} chaos2redis.c redisops.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -lhiredis -o chaos2redis.exe

gcc ${DBGCFLAGS} chaos2redis.c redisops.c chronometry.c \
pouch.c pin.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -lhiredis -o chaos2redis.dbg

strip *.exe
