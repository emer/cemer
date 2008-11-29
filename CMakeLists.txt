################################################################
# Basic Instructions for User
# 
# mkdir cmbuild;
# cd cmbuild;
# cmake [-G XCode] [-DMPI_BUILD=true]
#  [-DCMAKE_BUILD_TYPE=None | Debug | Release | RelWithDebInfo | MinSizeRel]
#  ../ 
# ccmake ./ to edit custom options
#

# Important notes:
# 1. do qmake -v -- if it does not say "Using Qt version 4.x.x.." (i.e., it says 3.x.x)
#    then you must find the qmake that is for Qt version 4 and set your path so it is
#    found first!

# Note: required minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0 FATAL_ERROR)

PROJECT(emergent)

ease CACHE STRING "Type of build to make: Debug or Release"
# crazy how hard it is to set the default build type 
IF(NOT CMAKE_BUILD_TYPE)
# this does nothing:
#  set(CMAKE_BUILD_TYPE Release CACHE STRING "Type of build to make: Debug or Release")
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)

# to enable MPI
set(MPI_BUILD FALSE CACHE BOOL "Set to true to enable MPI distributed memory system")

# default install prefix: hopefully /usr/local/ ??
#set(CMAKE_INSTALL_PREFIX /usr/local)

####################################################
# version setting

set(EMERGENT_VERSION_MAJOR "4")
set(EMERGENT_VERSION_MINOR "0")
set(EMERGENT_VERSION_PATCH "18")
set(EMERGENT_VERSION "${EMERGENT_VERSION_MAJOR}.${EMERGENT_VERSION_MINOR}.${EMERGENT_VERSION_PATCH}")

####################################################
# misc global stuff: paths, finding packages etc

set(mod_path "${PROJECT_SOURCE_DIR}/CMakeModules")

set(CMAKE_MODULE_PATH ${mod_path})

#set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Qt4 REQUIRED)
find_package(Coin REQUIRED)
find_package(SoQt REQUIRED)
find_package(Readline REQUIRED)
find_package(ODE)
find_package(GSL)

# setup QT_LIBRARIES, defines, etc through options, and the QT_USE_FILE thing does automagic
# TODO: can't figure out why it is still complaining about qt3 support but it is
# need to fix that!
set(QT_USE_QT3SUPPORT 1)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTXML 1)
include(${QT_USE_FILE})

include_directories(${QT_INCLUDES} ${COIN_INCLUDE_DIR} ${SOQT_INCLUDE_DIR}
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

# all libraries to link to -- used automatically in EMERGENT_LINK_LIBRARIES
# specify in executables
set(EMERGENT_LIBRARIES ${COIN_LIBRARY} ${SOQT_LIBRARY} ${QT_LIBRARIES}
    ${READLINE_LIBRARY}
    ${ODE_LIBRARY}
    ${GSL_LIBRARY}
)

# this is used for versioning the libs themselves
set(EMERGENT_LIB_VERSION ${EMERGENT_VERSION})
set(EMERGENT_LIB_SOVERSION ${EMERGENT_VERSION_MAJOR})

# install destinations
set(EMERGENT_INCLUDE_DEST include/Emergent)
set(EMERGENT_SHARE_DEST share/Emergent)

# determine the suffix based on build type

IF(CMAKE_BUILD_TYPE MATCHES "Debug") 
    IF(MPI_BUILD)
	set(CMAKE_DEBUG_POSTFIX "_dbg_mpi")
	set(CMAKE_EXECUTABLE_SUFFIX "_dbg_mpi")
    ELSE(MPI_BUILD)
	set(CMAKE_DEBUG_POSTFIX "_dbg")
	set(CMAKE_EXECUTABLE_SUFFIX "_dbg")
    ENDIF(MPI_BUILD)
ELSE(CMAKE_BUILD_TYPE MATCHES "Debug") 
    IF(MPI_BUILD)
	set(CMAKE_RELEASE_POSTFIX "_mpi")
	set(CMAKE_EXECUTABLE_SUFFIX "_mpi")
    ELSE(MPI_BUILD)
	set(CMAKE_RELEASE_POSTFIX "")
	set(CMAKE_EXECUTABLE_SUFFIX "")
    ENDIF(MPI_BUILD)
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug") 


# NOTE: several important macros in here:
include(${mod_path}/MacroLibrary.cmake)
# this one makes the config.h from cmake_config.h.in
include(${mod_path}/ConfigureChecks.cmake)
# all the support for maketa:
include(${mod_path}/Maketa.cmake)

####################################
#  All the subdirs

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

########### uninstall files ###############

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
