#!/bin/bash

set -e

CPUS="1"
HWCORES=`nproc`
if [ ${HWCORES} -gt 3 ]; then
  CPUS=$(( ${HWCORES}-3 ))
fi

FILE="dh.sp.1.in"
time ./dh.exe -n ${CPUS} -t 20000000 >${FILE}
dieharder -f ${FILE} -g 202 -a >dh.sp.1.out
