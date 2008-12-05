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
# Step 1: Attempt to find the emergent share directory
# from which everything else can be derived -- you do not typically
# need to edit this code, so skip to step 2 unless you have a problem
# with it finding the installed emergent code

# note: you can add weird paths here in case it is not found on a standard path
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

# this includes all necessary setup stuff to setup the make process
include(${EMERGENT_SHARE_DIR}/Emergent/CMakeModules/EmergentPluginSetup.cmake)

################################################################
# Step 2: set all the basic parameters about the project
#
# IMPORTANT: plugin programmer: this is stuff you need to set!!
# if copying from the HelloWorld example, then replace hw with your project name

# name the project: this will be the name of the plugin
PROJECT(hw)

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
  hw_def.h
  hw_pl.h
  hw.h
)

# list all the .cpp source files that will actually be compiled into the plugin
set(plugin_SRCS
  hw.cpp
  hw_pl.cpp
)

# this says that all the headers listed above are run through maketa (type scanning
# system) -- if this should not be the case (rare), then make a separate list here
set(plugin_HEADS_MAKETA ${plugin_HEADS})

# this says that all the headers listed above are run through moc (type scanning
# system for Qt) -- if this should not be the case (rare), then make a separate list here
# (typically just get a warning for files where it is not necessary)
set(plugin_HEADS_MOC ${plugin_HEADS})

# all the files necessary to compile the plugin -- used for install
set(plugin_FILES
  ${plugin_HEADS}
  ${plugin_SRCS}
  CMakeLists.txt
  build_plugin.sh
  build_plugin_dbg.sh
  )

# to make the compile verbose -- useful for tracking down issues sometimes
set(CMAKE_VERBOSE_MAKEFILE ON)

# NOTE to plugin programmer: you should be done now!  Everything below
# is standard boilerplate code that you should not need to edit.

################################################################
# Step 3: now use cmake modules from the installed code to 
# do all the magic building of stuff for our plugin

# this grabs all the magic cmake stuff for actually making according to
# the above settings
include(${EMERGENT_SHARE_DIR}/Emergent/CMakeModules/EmergentPlugin.cmake)

# this actually defines a standard set of make targets for building the software
# using the macros defined in the above include -- if you have more advanced
# needs, you can instead look at this file and copy the commands in here instead
# and modify them to suit your needs.
include(${EMERGENT_SHARE_DIR}/Emergent/CMakeModules/EmergentPluginStdMake.cmake)
