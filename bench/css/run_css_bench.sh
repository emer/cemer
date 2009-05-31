#!/bin/csh -f

# this is a script file for running various standard configurations of
# css_bench.proj

# name of executable to run
#set exe=emergent
set exe=../../build/bin/emergent

echo " "
echo "=============================================================="
echo "Running css_bench.proj"

$exe -nogui -ni -p css_bench.proj
