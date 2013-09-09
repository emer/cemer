# simple cmake file for standalone benchmarks

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

project(x64vec)

# optional compiler args

# definitely add this for best results 
add_definitions(-march=corei7)
# or this: but it might be subject to the avx problems
#add_definitions(-march=native)

# this is good for everything
add_definitions(-Ofast -ffast-math)

# need this for g++
#add_definitions(-std=c++11)
#add_definitions(-ftree-vectorizer-verbose=9)
# looks like avx is hard to support on mac -- assembler doesn't, even when
# fixed with the clang replacement script!
add_definitions(-mavx)
# to get assemply language: results go in
# build/CMakeFiles/x64vec.dir/x64vec.cpp.o
# add_definitions(-S -fverbose-asm)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

add_executable(x64vec x64vec.cpp)
