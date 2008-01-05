#!/bin/csh -f

# this is a script file for running various standard configurations of
# leabra_bench.proj

# optional args include:

# send_delta=true/false (default = true) -- toggles use of delta-based netin

# threads=

# log_epc=true (default false) -- generate epoch log (for debugging)

# log_trl=true (default false) -- generate trial log (for debugging)

# tag=<tag> -- extra tag to apply to log file name (for debugging)

#set opts=""
#set opts="send_delta=false"
set opts="threads=true max_cpus=2"

echo " "
echo "=============================================================="
echo "SMALL Network (5 x 25 units)"
../../src/emergent/bin/.libs/emergent -nogui -ni -p leabra_bench.proj epochs=100 pats=10 units=25 $opts
echo " "
echo "=============================================================="
echo "MEDIUM Network (5 x 100 units)"
../../src/emergent/bin/.libs/emergent -nogui -ni -p leabra_bench.proj epochs=20 pats=10 units=100 $opts
echo " "
echo "=============================================================="
echo "LARGE Network (5 x 625 units)"
../../src/emergent/bin/.libs/emergent -nogui -ni -p leabra_bench.proj epochs=1 pats=10 units=625 $opts
echo " "
echo "=============================================================="
echo "HUGE Network (5 x 1024 units)"
../../src/emergent/bin/.libs/emergent -nogui -ni -p leabra_bench.proj epochs=1 pats=5 units=1024 $opts
