################################################################
# This file does all the setup overhead stuff necessary for a 
# plugin cmake file -- it is the first thing included in the 
# CMakeLists.txt after it finds the installed version of the code
# We know that EMERGENT_SHARE_DIR is the Emergent install folder

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

# whether User or System, controls default install prefix
# TODO: we could try to get fancy, and deduce based on current src location
if (NOT EMERGENT_PLUGIN_TYPE)
  set(EMERGENT_PLUGIN_TYPE System)
endif (NOT EMERGENT_PLUGIN_TYPE)

if (EMERGENT_PLUGIN_TYPE STREQUAL "System")
  if (WIN32)
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
  else (WIN32)
    # we need to independently set CIP to be that of the root of the install
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
  endif (WIN32)
elseif (EMERGENT_PLUGIN_TYPE STREQUAL "User")
  if (WIN32)
    set(CMAKE_INSTALL_PREFIX $ENV{APPDATA} CACHE INTERNAL "do not change")
  else (WIN32)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE INTERNAL "do not change")
  endif (WIN32)
else (EMERGENT_PLUGIN_TYPE STREQUAL "User")
  message(FATAL_ERROR "EMERGENT_PLUGIN_TYPE must be set to one of: User | System")
endif (EMERGENT_PLUGIN_TYPE STREQUAL "System")
