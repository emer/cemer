# simple cmake file for standalone benchmarks

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

project(x64)

# optional compiler args
#add_definitions(-Weverything)

FIND_PACKAGE(CUDA)

set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-arch sm_20 --use_fast_math -O3)

FIND_CUDA_HELPER_LIBS(curand)

set(CUDA_LIBRARIES ${CUDA_LIBRARIES} ${CUDA_curand_LIBRARY})

CUDA_ADD_EXECUTABLE(x64 x64.cu)



