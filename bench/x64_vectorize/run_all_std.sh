#!/bin/bash

# run standard benchmark configs 

n_runs=3

echo "==== large size ===="

echo "===== CPU_ONLY ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 8192 n_per_un 2048 n_epochs 2 vec cpu
done

echo "===== SSE4 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 8192 n_per_un 2048 n_epochs 2 vec sse4
done

echo "===== SSE8 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 8192 n_per_un 2048 n_epochs 2 vec sse8
done

echo "==== medium size ===="

echo "===== CPU_ONLY ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 4096 n_per_un 1024 n_epochs 10 vec cpu
done

echo "===== SSE4 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 4096 n_per_un 1024 n_epochs 10 vec sse4
done

echo "===== SSE8 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 4096 n_per_un 1024 n_epochs 10 vec sse8
done

echo "==== small size ===="

echo "===== CPU_ONLY ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 2048 n_per_un 512 n_epochs 20 vec cpu
done

echo "===== SSE4 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 2048 n_per_un 512 n_epochs 20 vec sse4
done

echo "===== SSE8 ====="
for (( i=1; i<=$n_runs; i++ ))
  do
    ./build/x64vec n_units 2048 n_per_un 512 n_epochs 20 vec sse8
done

