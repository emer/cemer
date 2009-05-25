#!/bin/csh -f

# this is a script file for running various standard configurations of
# leabra_bench.proj

# required args are
# tag=<tag> -- tag for config of cpu and run info for recording run data

# optional args include:

# log_epc=true (default false) -- generate epoch log (for debugging)
# log_trl=true (default false) -- generate trial log (for debugging)

# any args passed to this script are simply passed along to the run

# here are the threading params, along with current defaults
#
# n_threads=2  -- number of threads to use
# thread_alloc_pct=0   -- what percent of total computation should be in the chunks
# thread_nibble_chunk=8  -- how big of a chunk of units should be nibbled by threads at a time
# thread_compute_thr=0.5 -- threshold for actually threading a given task -- all connection-level
#                           code is >= .5    
# thread_min_units=1000   -- minimum number of units in net to engage threading at all

# e.g., a thread testing run would be:
# ./run_leabra_bench.sh n_threads=2 thread_alloc_pct=0 thread_nibble_chunk=8 thread_compute_thr=0.5 thread_min_units=1000 tag=_mac_mbp_216ghz_thr2alc0nib8thr5

# name of executable to run
set exe=emergent
#set exe=../../build/bin/emergent

# echo " "
# echo "=============================================================="
# echo "SMALL Network (5 x 25 units)"
# $exe -nogui -ni -p leabra_bench.proj epochs=10 pats=100 units=25 $*
echo " "
echo "=============================================================="
echo "MEDIUM Network (5 x 100 units)"
$exe -nogui -ni -p leabra_bench.proj epochs=3 pats=100 units=100 $*
echo " "
echo "=============================================================="
echo "LARGE Network (5 x 625 units)"
$exe -nogui -nogui -ni -p leabra_bench.proj epochs=1 pats=10 units=625 $*
echo " "
echo "=============================================================="
echo "HUGE Network (5 x 1024 units)"
$exe -nogui -ni -p leabra_bench.proj epochs=1 pats=5 units=1024 $*
