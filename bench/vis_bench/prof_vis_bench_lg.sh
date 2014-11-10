#!/bin/csh -f

# this is a script file for running vis_bench_lg.proj under hpcrun profiler

# required args:
# 1 = tag=<tag> -- tag for config of cpu and run info for recording run data

set tag=$1
echo "TAG=$tag"

# any other args passed to this script are simply passed along to the run

# here are the threading params, along with current defaults
#
# n_threads=2  -- number of threads to use

# a basic run is:
# ./prof_vis_bench_lg.sh _haswell_10cx2_avx2_th2 n_threads=2

# name of executable to run
#set exe=emergent_701
#set exe=emergent
#set exe=../../build/bin/emergent
# hpc version has hpcprof enabled -- start with -ds and it will focus on the target code only
# unfortunately this prevents REALTIME from working :(
set exe=../../build_hpc/bin/emergent_hpc
#set exe=../../build_cuda/bin/emergent

echo " "
echo "=============================================================="
echo "vis_bench_lg -- large size vision model, for 1 epoch of 50 patterns"

# linking against hpctoolkit prevents REALTIME from working!!! :( 
echo "======================"
echo "First pass: CPUTIME -- REALTIME doesn't work!"
echo "hpcrun -ds -o hpcrunout$tag -e CPUTIME@1000 $exe -nogui -ni -p vis_bench_lg.proj tag=$tag $argv[2-]"
hpcrun -ds -o hpcrunout$tag -e CPUTIME@1000 $exe -nogui -ni -p vis_bench_lg.proj tag=$tag $argv[2-]

echo "======================"
echo "Second pass: misses"
echo "hpcrun -ds -o hpcrunout$tag -e PAPI_TOT_CYC@15000000 -e PAPI_RES_STL@400000 -e PAPI_MEM_WCY@400000 -e PAPI_L1_TCM@400000 -e PAPI_L2_TCM@400000 $exe -nogui -ni -p vis_bench_lg.proj tag=$tag $argv[2-]"
hpcrun -ds -o hpcrunout$tag -e PAPI_TOT_CYC@15000000 -e PAPI_RES_STL@400000 -e PAPI_MEM_WCY@400000 -e PAPI_L1_TCM@400000 -e PAPI_L2_TCM@400000 $exe -nogui -ni -p vis_bench_lg.proj tag=$tag $argv[2-]

echo "======================"
echo "Third pass: hits"
echo "hpcrun -ds -o hpcrunout$tag -e PAPI_TOT_INS@15000000 -e PAPI_L1_TCH@400000 -e PAPI_L2_TCH@400000 -e PAPI_L3_TCH@400000 $exe -nogui -ni -p vis_bench_lg.proj tag=$tag $argv[2-]"
hpcrun -ds -o hpcrunout$tag -e PAPI_TOT_INS@15000000 -e PAPI_L1_TCH@400000 -e PAPI_L2_TCH@400000 -e PAPI_L3_TCH@400000 $exe -nogui -ni -p vis_bench_lg.proj tag=$tag $argv[2-]

echo "======================"
echo "hpcstruct, hpcprof"

echo "hpcstruct $exe"
hpcstruct $exe
# always have this around -- what is actually used -- keep it up-to-date
hpcstruct ../../build/bin/emergent

echo "hpcprof -o hpcprofout${tag} -S emergent.hpcstruct -I ../../src/'*' hpcrunout$tag"
hpcprof -o hpcprofout${tag} -S emergent.hpcstruct -I ../../src/'*' hpcrunout$tag
