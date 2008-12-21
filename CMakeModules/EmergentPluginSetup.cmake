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
  set(EMERGENT_PLUGIN_TYPE User)
endif (NOT EMERGENT_PLUGIN_TYPE)

# although we know the app folder, we need to extract install prefix
# note that on Windows, it is the actual app folder, contrary to cmake docs
if (WIN32)
  #note: EMERGENTDIR var must exist...
  if (EMERGENTDIR)
    set(EMERGENT_INSTALL_PREFIX "${EMERGENTDIR}")
  else (EMERGENTDIR)
    message(FATAL_ERROR "EMERGENTDIR variable must exist")
  endif (EMERGENTDIR)
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
    set(EMERGENT_PLUGIN_DEST plugins)
  else (WIN32)
    # we need to independently set CIP to be that of the root of the install
    set(CMAKE_INSTALL_PREFIX ${EMERGENT_INSTALL_PREFIX} CACHE INTERNAL "do not change")
    set(EMERGENT_PLUGIN_DEST lib/Emergent/plugins)
  endif (WIN32)
elseif (EMERGENT_PLUGIN_TYPE STREQUAL "User")
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
else (EMERGENT_PLUGIN_TYPE STREQUAL "User")
  message(FATAL_ERROR "EMERGENT_PLUGIN_TYPE must be set to one of: User | System")
endif (EMERGENT_PLUGIN_TYPE STREQUAL "System")

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
