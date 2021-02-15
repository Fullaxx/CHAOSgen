#!/bin/bash

set -e

CPUS="1"
HWCORES=`nproc`
if [ ${HWCORES} -gt 3 ]; then
  CPUS=$(( ${HWCORES}-3 ))
fi

FILE="dh.lp.1.in"
START=`awk -F. '{print $1}' /proc/uptime`
dh_lp.exe -n ${CPUS} -t 20000000 >${FILE}
STOP=`awk -F. '{print $1}' /proc/uptime`
COUNT=`cat ${FILE} | wc -l`
DIFF=$(( STOP - START ))
RNUMPRESEC=`calc "${COUNT}/${DIFF}" | cut -d. -f1 | tr '~' ' ' | awk '{print $1}'`
echo "${COUNT} random numbers generated in ${DIFF}s (approx ${RNUMPRESEC} per sec)"
dieharder -f ${FILE} -g 202 -a | tee dh.lp.1.out
