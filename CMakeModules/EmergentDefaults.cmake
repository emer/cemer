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
  # shut off warnings: 4250 (inheritance dominance)
  # 4258;4661;4996 (unsafe crt routines)
  add_definitions(/wd4250 /wd4258 /wd4661 /wd4996)

  # enable multi-threaded compiling (always safe, will ignore and print warning when incompatible)
  # don't use add_definitions here, because RC.exe doesn't allow /MP flag.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  # need to set this so larger .cpp files don't error (what a stupid non-default!!!)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

else (WIN32) # assume gcc!!!
  # A function with a non-void return-type that doesn't return a value s/b an error!!!
  # GCC added support for treating this as an error somewhere around version 4.2
  include(CheckCXXCompilerFlag)
  set(GCC_RETURN_TYPE_ERROR "-Werror=return-type")
  set(GCC_RETURN_TYPE_WARN "-Wreturn-type")
  check_cxx_compiler_flag(GCC_RETURN_TYPE_ERROR HAS_RETURN_TYPE_ERROR_FLAG)
  if (HAS_RETURN_TYPE_ERROR_FLAG)
    add_definitions(${GCC_RETURN_TYPE_ERROR})
  else (HAS_RETURN_TYPE_ERROR_FLAG)
    message("Compiler does not support ${GCC_RETURN_TYPE_ERROR}, will use ${GCC_RETURN_TYPE_WARN} instead.")
    add_definitions(${GCC_RETURN_TYPE_WARN}) # at least get a warning!
  endif (HAS_RETURN_TYPE_ERROR_FLAG)
  
  # NOTE:  -ftree-vectorizer-verbose=1 can be interesting but not worth it for a default param -- users may add at own discretion 
  add_definitions(-Woverloaded-virtual)
  if (APPLE)
    # on Mac, DEBUG is not defined!
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
    message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
  endif (APPLE)
endif (WIN32)

