# This file is included near the top of both Emergent
# and plugins so that we have a consistent set of 
# default values and processing
#
# Here are the variables that must be set so far:
# (none yet)

# some standard env variables, loaded locally 
# and fixed up for Windows
file(TO_CMAKE_PATH "$ENV{COINDIR}" COINDIR)
# note: EMERGENTDIR is really only used on Windows or for development
file(TO_CMAKE_PATH "$ENV{EMERGENTDIR}" EMERGENTDIR)

# default build type is RelWithDebInfo
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

# set cache parameter for mpi option
set(MPI_BUILD FALSE CACHE BOOL "Set to true to enable MPI distributed memory system")

# basic compiler warnings, etc.
if (WIN32)
  #shut off warnings: 4250 (inheritance dominance)
  # 4258;4661;4996 (unsafe crt routines)
  add_definitions(/wd4250 /wd4258 /wd4661 /wd4996)
  # enable multi-threaded compiling (always safe, will ignore and print warning when incompatible)
  add_definitions(/MP)
  if (CMAKE_CL_64)
    # need to set this so larger .cpp files don't error (what a stupid non-default!!!)
    add_definitions(/bigobj)
  endif (CMAKE_CL_64)
endif (WIN32)

