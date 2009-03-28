################################################################
# Basic Instructions for User
# 
# To build the plugin:
# mkdir build;
# cd build;
# cmake ../ [-G see cmake --help for options] [-DMPI_BUILD=true] \
#  [-DCMAKE_BUILD_TYPE= Debug | Release | RelWithDebInfo | MinSizeRel]
#  [-DEMERGENT_PLUGIN_TYPE= User | System | Default] # where to install plugin
#  [-DCMAKE_VERBOSE_MAKEFILE=ON | OFF] # useful for tracking down issues sometimes
#
# ccmake ./ to edit custom options
#
# Important notes:
# 1. do qmake -v -- if it does not say "Using Qt version 4.x.x.." (i.e., it says 3.x.x)
#    then you must find the qmake that is for Qt version 4 and set your path so it is
#    found first!
#

# To configure this file for a new plugin as a plugin programmer, skip to Step 3

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
      "C:/Program Files/Emergent"
      C:/Emergent
      "C:/Program Files (x86)/Emergent" # W32 on 64, should be rare
      NO_DEFAULT_PATH
    )
  endif (EMERGENTDIR)
else (WIN32)
  file(TO_CMAKE_PATH "$ENV{EMERGENT_PREFIX_DIR}" EMERGENT_PREFIX_DIR)
  if (EMERGENT_PREFIX_DIR)
    set(EMERGENT_SHARE_DIR "${EMERGENT_PREFIX_DIR}/share/Emergent")
  else (EMERGENT_PREFIX_DIR)
    find_path(EMERGENT_SHARE_DIR README PATHS
      /usr/local/share/Emergent
      /usr/share/Emergent
      /opt/local/share/Emergent
      /opt/share/Emergent
      NO_DEFAULT_PATH
    )
  endif (EMERGENT_PREFIX_DIR)
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

# name the project: this will be the name of the plugin and for
# consistency should be the same as the folder in which it lives
PROJECT(PyEmergent)

# version setting: you are responsible for your own versioning and it does not
# have to correspond with the emergent versions, though that wouldn't hurt
# TODO: if you change these values, change them in PyEmergent_pl.h as well
set(PLUGIN_VERSION_MAJOR "0")
set(PLUGIN_VERSION_MINOR "0")
set(PLUGIN_VERSION_PATCH "0")
set(PLUGIN_VERSION "${PLUGIN_VERSION_MAJOR}.${PLUGIN_VERSION_MINOR}.${PLUGIN_VERSION_PATCH}")

# this includes all necessary setup stuff to setup the make process
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPluginSetup.cmake)


################################################################
# Step 3: specify files and dependent libraries for your project
#
# IMPORTANT: plugin programmer: this is the section you may need
# to modify or extend, especially if you add additional source
# files or need to include or link to additional libraries not
# part of the Emergent prerequisites

# list all the header files here (_def.h and _pl.h are obligatory plugin boilerplate)
# do not list any included files, only new files that are in this folder
# note: do NOT change the name "plugin" here or below
# note: do *not* add the generated PyEmergent_TA_*.h files
set(plugin_HEADS
  PyEmergent_def.h
  PyEmergent_pl.h
  PyEmergent.h
# if you create Qt or Inventor-based classes, put them in PyEmergent_qtso.*
# and uncomment the following line:
# PyEmergent_qtso.h
)

# list all the .cpp source files that will be compiled into the plugin
# note: do *not* add the generated PyEmergent_TA.cpp file
set(plugin_SRCS
  PyEmergent_pl.cpp
  PyEmergent.cpp
# if you create Qt or Inventor-based classes, put them in PyEmergent_qtso.*
# and uncomment the following line:
# PyEmergent_qtso.cpp
)

# list the project headers that are run through maketa (the Emergent
# type scanning system) 
set(plugin_HEADS_MAKETA
  "${EMERGENT_INCLUDE_DIR}/ta_TA_type.h"
# IMPORTANT: if your plugin uses network code, add the appropriate
# headers after the ta_TA, ex, for a Leabra-based plugin, add these:
# "${EMERGENT_INCLUDE_DIR}/network_TA_type.h" 
# "${EMERGENT_INCLUDE_DIR}/leabra_TA_type.h" 
  ${plugin_HEADS}
)

# any headers that declare any Qt/QObject-derived classes go here --
# Emergent's taBase-derived classes (the bulk of Emergent) don't need this --
# if you are unsure, you can usually put any header here and Qt/moc will
# just ignore it with a warning that "no QObject classes were found"
set(plugin_HEADS_MOC
  PyEmergent_pl.h
# if you create Qt or Inventor classes, put them in PyEmergent_qtso.*
# and uncomment the following line:
# PyEmergent_qtso.h
)

# all the files necessary to compile the plugin -- used for install
set(plugin_FILES
  ${plugin_HEADS}
  ${plugin_SRCS}
  CMakeLists.txt
# TODO: if you have any additional files needed by the plugin, such as
# media files, documentation, etc., add them here so they get installed
)

################################################################
# Step 3a: find any additional libraries that are needed, if any
# assuming it is on known path, 
# else see {root}/CMakeModules/FindGSL.cmake for example

# the following lines, commented out, are examples from audioproc plugin
find_library(PYTHON_LIBRARY python2.5)
message(STATUS PYTHON_LIBRARY: ${PYTHON_LIBRARY})
set(EMERGENT_PLUGIN_EXTRA_LIBRARIES ${PYTHON_LIBRARY})
set(CMAKE_SHARED_LINKER_FLAGS "-Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions")

# NOTE to plugin programmer: you should be done now!  Everything below
# is standard boilerplate code that you should not need to edit.


################################################################
# Step 4: now use cmake modules from the installed code to 
# do all the magic building of stuff for our plugin

# this grabs all the magic cmake stuff for actually making according to
# the above settings
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPlugin.cmake)

# this actually defines a standard set of make targets for building the software
# using the macros defined in the above include -- if you have more advanced
# needs, you can instead look at this file and copy the commands in here instead
# and modify them to suit your needs.
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPluginStdMake.cmake)
