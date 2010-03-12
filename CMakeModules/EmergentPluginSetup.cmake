################################################################
# This file does all the setup overhead stuff necessary for a 
# plugin cmake file -- it is the first thing included in the 
# CMakeLists.txt after it finds the installed version of the code
# We know that EMERGENT_SHARE_DIR is the Emergent install folder

################################################################
# Step 1: ensure we have the right version of cmake and set build params
# typically no need to change this stuff, but it has to come first

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(POLICY CMP0011)
   cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

if (WIN32 AND NOT MSVC)
  message(FATAL_ERROR "Only Microsoft Visual C++ (nmake or IDE) is supported")
endif (WIN32 AND NOT MSVC)

# although we know the app folder, we need to extract app install prefix
# note that on Windows, it is the actual app folder, contrary to cmake docs
if (WIN32)
  #note: EMERGENTDIR var must exist...
  if (EMERGENTDIR)
    set(EMERGENT_INSTALL_PREFIX "${EMERGENTDIR}")
  else (EMERGENTDIR)
    message(FATAL_ERROR "EMERGENTDIR variable must exist")
  endif (EMERGENTDIR)
else (WIN32)
  # use manual override first
  set(EMERGENT_INSTALL_PREFIX $ENV{EMERGENT_PREFIX_DIR})
  if (NOT "${EMERGENT_INSTALL_PREFIX}")
    # find the path, in terms of an equivalent to CMAKE_INSTALL_PREFIX
    find_path(EMERGENT_INSTALL_PREFIX share/Emergent/AUTHORS PATHS
      /usr/local
      /usr
      /opt/local
      /opt
      NO_DEFAULT_PATH
    )
  endif (NOT "${EMERGENT_INSTALL_PREFIX}")
endif (WIN32)

################################################################
# EMERGENT_PLUGIN_TYPE determines where to install plugin
# User:   installed in the user area -- only avail for user
# System: installed in the system area -- avail to all users
# see:
# http://grey.colorado.edu/emergent/index.php/CMake#Key_CMake_variables
# http://grey.colorado.edu/emergent/index.php/Plugins

# set the install prefix and actual install location
if ("${EMERGENT_PLUGIN_TYPE}" STREQUAL "System")
  if (WIN32)
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST plugins)
  else (WIN32)
    # we need to independently set CIP to be that of the root of the install
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST lib/Emergent/plugins)
  endif (WIN32)
else ("${EMERGENT_PLUGIN_TYPE}" STREQUAL "System")
  # user is default
  set(EMERGENT_PLUGIN_TYPE "User")  # in case it was never set
  if ($ENV{EMERGENT_USER_PLUGIN_DIR})
    set(CMAKE_INSTALL_PREFIX $ENV{EMERGENT_USER_PLUGIN_DIR} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST .)
  else ($ENV{EMERGENT_USER_PLUGIN_DIR})
    if (WIN32)
      set(CMAKE_INSTALL_PREFIX $ENV{USERPROFILE} CACHE INTERNAL "do not change")
      set(EMERGENT_PLUGIN_DEST Emergent/plugins)
    elseif (APPLE)
      set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE INTERNAL "do not change")
      set(EMERGENT_PLUGIN_DEST Library/Emergent/plugins)
    else (WIN32)
      set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE INTERNAL "do not change")
      set(EMERGENT_PLUGIN_DEST lib/Emergent/plugins)
    endif (WIN32)
  endif ($ENV{EMERGENT_USER_PLUGIN_DIR})
endif ("${EMERGENT_PLUGIN_TYPE}" STREQUAL "System")

set(mod_path "${EMERGENT_SHARE_DIR}/CMakeModules")
set(CMAKE_MODULE_PATH ${mod_path})

# defaults shared by Emergent and plugins
include(${mod_path}/EmergentDefaults.cmake)

# set the lib and executable suffix based on build type -- need this before finding
# the emergent and temt packages!
include(${mod_path}/SetBuildSuffix.cmake)

# packaging and testing systems 
# NOTE: not applicable to plugins at this time
#include(${mod_path}/CPackConfig.cmake)
#include(CPack)
#include(CTest)

# find all of our dependencies -- also sets their include paths in include_directories
# and sets the EMERGENT_DEP_LIBRARIES variable to all the dependency libraries
find_package(EmergentDependencies)

# find full emergent install
find_package(Emergent)
include_directories(${EMERGENT_INCLUDE_DIR})

# several important macros in here:
include(${mod_path}/MacroLibrary.cmake)
# this one does all the configure checks to set various variables
include(${mod_path}/ConfigureChecks.cmake)
# all the support for maketa:
include(${mod_path}/Maketa.cmake)

# use this instead of TARGET_LINK_LIBRARIES -- sets suffix and other properties
MACRO (EMERGENT_PLUGIN_LINK_LIBRARIES _targ _xtra_libs)
    target_link_libraries(${_targ}  ${_xtra_libs} ${EMERGENT_LIBRARIES} ${EMERGENT_DEP_LIBRARIES})
if (false) #not for plugins -- confusing to users and unnecessary, and Unix-specific
    set_target_properties(${_targ} PROPERTIES
      VERSION ${PLUGIN_VERSION}
    )
endif (false)
ENDMACRO (EMERGENT_PLUGIN_LINK_LIBRARIES)
