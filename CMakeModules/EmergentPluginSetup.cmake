################################################################
# This file does all the setup overhead stuff necessary for a 
# plugin cmake file -- it is the first thing included in the 
# CMakeLists.txt after it finds the installed version of the code


################################################################
# Step 1: ensure we have the right version of cmake and set build params
# typically no need to change this stuff, but it has to come first

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0 FATAL_ERROR)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# default build type is RelWithDebInfo
IF(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)

# to enable MPI
set(MPI_BUILD FALSE CACHE BOOL "Set to true to enable MPI distributed memory system")
