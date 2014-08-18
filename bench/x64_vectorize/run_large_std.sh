#!/bin/bash

# run standard benchmark configs 

n_runs=3

echo "==== large size ===="

echo "===== SSE8 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_layers 4 n_units 8192 n_per_un 2048 n_epochs 2 vec sse8
done

echo "===== SSE4 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_layers 4 n_units 8192 n_per_un 2048 n_epochs 2 vec sse4
done

echo "===== CPU_ONLY ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_layers 4 n_units 8192 n_per_un 2048 n_epochs 2 vec cpu
done
