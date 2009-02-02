################################################################
# Basic Instructions for User
# 
# To build the plugin:
# mkdir build;
# cd build;
# cmake ../ [-G see cmake --help for options] [-DMPI_BUILD=true] \
#  [-DCMAKE_BUILD_TYPE=None | Debug | Release | RelWithDebInfo | MinSizeRel]
#  [-DEMERGENT_PLUGIN_TYPE= User | System]
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
# Step 0: ensure we have the right version of cmake -- this
# has to go here at the top of this file otherwise cmake complains
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0 FATAL_ERROR)


################################################################
# Step 1: Find the emergent share (application) directory
# Normal default installation locations should be found, but you
# can edit these paths to add a non-standard location. 
# The platform environment variable overrides are checked first.
if (WIN32)
  file(TO_CMAKE_PATH "$ENV{EMERGENTDIR}" EMERGENTDIR)
  #note: on Windows the normal case is for the ENV var to exist...
  if (EMERGENTDIR)
    set(EMERGENT_SHARE_DIR "${EMERGENTDIR}")
  else (EMERGENTDIR)
    find_path(EMERGENT_SHARE_DIR README PATHS
      "$ENV{EMERGENTDIR}"
      "C:/Program Files/Emergent"
      C:/Emergent
      "C:/Program Files (x86)/Emergent" # W32 on 64, should be rare
      NO_DEFAULT_PATH
    )
  endif (EMERGENTDIR)
else (WIN32)
  if ("$ENV{EMERGENT_PREFIX_DIR}")
    set(EMERGENT_SHARE_DIR "$ENV{EMERGENT_PREFIX_DIR}/share/Emergent")
  else ("$ENV{EMERGENT_PREFIX_DIR}")
    find_path(EMERGENT_SHARE_DIR README PATHS
      /usr/local/share/Emergent
      /usr/share/Emergent
      /opt/local/share/Emergent
      /opt/share/Emergent
      NO_DEFAULT_PATH
    )
  endif ("$ENV{EMERGENT_PREFIX_DIR}")
endif (WIN32)

if(EMERGENT_SHARE_DIR)
  message(STATUS "Found Emergent Application Directory Successfully!!")
  message(STATUS "EMERGENT_SHARE_DIR is: ${EMERGENT_SHARE_DIR}")
else(EMERGENT_SHARE_DIR)
  message(STATUS "Could not find installed Emergent share directory -- cannot proceed further")
  message(FATAL_ERROR "Edit Step 1 of CMakeLists.txt file to include unusual paths for searching")
endif(EMERGENT_SHARE_DIR)


################################################################
# Step 2: set build params

# name the project: this will be the name of the plugin
PROJECT(audioproc)

#### version setting: you are responsible for your own versioning and it does not
#### have to correspond with the emergent versions, though that wouldn't hurt

set(PLUGIN_VERSION_MAJOR "4")
set(PLUGIN_VERSION_MINOR "0")
set(PLUGIN_VERSION_PATCH "18")
set(PLUGIN_VERSION "${PLUGIN_VERSION_MAJOR}.${PLUGIN_VERSION_MINOR}.${PLUGIN_VERSION_PATCH}")

# this includes all necessary setup stuff to setup the make process
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPluginSetup.cmake)


################################################################
# Step 3: set all the basic parameters about the project
#
# IMPORTANT: plugin programmer: this is the section you may need
# to modify or extend, especially if you add additional source
# files or need to include or link to additional libraries not
# part of the Emergent prerequisites

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
set(plugin_HEADS_MAKETA 
  "${EMERGENT_INCLUDE_DIR}/ta_TA_type.h"
  ${plugin_HEADS}
) 

# this says that all the headers listed above are run through moc (type scanning
# system for Qt) -- if this should not be the case (rare), then make a separate list here
# (typically just get a warning for files where it is not necessary)
set(plugin_HEADS_MOC 
  audioproc_pl.h
)

# all the files necessary to compile the plugin -- used for install
set(plugin_FILES ${plugin_HEADS} ${plugin_SRCS} CMakeLists.txt)

################################################################
# Step 3a: find any additional libraries that are needed
# assuming it is on known path, 
# else see {root}/CMakeModules/FindGSL.cmake for example

if (WIN32)
  IF (EXISTS "${EMERGENTDIR}/3rdparty/lib/libsndfile-1.lib")
    SET(SNDFILE_LIBRARY "${EMERGENTDIR}/3rdparty/lib/libsndfile-1.lib")
  ENDIF (EXISTS "${EMERGENTDIR}/3rdparty/lib/libsndfile-1.lib")
  set(EMERGENT_PLUGIN_EXTRA_LIBRARIES ${SNDFILE_LIBRARY})
else (WIN32)
  find_library(SNDFILE_LIBRARY sndfile)
  set(EMERGENT_PLUGIN_EXTRA_LIBRARIES sndfile)
endif (WIN32)
message(STATUS SNDFILE_LIBRARY: ${SNDFILE_LIBRARY})

# NOTE to plugin programmer: you should be done now!  Everything below
# is standard boilerplate code that you should not need to edit.

################################################################
# Step 3: now use cmake modules from the installed code to 
# do all the magic building of stuff for our plugin

# this grabs all the magic cmake stuff for actually making according to
# the above settings
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPlugin.cmake)

# this actually defines a standard set of make targets for building the software
# using the macros defined in the above include -- if you have more advanced
# needs, you can instead look at this file and copy the commands in here instead
# and modify them to suit your needs.
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPluginStdMake.cmake)
