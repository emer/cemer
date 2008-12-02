################################################################
# Basic Instructions for User
# 
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

################################################################
# Step 1: ensure we have the right version of cmake

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

################################################################
# Step 2: set all the basic parameters about the project

PROJECT(emergent)

# default build type is RelWithDebInfo
IF(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)

# set cache parameter for mpi option
set(MPI_BUILD FALSE CACHE BOOL "Set to true to enable MPI distributed memory system")

#### version setting

set(EMERGENT_VERSION_MAJOR "4")
set(EMERGENT_VERSION_MINOR "0")
set(EMERGENT_VERSION_PATCH "18")
set(EMERGENT_VERSION "${EMERGENT_VERSION_MAJOR}.${EMERGENT_VERSION_MINOR}.${EMERGENT_VERSION_PATCH}")

################################################################
# Step 3: misc global stuff: paths, finding packages etc
# Typically no need to change anything below here

set(mod_path "${PROJECT_SOURCE_DIR}/CMakeModules")

set(CMAKE_MODULE_PATH ${mod_path})

# turn this on for debugging new things
#set(CMAKE_VERBOSE_MAKEFILE ON)

# packaging and testing systems
include(CPack)
include(CTest)

# find all of our dependencies -- also sets their include paths in include_directories
# and sets the EMERGENT_DEP_LIBRARIES variable to all the dependency libraries
find_package(EmergentDependencies)

# inclue all our subdirs in overall path
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src/temt/ta
  ${PROJECT_SOURCE_DIR}/src/temt/taiqtso
  ${PROJECT_SOURCE_DIR}/src/temt/css
  ${PROJECT_SOURCE_DIR}/src/temt/css
  ${PROJECT_SOURCE_DIR}/src/emergent/network
  ${PROJECT_SOURCE_DIR}/src/emergent/bp
  ${PROJECT_SOURCE_DIR}/src/emergent/leabra
  ${PROJECT_SOURCE_DIR}/src/emergent/so
  ${PROJECT_SOURCE_DIR}/src/emergent/cs
)

# this is used for versioning the libs themselves
set(EMERGENT_LIB_VERSION ${EMERGENT_VERSION})
set(EMERGENT_LIB_SOVERSION ${EMERGENT_VERSION_MAJOR})

# install destinations
if (WIN32)
  # we have issues with path spaces, so install to C:/Emergent
  set(CMAKE_INSTALL_PREFIX "C:/")
  # everything gets bundled
  set(EMERGENT_INCLUDE_DEST Emergent/include)
  set(EMERGENT_SHARE_DEST Emergent)
else (WIN32)
  set(EMERGENT_INCLUDE_DEST include/Emergent)
  set(EMERGENT_SHARE_DEST share/Emergent)
endif (WIN32)

# basic compiler warnings, etc.
if (WIN32)
  #shut off warnings: 4250 (inheritance dominance)
  # 4258;4661;4996 (unsafe crt routines)
  add_definitions(/wd4250 /wd4258 /wd4661 /wd4996)
  # enable multi-threaded compiling (always safe, will ignore and print warning when incompatible)
  add_definitions(/MP)
endif (WIN32)

# set the lib and executable suffix based on build type
include(${mod_path}/SetBuildSuffix.cmake)
# several important macros in here:
include(${mod_path}/MacroLibrary.cmake)
# this one does all the configure checks to set various variables
include(${mod_path}/ConfigureChecks.cmake)
# this one makes the config.h from cmake_config.h.in
include(${mod_path}/ConfigureGenerate.cmake)
# all the support for maketa:
include(${mod_path}/Maketa.cmake)

####################################
#  report on status prior to building

message( STATUS )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "********************* Summary of Key Build Parameters *************************" )
message( STATUS "-------------------------------------------------------------------------------" )
message( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message( STATUS "      (Options are: None | Debug | Release | RelWithDebInfo | MinSizeRel)" )
message( STATUS "MPI_BUILD = ${MPI_BUILD}   (true or false)" )
message( STATUS )
message( STATUS "Change a value with: cmake -D<Variable>=<Value>" )
message( STATUS "-------------------------------------------------------------------------------" )


####################################
#  Now the actual building begins: all the subdirs

# TEMT system: The EMergent Toolkit
add_subdirectory(src/temt/maketa)
add_subdirectory(src/temt/taiqtso)
add_subdirectory(src/temt/ta)
add_subdirectory(src/temt/css)
add_subdirectory(src/temt/lib)
add_subdirectory(src/temt/css_bin)
add_subdirectory(src/temt/ta/images)

# Emergent code proper
add_subdirectory(src/emergent/network)
add_subdirectory(src/emergent/bp)
add_subdirectory(src/emergent/leabra)
add_subdirectory(src/emergent/so)
add_subdirectory(src/emergent/cs)
add_subdirectory(src/emergent/lib)
add_subdirectory(src/emergent/bin)

# Plugins
# add_subdirectory(plugins)

# share misc files
add_subdirectory(prog_lib)
add_subdirectory(3dobj_lib)
add_subdirectory(css_lib)
add_subdirectory(test)
add_subdirectory(demo/bp)
add_subdirectory(demo/cs)
add_subdirectory(demo/leabra)
add_subdirectory(demo/so)
add_subdirectory(demo/virt_env)
add_subdirectory(demo/data_proc)

########### install files ###############

install(FILES AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README
  DESTINATION ${EMERGENT_SHARE_DEST}
)

# also install our cmake module files for use by plugins, etc
FILE(GLOB OUR_CMAKE_MOD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/*.cmake")

install(FILES ${OUR_CMAKE_MOD_FILES}
  DESTINATION ${EMERGENT_SHARE_DEST}/CMakeModules
)

########### uninstall files ###############

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
