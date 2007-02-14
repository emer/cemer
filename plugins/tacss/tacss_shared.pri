# Shared declarations for any pdp++ plugin
# Assumes you have ${PDP4DIR} AND ${QTDIR} set

# CONFIG info (created by configure)
# note: have to use ../ because context is a subfolder when read
#!include(../config.pri)
#  message( "config.pri file is missing or could not be included" )

TEMPLATE = lib
CONFIG += plugin warn_off

# we have up to 8 variants of binary, based on: debug, mpi, and nogui
# we set parameters for these below

# DESTDIR - standard location for system (shared by all users) plugins
# we will install in system area if compiling there, or user area, if there

LIB_VER = 3.5

BUILD_EXT_US = _
BUILD_EXT_SF = /

debug {
  mpi {
    nogui {
      BUILD_EXT = debug_mpi_nogui
#      DEFINES += DEBUG DMEM_COMPILE NO_TA_GUI
    } else {
      BUILD_EXT = debug_mpi
#      DEFINES += DEBUG DMEM_COMPILE TA_GUI
    }
  } else {
    nogui {
      BUILD_EXT = debug_nogui
#      DEFINES += DEBUG NO_TA_GUI
    } else {
      BUILD_EXT = debug
#      DEFINES += DEBUG TA_GUI
    }
  }
} else {
  mpi {
    nogui {
      BUILD_EXT = mpi_nogui
#      DEFINES += DMEM_COMPILE NO_TA_GUI
    } else {
      BUILD_EXT = mpi
#      DEFINES += DMEM_COMPILE TA_GUI
    }
  } else {
    nogui {
      BUILD_EXT = nogui
#      DEFINES += NO_TA_GUI
    } else {
      BUILD_EXT = 
      BUILD_EXT_US =
      BUILD_EXT_SF =
#      DEFINES += TA_GUI
    }
  }
}


# value with leading underscore
BUILD_EXT_US = $${BUILD_EXT_US}$${BUILD_EXT}
# value as a subfolder path
BUILD_EXT_SF = $${BUILD_EXT_SF}$${BUILD_EXT}

DESTDIR = ../../../lib/tacss/plugins$${BUILD_EXT_SF}
LIBS += -ltacss$${BUILD_EXT_US}-$${LIB_VER}

INCLUDEPATH +=\
	. \
	$$(PDP4DIR) \
	$$(PDP4DIR)/src/ta \
	$$(PDP4DIR)/src/ta/ios-g++-3.1 \
	$$(PDP4DIR)/src/pdp \
	$$(PDP4DIR)/src/css \
	$$(PDP4DIR)/src/leabra \
	$$(PDP4DIR)/src/taiqtso \
	$$(PDP4DIR)/src/bp \
	$$(PDP4DIR)/include \
	../../include	 

# Tried to do -I w/ a var, but qmake didn't support it
# TODO: There is probably a way to do this w/o making two lists
MAKETA_INCLUDEPATH +=\
	-I. \
	-I$$(PDP4DIR) \
	-I$$(PDP4DIR)/src/ta \
	-I$$(PDP4DIR)/src/ta/ios-g++-3.1 \
	-I$$(PDP4DIR)/src/pdp \
	-I$$(PDP4DIR)/src/css \
	-I$$(PDP4DIR)/src/leabra \
	-I$$(PDP4DIR)/src/taiqtso \
	-I$$(PDP4DIR)/src/bp \
	-I$$(PDP4DIR)/include \
	-I../../include	

HEADERS += $${TARGET}_TA_type.h $${TARGET}_TA_inst.h
SOURCES += $${TARGET}_TA.cpp

LIBS +=	-L$$(PDP4DIR)/lib -L$$../../lib

# in the following, we create a new target 'maketa' which has the indicated properties
maketa.target = $${TARGET}_TA_type.h

maketa.commands = $$(PDP4DIR)/src/maketa/maketa -D__MAKETA__  -css -cpp=\"g++ -E\" $${MAKETA_INCLUDEPATH} $${TARGET} $${MAKETA_HEADERS}

maketa.depends = $${MAKETA_HEADERS}

# the following then creates the maketa target
QMAKE_EXTRA_UNIX_TARGETS += maketa

# the following insures we run the maketa target before trying to compile the project
PRE_TARGETDEPS += $${TARGET}_TA_type.h

QMAKE_CLEAN =\
	$${TARGET}_TA.cpp \
	$${TARGET}_TA_inst.h \
	$${TARGET}_TA_type.h \
	*~ \
	\#*

