#!/bin/csh -f

# this is a script file for running vis_bench_lg.proj

# required args are
# tag=<tag> -- tag for config of cpu and run info for recording run data

# optional args include:

# any args passed to this script are simply passed along to the run

# here are the threading params, along with current defaults
#
# n_threads=2  -- number of threads to use

# a basic run is:
# ./run_vis_bench_lg.sh n_threads=2 tag=_mac_mbp2_702_th2

# name of executable to run
#set exe=emergent_701
#set exe=emergent
set exe=../../build/bin/emergent
#set exe=../../build_cuda/bin/emergent

echo " "
echo "=============================================================="
echo "vis_bench_lg -- large size vision model, for 1 epoch of 50 patterns"
$exe -nogui -ni -p vis_bench_lg.proj epochs=1 $*
