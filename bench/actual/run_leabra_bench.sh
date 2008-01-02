#!/bin/csh -f

# this is a script file for running various standard configurations of
# leabra_bench.proj

# optional args include:

# send_delta=true/false (default = true) -- toggles use of delta-based netin

# log_epc=true (default false) -- generate epoch log (for debugging)

# log_trl=true (default false) -- generate trial log (for debugging)

# tag=<tag> -- extra tag to apply to log file name (for debugging)

set opts=""
#set opts="send_delta=true"

echo " "
echo "=============================================================="
echo "SMALL Network (5 x 25 units)"
emergent -nogui -ni -p leabra_bench.proj epochs=100 pats=10 units=25 $opts
echo " "
echo "=============================================================="
echo "MEDIUM Network (5 x 100 units)"
emergent -nogui -ni -p leabra_bench.proj epochs=20 pats=10 units=100 $opts
echo " "
echo "=============================================================="
echo "LARGE Network (5 x 500 units)"
emergent -nogui -ni -p leabra_bench.proj epochs=2 pats=10 units=625 $opts
echo " "
echo "=============================================================="
echo "HUGE Network (5 x 1000 units)"
emergent -nogui -ni -p leabra_bench.proj epochs=1 pats=10 units=1024 $opts
