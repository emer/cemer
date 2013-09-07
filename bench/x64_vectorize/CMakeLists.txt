# simple cmake file for standalone benchmarks

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

project(x64vec)

# optional compiler args
#add_definitions(-Weverything)
#add_definitions(-ftree-vetorizer-verbose=2 -ffast-math)
#add_definitions(-ffast-math)

# need this for g++4.8.1:
add_definitions(-std=c++11 -ftree-vectorizer-verbose=4 -Ofast -march=corei7)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

add_executable(x64vec x64vec.cpp)
