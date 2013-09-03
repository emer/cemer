# simple cmake file for standalone benchmarks

cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

project(x64vec)

# optional compiler args
#add_definitions(-Weverything)

add_executable(x64vec x64vec.cpp)
