################################################################
# Basic Instructions for User
# 
# mkdir cmbuild; cd cmbuild; cmake ../ [-DMPI_BUILD=true for mpi] [-DCMAKE_BUILD_TYPE=Debug for debug]
# ccmake ./ to edit custom options
#

# Important notes:
# 1. do qmake -v -- if it does not say "Using Qt version 4.x.x.." (i.e., it says 3.x.x)
#    then you must find the qmake that is for Qt version 4 and set your path so it is
#    found first!

# Note: required minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

PROJECT(emergent)

# default build type 
#SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_BUILD_TYPE Debug)

# to enable MPI
#SET(MPI_BUILD true)

####################################################
# version setting

SET(EMERGENT_VERSION_MAJOR "4")
SET(EMERGENT_VERSION_MINOR "0")
SET(EMERGENT_VERSION_PATCH "18")
SET(EMERGENT_VERSION "${EMERGENT_VERSION_MAJOR}.${EMERGENT_VERSION_MINOR}.${EMERGENT_VERSION_PATCH}")

####################################################
# misc global stuff: paths, finding packages etc

SET(mod_path "${PROJECT_SOURCE_DIR}/CMakeModules")

SET(CMAKE_MODULE_PATH ${mod_path})

SET(CMAKE_VERBOSE_MAKEFILE ON)

message(STATUS "module path: ${CMAKE_MODULE_PATH} !!!")

find_package(Qt4 REQUIRED)
add_definitions(${QT_DEFINITIONS})

#find_package(Coin REQUIRED)
#FIND_PACKAGE(SoQt REQUIRED)

#include_directories(${QT_INCLUDES} ${COIN_INCLUDE_DIR} ${SOQT_INCLUDE_DIR})

include_directories(${QT_INCLUDES})

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${COIN_LIBRARY})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SOQT_LIBRARY})


#QT4_WRAP_CPP(MY_MOC_SRCS ${MOC_HDRS})

# include(KDE4Defaults)
# include(MacroLibrary)
# include(ConvenienceLibs.cmake)
# include(ManualStuff.cmake)

include(${mod_path}/ConfigureChecks.cmake)
include(${mod_path}/Maketa.cmake)

####################################
#  All the subdirs

add_subdirectory(src/temt/maketa)
# add_subdirectory(src/temt/ta)
# add_subdirectory(src/temt/css)
# add_subdirectory(src/temt/lib)
# add_subdirectory(src/emergent/network)
# add_subdirectory(src/emergent/bp)
# add_subdirectory(src/emergent/leabra)
# add_subdirectory(src/emergent/so)
# add_subdirectory(src/emergent/cs)
# add_subdirectory(src/emergent/lib)
# add_subdirectory(src/emergent/bin)
# add_subdirectory(plugins)
# add_subdirectory(prog_lib)
# add_subdirectory(3dobj_lib)
# add_subdirectory(css_lib)
# add_subdirectory(test)
# add_subdirectory(demo/bp)
# add_subdirectory(demo/cs)
# add_subdirectory(demo/leabra)
# add_subdirectory(demo/so)
# add_subdirectory(demo/virt_env)
# add_subdirectory(demo/data_proc)
# add_subdirectory(src/temt/ta/images)


########### install files ###############

#install(FILES  AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README DESTINATION )

