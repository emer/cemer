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

# whether User or System, controls default install prefix
# TODO: we could try to get fancy, and deduce based on current src location
if (NOT EMERGENT_PLUGIN_TYPE)
  set(EMERGENT_PLUGIN_TYPE System)
endif (NOT EMERGENT_PLUGIN_TYPE)

# although we know the app folder, we need to extract install prefix
if (WIN32)
  #note: the normal case is for the ENV var to exist...
  if (${EMERGENTDIR})
    set(EMERGENT_INSTALL_PREFIX "${EMERGENTDIR}/..")
  else (${EMERGENTDIR})
    find_path(EMERGENT_INSTALL_PREFIX Emergent/README PATHS
      C:/
      "C:/Program Files"
      "C:/Program Files (x86)" # W32 on 64, should be rare
      NO_DEFAULT_PATH
    )
  endif (${EMERGENTDIR})
else (WIN32)
  # find the path, in terms of an equivalent to CMAKE_INSTALL_PREFIX
  find_path(EMERGENT_INSTALL_PREFIX share/Emergent/README PATHS
    /usr
    /usr/local
    /opt/local
    NO_DEFAULT_PATH
  )
endif (WIN32)

# set the install prefix and actual install location
# see:
# http://grey.colorado.edu/emergent/index.php/CMake#Key_CMake_variables
# http://grey.colorado.edu/emergent/index.php/Plugin_design
if (EMERGENT_PLUGIN_TYPE STREQUAL "System")
  if (WIN32)
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST Emergent/plugins)
  else (WIN32)
    # we need to independently set CIP to be that of the root of the install
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST lib/Emergent/plugins)
  endif (WIN32)
elseif (EMERGENT_PLUGIN_TYPE STREQUAL "User")
  if (WIN32)
    set(CMAKE_INSTALL_PREFIX $ENV{APPDATA} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST ccnlab/emergent/plugins)
  else (WIN32)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST lib/Emergent/plugins)
  endif (WIN32)
else (EMERGENT_PLUGIN_TYPE STREQUAL "User")
  message(FATAL_ERROR "EMERGENT_PLUGIN_TYPE must be set to one of: User | System")
endif (EMERGENT_PLUGIN_TYPE STREQUAL "System")
