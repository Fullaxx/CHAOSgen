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
pouch.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -lhiredis -o chaos2redis.exe

gcc ${DBGCFLAGS} chaos2redis.c redisops.c chronometry.c \
pouch.c pot?.c getopts.c my_gcry_help.c \
-lpthread ${GCLIBS} -lhiredis -o chaos2redis.dbg

gcc ${OPTCFLAGS} chaos_srv_*.c cJSON.c \
getopts.c futils.c searest*.c \
-lpthread -lmicrohttpd -lhiredis -o chaos_srv.exe

gcc ${DBGCFLAGS} chaos_srv_*.c cJSON.c chronometry.c \
getopts.c futils.c searest*.c \
-lpthread -lmicrohttpd -lhiredis -o chaos_srv.dbg

strip *.exe
