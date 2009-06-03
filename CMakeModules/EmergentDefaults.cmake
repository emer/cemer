# This file is included near the top of both Emergent
# and plugins so that we have a consistent set of 
# default values and processing
#
# Here are the variables that must be set so far:
# (none yet)

# some standard env variables, loaded locally 
# and fixed up for Windows
# note: EMERGENTDIR is really only used on Windows or for development
file(TO_CMAKE_PATH "$ENV{EMERGENTDIR}" EMERGENTDIR)
file(TO_CMAKE_PATH "$ENV{COINDIR}" COINDIR)
# assume in 3rdparty
#TODO: set COINDIR according to defaults
#allowed overrides:
file(TO_CMAKE_PATH "$ENV{EMERGENT_PLUGIN_DIR}" EMERGENT_PLUGIN_DIR)

# default build type is RelWithDebInfo
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

# set cache parameter for mpi option
set(MPI_BUILD FALSE CACHE BOOL "Set to true to enable MPI distributed memory system")

# basic compiler warnings, etc.
# note: really should be testing MSVC but contrary to docs, it is not true using MSVC
if (WIN32)
  #shut off warnings: 4250 (inheritance dominance)
  # 4258;4661;4996 (unsafe crt routines)
  add_definitions(/wd4250 /wd4258 /wd4661 /wd4996)
  # enable multi-threaded compiling (always safe, will ignore and print warning when incompatible)
  add_definitions(/MP)
#also needed for 32  if (CMAKE_CL_64)
    # need to set this so larger .cpp files don't error (what a stupid non-default!!!)
    add_definitions(/bigobj)
#  endif (CMAKE_CL_64)
else (WIN32) # assume gcc!!!
  # a function with a non-void return-type that doesn't return a value s/b an error!!!
  add_definitions(-Woverloaded-virtual -ftree-vectorizer-verbose=2)
  if (APPLE) #grr... not working on Mac for some reason
    message(STATUS "note: '-Werror=return-type' not working on Mac: suggest fixing") 
    add_definitions(-Wreturn-type) # at least get a warning!
  else (APPLE)
    add_definitions(-Werror=return-type)
  endif (APPLE)
endif (WIN32)

