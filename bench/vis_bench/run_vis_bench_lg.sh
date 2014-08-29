#!/bin/csh -f

# this is a script file for running vis_bench_lg.proj

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
# ./run_vis_bench_lg.sh n_threads=2 thread_alloc_pct=0 thread_nibble_chunk=8 thread_compute_thr=0.5 thread_min_units=1000 tag=_mac_mbp_216ghz_thr2alc0nib8thr5

# a basic run is:
# ./run_vis_bench_lg.sh n_threads=2 tag=_mac_mbp2_702_th2

# name of executable to run
#set exe=emergent
set exe=../../build/bin/emergent

echo " "
echo "=============================================================="
echo "vis_bench_lg -- large size vision model, for 1 epoch of 50 patterns"
$exe -nogui -ni -p vis_bench_lg.proj epochs=1 $*
