[![Build Status](https://travis-ci.com/Fullaxx/CHAOSgen.svg?branch=master)](https://travis-ci.com/Fullaxx/CHAOSgen)

# CHAOSgen
CHAOSgen is a random number generator based on hardware events

## Base Docker Image
[Debian](https://hub.docker.com/_/debian) buster-slim (x64)

## Get the image from Docker Hub or build it yourself
```
docker pull fullaxx/chaosgen
docker build -t="fullaxx/chaosgen" github.com/Fullaxx/chaosgen
```

## Requirements for building
First we need to make sure we have all the appropriate libraries. \
Please consult this chart for help with installing the required packages. \
If your OS is not listed, please help us fill out the table, or submit a request via github.

| OS     | Commands (as root)                                         |
| ------ | ---------------------------------------------------------- |
| CentOS | `yum install -y gcc libgcrypt-devel`                       |
| Debian | `apt update; apt install -y build-essential libgcrypt-dev` |
| Fedora | `yum install -y gcc libgcrypt-devel`                       |
| Ubuntu | `apt update; apt install -y build-essential libgcrypt-dev` |

## Compile the Code
Install the libgcrypt development package and the compile the code
```
cd src
./compile.sh
```

## Code Flow
<code>start_your_engines()</code> kicks off 2 threads:
* <code>long_spin()</code> will continuously increment a uint64_t variable (i.e. the rolling stone).
* <code>time_spin()</code> will continuously pull the time from the hardware clock.

Entropy will be pulled from the least significant bits of the rolling stone and the hardware clock. \
The entropy pouch is updated using <code>siphon()</code> after every <code>clock_gettime()</code> call in <code>time_spin()</code>. \
Chaos can be gathered from the entropy pouch using <code>collect_chaos()</code> after <code>siphon()</code>. \
<code>get_chaos()</code> should never hand out the same data twice. \
Once collected, chaos is fed through libgcrypt hashing functions using <code>transmute_2()</code>. \
See [code_walkthrough.txt](https://github.com/Fullaxx/CHAOSgen/blob/master/code_walkthrough.txt) for more detailed information.

## Running the Binaries
There are 4 distinct binaries:
* keygen - used to create key files for symmetric encryption
* dh - used to generate random numbers that dieharder can ingest
* stats - used to show rates of entropy generation and random number production
* chaos2redis - used to populate a redis store with random numbers (see [chaos-dispensary](https://github.com/Fullaxx/chaos-dispensary) for usage)

## Using keygen
Use 2 hashing cores to generate a 1KB random file named 1KB.bin:
```
./keygen.exe -n 2 -f 1KB.bin -b 1000
```
Use 6 hashing cores to generate a 1MB random file named 1MB.bin:
```
./keygen.exe -n 6 -f 1MB.bin -b 1000000
```

## Using stats
stats.exe will show you event counts every second.
These values directly translate to how quickly you can generate entropy.
The first line will tell you how many times <code>clock_gettime()</code> got called.
Entropy is updated via <code>siphon()</code> after every <code>clock_gettime()</code>.
The second line is the amount of chaos you are collecting per second via <code>collect_chaos()</code>.
This value will tell you how quickly entropy is being generated.
The rate of chaos production and the number of hashing cores will determine how fast random numbers are being produced via <code>transmute_2()</code>.
```
./stats.exe
clock_gettime called: 3021312
chaos: 33295 (2663600 n/s) [105.479 GB/hr]
```
You can adjust the number of hashing cores with -n to maximize random number output:
```
./stats.exe -n 6
clock_gettime called: 2555011
chaos: 111480 (8918400 n/s) [353.169 GB/hr]
```

## Testing randomness with dieharder
```
$ docker run -it --rm fullaxx/chaosgen /root/chaos/test_sp.sh
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

## More Info
* [chaos-dispensary](https://github.com/Fullaxx/chaos-dispensary) is a web service that dispenses random numbers
