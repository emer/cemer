# simple cmake file for standalone benchmarks

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

project(x64cuda)

# definitely add this for best results 
#add_definitions(-march=corei7)
# or this: but it might be subject to the avx problems
add_definitions(-march=native)

# this is good for everything
add_definitions(-Ofast -ffast-math)

# need this for g++
#add_definitions(-std=c++11)
#add_definitions(-ftree-vectorizer-verbose=9)
# looks like avx is hard to support on mac -- assembler doesn't, even when
# fixed with the clang replacement script!
#add_definitions(-mavx)
# to get assemply language: results go in
# build/CMakeFiles/x64vec.dir/x64vec.cpp.o
# add_definitions(-S -fverbose-asm)

FIND_PACKAGE(CUDA)

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-arch sm_20 --use_fast_math -O3)

FIND_CUDA_HELPER_LIBS(curand)

set(CUDA_LIBRARIES ${CUDA_LIBRARIES} ${CUDA_curand_LIBRARY})

CUDA_ADD_EXECUTABLE(x64cuda x64cuda.cu)



