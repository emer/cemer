# Shared declarations for libpdp-based plugin
# Assumes you have ${PDP4DIR} AND ${QTDIR} set

# CONFIG info (created by configure)
!include(config.pri) {
  error( "config.pri file is missing or could not be included!" )
}
TEMPLATE = lib
CONFIG += plugin warn_off

# local root, relative to a plugin src folder, which works whether this is PDPDIR or mypdpdir
THIS_ROOT = ../../..

# the specific build folder of the version of msvc we are using
# TODO: this would need to be changed for later versions
win32 {
BUILD_MSVC = $$(PDP4DIR)/build/msvc7
}

# we have up to 8 variants of binary, based on: debug, mpi, and nogui
# we set parameters for these below

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

# note: all plugins search plugins for dependencies
LIBS += -L$${THIS_ROOT}/lib/plugins$${BUILD_EXT_SF}
LIBS += -L$$(PDP4DIR)/lib/plugins$${BUILD_EXT_SF}
LIBS +=	-L$${THIS_ROOT}/lib
win32 {
  LIBS += -L$${BUILD_MSVC}/lib
  LIBS += $${BUILD_MSVC}/lib/ta$${BUILD_EXT_US}.lib
#  LIBS += -lta$${BUILD_EXT_US}
  LIBS += $${BUILD_MSVC}/lib/pdp$${BUILD_EXT_US}.lib
} else {
  LIBS += -L$$(PDP4DIR)/lib
  LIBS += -ltacss$${BUILD_EXT_US}-$${LIB_VER}
  LIBS += -lpdp$${BUILD_EXT_US}-$${LIB_VER}
}


# following is the basic list for taccs -- pdp extends it
INCLUDEPATH +=\
	. \
	$(PDP4_INC_DIR)/taiqtso \
	$(PDP4_INC_DIR)/ta \
	$(PDP4_INC_DIR)/ta/ios-g++-3.1 \
	$(PDP4_INC_DIR)/css \
	$(PDP4_INC_DIR)/pdp \
	$(PDP4_INC_DIR)/bp \
	$(PDP4_INC_DIR)/cs \
	$(PDP4_INC_DIR)/leabra \
	$(PDP4_INC_DIR)/so \
	$(PDP4_INC_DIR) \
	$${THIS_ROOT}/include	 
win32 {
INCLUDEPATH +=\
	$${BUILD_MSVC}
} else {
INCLUDEPATH +=\
	$$(PDP4DIR)
}

# following is the basic list for taccs -- pdp extends it
MAKETA_INCLUDEPATH +=\
	-I. \
	-I$(PDP4_INC_DIR)/taiqtso \
	-I$(PDP4_INC_DIR)/ta \
	-I$(PDP4_INC_DIR)/ta/ios-g++-3.1 \
	-I$(PDP4_INC_DIR)/css \
	-I$(PDP4_INC_DIR)/pdp \
	-I$(PDP4_INC_DIR)/bp \
	-I$(PDP4_INC_DIR)/cs \
	-I$(PDP4_INC_DIR)/leabra \
	-I$(PDP4_INC_DIR)/so \
	-I$(PDP4_INC_DIR) \
	-I$${THIS_ROOT}/include	
win32 {
MAKETA_INCLUDEPATH +=\
	-I$${BUILD_MSVC}
} else {
MAKETA_INCLUDEPATH +=\
	-I$$(PDP4DIR)
}

DESTDIR = $${THIS_ROOT}/lib/plugins$${BUILD_EXT_SF}

