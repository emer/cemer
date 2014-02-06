# Install script for directory: /Users/rohrlich/emergent/src/temt/json

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Emergent" TYPE FILE FILES
    "/Users/rohrlich/emergent/src/temt/json/internalJSONNode.h"
    "/Users/rohrlich/emergent/src/temt/json/JSON_Base64.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONAllocator.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONChildren.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONDefs.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONDebug.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONGlobals.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONMemory.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONMemoryPool.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONNode.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONOptions.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONPreparse.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONSharedString.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONSingleton.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONStats.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONStream.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONValidator.h"
    "/Users/rohrlich/emergent/src/temt/json/JSONWorker.h"
    "/Users/rohrlich/emergent/src/temt/json/libjson.h"
    "/Users/rohrlich/emergent/src/temt/json/NumberToString.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Emergent" TYPE FILE FILES
    "/Users/rohrlich/emergent/include/internalJSONNode"
    "/Users/rohrlich/emergent/include/JSONAllocator"
    "/Users/rohrlich/emergent/include/JSONChildren"
    "/Users/rohrlich/emergent/include/JSONDebug"
    "/Users/rohrlich/emergent/include/JSONMemory"
    "/Users/rohrlich/emergent/include/JSONNode"
    "/Users/rohrlich/emergent/include/JSONPreparse"
    "/Users/rohrlich/emergent/include/JSONStream"
    "/Users/rohrlich/emergent/include/JSONValidator"
    "/Users/rohrlich/emergent/include/JSONWorker"
    "/Users/rohrlich/emergent/include/libjson"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

