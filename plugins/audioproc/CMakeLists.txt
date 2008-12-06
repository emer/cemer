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

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0 FATAL_ERROR)

################################################################
# Step 0: Attempt to find the emergent share directory
# from which everything else can be derived -- you do not typically
# need to edit this code, so skip to step 2 unless you have a problem
# with it finding the installed emergent code

# note: you can add weird paths here in case it is not found on a standard path
# find the path, in terms of an equivalent to CMAKE_INSTALL_PREFIX
if (WIN32)
  find_path(EMERGENT_INSTALL_PREFIX Emergent/README PATHS
    C:/
    "C:/Program Files"
    $ENV{EMERGENTDIR}
    NO_DEFAULT_PATH
  )
else (WIN32)
  find_path(EMERGENT_INSTALL_PREFIX share/Emergent/README PATHS
    /usr
    /usr/local
    /opt/local
    NO_DEFAULT_PATH
  )
endif (WIN32)

if(EMERGENT_INSTALL_PREFIX)
  message(STATUS "Found Emergent Install Prefix Directory Successfully!!")
  message(STATUS "EMERGENT_INSTALL_PREFIX is: ${EMERGENT_INSTALL_PREFIX}")
else(EMERGENT_INSTALL_PREFIX)
  message(STATUS "Could not find installed Emergent share directory -- cannot proceed further")
  message(FATAL_ERROR "Edit Step 3 of CMakeLists.txt file to include unusual paths for searching")
endif(EMERGENT_INSTALL_PREFIX)

if (WIN32)
  set(EMERGENT_SHARE_DIR ${EMERGENT_INSTALL_PREFIX}/Emergent)
else (WIN32)
  set(EMERGENT_SHARE_DIR ${EMERGENT_INSTALL_PREFIX}/share/Emergent)
endif (WIN32)


################################################################
# Step 1: ensure we have the right version of cmake and set build params
# this includes all necessary setup stuff to setup the make process
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPluginSetup.cmake)


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
set(plugin_HEADS_MOC 
  audioproc_pl.h
)

# all the files necessary to compile the plugin -- used for install
set(plugin_FILES ${plugin_HEADS} ${plugin_SRCS} CMakeLists.txt)

################################################################
# Step 2a: find any additional libraries that are needed
# assuming it is on known path, 
# else see {root}/CMakeModules/FindGSL.cmake for example

find_library(SNDFILE_LIBRARY sndfile)
message(STATUS SNDFILE_LIBRARY: ${SNDFILE_LIBRARY})
set(EMERGENT_PLUGIN_EXTRA_LIBRARIES sndfile)

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
