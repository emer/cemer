#!/bin/csh -f

# this is a script file for running various standard configurations of
# bp_bench.proj

# required args are
# tag=<tag> -- tag for config of cpu and run info for recording run data

# optional args include:

# log_epc=true (default false) -- generate epoch log (for debugging)
# log_trl=true (default false) -- generate trial log (for debugging)

# any args passed to this script are simply passed along to the run

# e.g., a thread testing run would be:
#./run_bp_bench.sh n_threads=2 chunk_pct=0.5 nibble_chunk=4 compute_thr=0.5 tag=_mac_mbp_216ghz_thr2pct50nch4thr5

# name of executable to run
#set exe=emergent
set exe=../../build/bin/emergent

# echo " "
# echo "=============================================================="
# echo "SMALL Network (5 x 25 units)"
# $exe -nogui -ni -p bp_bench.proj epochs=30 pats=1000 units=25 $*
echo " "
echo "=============================================================="
echo "MEDIUM Network (5 x 100 units)"
$exe -nogui -ni -p bp_bench.proj epochs=10 pats=1000 units=100 $*
echo " "
echo "=============================================================="
echo "LARGE Network (5 x 625 units)"
$exe -nogui -nogui -ni -p bp_bench.proj epochs=3 pats=100 units=625 $*
echo " "
echo "=============================================================="
echo "HUGE Network (5 x 1024 units)"
$exe -nogui -ni -p bp_bench.proj epochs=1 pats=50 units=1024 $*
