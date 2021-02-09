#!/bin/bash

set -e

CPUS="1"
HWCORES=`nproc`
if [ ${HWCORES} -gt 3 ]; then
  CPUS=$(( ${HWCORES}-3 ))
fi

FILE="dh.lp.1.in"
time ./dh_lp.exe -n ${CPUS} -t 20000000 > ${FILE}
dieharder -f ${FILE} -g 202 -a > dh.lp.1.out
