################################################################
# Basic Instructions for User
# 
# To build the plugin:
# mkdir build;
# cd build;
# mkdir build;
# cd build;
# cmake ../ [-G see cmake --help for options] [-DMPI_BUILD=true] \
#  [-DCMAKE_BUILD_TYPE=None | Debug | Release | RelWithDebInfo | MinSizeRel]
#  [-DCMAKE_INSTALL_PREFIX=<path to install>]
#
# ccmake ./ to edit custom options
#
# Important notes:
# 1. do qmake -v -- if it does not say "Using Qt version 4.x.x.." (i.e., it says 3.x.x)
#    then you must find the qmake that is for Qt version 4 and set your path so it is
#    found first!
#

# To configure this file for a new plugin as a plugin programmer, skip to Step 2

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

# to make the compile verbose -- useful for tracking down issues sometimes
set(CMAKE_VERBOSE_MAKEFILE ON)

################################################################
# Step 2: set all the basic parameters about the project
#
# IMPORTANT: plugin programmer: this is stuff you need to set!!
# if copying from the HelloWorld example, then replace audioproc with your project name

# name the project: this will be the name of the plugin
PROJECT(audioproc)

#### version setting: you are responsible for your own versioning and it does not
#### have to correspond with the emergent versions, though that wouldn't hurt

set(PLUGIN_VERSION_MAJOR "4")
set(PLUGIN_VERSION_MINOR "0")
set(PLUGIN_VERSION_PATCH "18")
set(PLUGIN_VERSION "${PLUGIN_VERSION_MAJOR}.${PLUGIN_VERSION_MINOR}.${PLUGIN_VERSION_PATCH}")

# list all the header files here (_def.h and _pl.h are obligatory plugin boilerplate
# and those files also need to be edited to contain the proper name of the project)
# note: do NOT change the name "plugin" here or below
set(plugin_HEADS
  audioproc_def.h
  audioproc_pl.h
  audioproc.h
  ap_files.h
  gammatone.h
)

# list all the .cpp source files that will actually be compiled into the plugin
set(plugin_SRCS
  audioproc.cpp
  audioproc_pl.cpp
  ap_files.cpp
  gammatone.cpp
)

# this says that all the headers listed above are run through maketa (type scanning
# system) -- if this should not be the case (rare), then make a separate list here
set(plugin_HEADS_MAKETA ${plugin_HEADS})

# this says that all the headers listed above are run through moc (type scanning
# system for Qt) -- if this should not be the case (rare), then make a separate list here
# (typically just get a warning for files where it is not necessary)
set(plugin_HEADS_MOC ${plugin_HEADS})

# all the files necessary to compile the plugin -- used for install
set(plugin_FILES ${plugin_HEADS} ${plugin_SRCS} CMakeLists.txt)

################################################################
# Step 2a: find any additional libraries that are needed
# assuming it is on known path, 
# else see {root}/CMakeModules/FindGSL.cmake for example

find_library(SNDFILE_LIBRARY sndfile)


# NOTE to plugin programmer: you should be done now!  Everything below
# is standard boilerplate code that you should not need to edit.

################################################################
# Step 3: now the system attempts to find the emergent share directory
# from which everything else can be derived

find_path(EMERGENT_SHARE_DIR Emergent/README
   /usr/share
   /usr/local/share
   /opt/local/share
   $ENV{SHARE}
)

if(EMERGENT_SHARE_DIR)
  message(STATUS "Found Emergent Share Directory Successfully!!")
  message(STATUS "Share dir is: ${EMERGENT_SHARE_DIR}")
else(EMERGENT_SHARE_DIR)
  message(STATUS "Could not find installed Emergent share directory -- cannot proceed further")
  message(FATAL_ERROR "Edit Step 3 of CMakeLists.txt file to include unusual paths for searching")
endif(EMERGENT_SHARE_DIR)

################################################################
# Step 4: now use cmake modules from the installed code to 
# do all the magic building of stuff for our plugin

# this grabs all the magic cmake stuff
include(${EMERGENT_SHARE_DIR}/Emergent/CMakeModules/EmergentPlugin.cmake)

QT4_WRAP_CPP(plugin_SRCS_MOC ${plugin_HEADS_MOC})

# this sets up system to scan header files for type information using maketa
CREATE_MAKETA_COMMAND(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}" "${plugin_HEADS_MAKETA}")

# set the full set of sources including auto-built ones here -- important that
# _TA.cpp file is first one to make sure maketa is run first
set(plugin_full_SRCS
  ${PROJECT_NAME}_TA.cpp ${plugin_SRCS}
  ${plugin_SRCS_MOC}
)

# adds the library as an official target to compile
add_library(${PROJECT_NAME} SHARED ${plugin_full_SRCS})

# does all the stuff to make the library link against all the right other libraries
# final arg is for any extra libraries this plugin might want
EMERGENT_PLUGIN_LINK_LIBRARIES(${PROJECT_NAME} ${SNDFILE_LIBRARY})
# todo: something diff about a system vs. emergent plugin??

################################################################
# Step 5: install stuff

#install(FILES ${plugin_FILES}
#  DESTINATION ${EMERGENT_SHARE_DIR}/Emergent/plugins/${PROJECT_NAME}
#)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${EMERGENT_PLUGIN_DEST}
)

