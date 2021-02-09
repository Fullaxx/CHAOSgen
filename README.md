[![Build Status](https://travis-ci.com/Fullaxx/CHAOSgen.svg?branch=master)](https://travis-ci.com/Fullaxx/CHAOSgen)

# CHAOSgen
CHAOSgen is a random number generator based on hardware events

## Base Docker Image
[Ubuntu](https://hub.docker.com/_/ubuntu) 20.04 (x64)

## Get the image from Docker Hub or build it yourself
```
docker pull fullaxx/chaosgen
docker build -t="fullaxx/chaosgen" github.com/Fullaxx/chaosgen
```

## Testing randomness with dieharder
```
$ ./dh.exe -t 20000000 >dh.1.in
$ dieharder -f dh.1.in -g 202 -a >dh.1.out
#=============================================================================#
#            dieharder version 3.31.1 Copyright 2003 Robert G. Brown          #
#=============================================================================#
   rng_name    |           filename             |rands/second|
     file_input|                         dh.1.in|  3.98e+06  |
#=============================================================================#
        test_name   |ntup| tsamples |psamples|  p-value |Assessment
#=============================================================================#
   diehard_birthdays|   0|       100|     100|0.83495641|  PASSED
      diehard_operm5|   0|   1000000|     100|0.84701912|  PASSED
  diehard_rank_32x32|   0|     40000|     100|0.29121974|  PASSED
    diehard_rank_6x8|   0|    100000|     100|0.09834982|  PASSED
   diehard_bitstream|   0|   2097152|     100|0.17172475|  PASSED
        diehard_opso|   0|   2097152|     100|0.69133105|  PASSED
        diehard_oqso|   0|   2097152|     100|0.05188204|  PASSED
         diehard_dna|   0|   2097152|     100|0.03561770|  PASSED
diehard_count_1s_str|   0|    256000|     100|0.48676195|  PASSED
diehard_count_1s_byt|   0|    256000|     100|0.19582831|  PASSED
 diehard_parking_lot|   0|     12000|     100|0.76062712|  PASSED
    diehard_2dsphere|   2|      8000|     100|0.93807777|  PASSED
    diehard_3dsphere|   3|      4000|     100|0.96252837|  PASSED
     diehard_squeeze|   0|    100000|     100|0.56220924|  PASSED
        diehard_sums|   0|       100|     100|0.06032821|  PASSED
        diehard_runs|   0|    100000|     100|0.93674540|  PASSED
        diehard_runs|   0|    100000|     100|0.08227590|  PASSED
       diehard_craps|   0|    200000|     100|0.23747495|  PASSED
       diehard_craps|   0|    200000|     100|0.41935142|  PASSED
 marsaglia_tsang_gcd|   0|  10000000|     100|0.32582469|  PASSED
 marsaglia_tsang_gcd|   0|  10000000|     100|0.89273226|  PASSED
```
