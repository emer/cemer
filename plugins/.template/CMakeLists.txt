################################################################
# Basic Instructions for User
# 
# To build the plugin:
# mkdir build;
# cd build;
# cmake ../ [-G see cmake --help for options] [-DMPI_BUILD=true] \
#  [-DCMAKE_BUILD_TYPE=Debug | Release | RelWithDebInfo | MinSizeRel]
#  [-DEMERGENT_PLUGIN_TYPE= User | System] # where to install plugin
#  [-DCMAKE_VERBOSE_MAKEFILE=ON | OFF] # useful for tracking down issues sometimes
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
# Step 1: Attempt to find the emergent share directory
# from which everything else can be derived -- you do not typically
# need to edit this code, so skip to step 2 unless you have a problem
# with it finding the installed emergent code

# note: you can add weird paths here in case it is not found on a standard path
# find the path, in terms of an equivalent to CMAKE_INSTALL_PREFIX
if (WIN32)
  find_path(EMERGENT_INSTALL_PREFIX Emergent/README PATHS
    C:/
    "C:/Program Files"
    $ENV{EMERGENTDIR}/..
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


################################################################
# Step 1: set build params
# this includes all necessary setup stuff to setup the make process
include(${EMERGENT_SHARE_DIR}/CMakeModules/EmergentPluginSetup.cmake)

################################################################
# Step 2: set all the basic parameters about the project
#
# IMPORTANT: plugin programmer: this is stuff you may need to modify
# or extend, especially if you add additional source files or
# need to include or link to additional libraries not part of the
# Emergent prerequisites

# name the project: this will be the name of the plugin
PROJECT(template)

#### version setting: you are responsible for your own versioning and it does not
#### have to correspond with the emergent versions, though that wouldn't hurt

set(PLUGIN_VERSION_MAJOR "1")
set(PLUGIN_VERSION_MINOR "0")
set(PLUGIN_VERSION_PATCH "0")
set(PLUGIN_VERSION "${PLUGIN_VERSION_MAJOR}.${PLUGIN_VERSION_MINOR}.${PLUGIN_VERSION_PATCH}")

# list all the header files here (_def.h and _pl.h are obligatory plugin boilerplate)
# do not list any included files, only new files that are in this folder
# note: do NOT change the name "plugin" here or below
# note: do *not* add the generated template_TA_*.h files
set(plugin_HEADS
  template_def.h
  template_pl.h
  template.h
)

# list all the .cpp source files that will be compiled into the plugin
# note: do *not* add the generated template_TA.cpp file
set(plugin_SRCS
  template_pl.cpp
  template.cpp
)

# list the project headers that are run through maketa (the Emergent
# type scanning system) 
set(plugin_HEADS_MAKETA
  ta_TA_type.h
# IMPORTANT: if your plugin uses network code, add the appropriate
# headers after the ta_TA, ex, for a Leabra-based plugin, add these:
# network_TA_type.h 
# leabra_TA_type.h 
  ${plugin_HEADS}
)

# any headers that declare any Qt/QObject-derived classes go here --
# Emergent's taBase-derived classes (the bulk of Emergent) don't need this --
# if you are unsure, you can usually put any header here and Qt/moc will
# just ignore it with a warning that "no QObject classes were found"
set(plugin_HEADS_MOC
  template_pl.h
)

# all the files necessary to compile the plugin -- used for install
set(plugin_FILES
  ${plugin_HEADS}
  ${plugin_SRCS}
  CMakeLists.txt
  build_plugin.sh
  build_plugin_dbg.sh
  )

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
