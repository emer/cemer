# Shared declarations for libpdp-based plugin
# Assumes you have QTDIR enviro set

# config info (created by configure)
!include(config.pri) {
  error( "config.pri file is missing or could not be included!" )
}
TEMPLATE = lib

# local root, relative to a plugin src folder, which works whether this is EMERGENTDIR or mypdpdir
THIS_ROOT = ../..

# the specific build folder of the version of msvc we are using
win32 {
# TODO: this would need to be changed for later versions
  BUILD_MSVC = $$(EMERGENTDIR)/build/msvc7
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
LIBS += -L$${EMERGENT_DIR}/lib/plugins$${BUILD_EXT_SF}
LIBS +=	-L$${THIS_ROOT}/lib
win32 {
  DEFINES += QT_DLL TA_DLL EMERGENT_DLL
#  LIBS += -L$${BUILD_MSVC}/lib
#  LIBS += -ltemt$${BUILD_EXT_US} -lemergent$${BUILD_EXT_US}
  LIBS += $${BUILD_MSVC}/lib/temt$${BUILD_EXT_US}.lib
  LIBS += $${BUILD_MSVC}/lib/emergent$${BUILD_EXT_US}.lib
} else {
  LIBS += -L$${EMERGENT_DIR}/lib
  LIBS += -ltemt$${BUILD_EXT_US}-$${LIB_VER}
  LIBS += -lemergent$${BUILD_EXT_US}-$${LIB_VER}
}

# include paths
INCLUDEPATH +=\
	. \
	$${EMERGENT_INC_DIR}  \
	$${THIS_ROOT}/include

MAKETA_INCLUDEPATH +=\
	-I. \
	-I$${EMERGENT_INC_DIR} \
	-I$${THIS_ROOT}/include	
	
# modal include folders -- these are diff depending on platform and dev/retail
# on win32, we must have EMERGENTDIR defined, and we always deploy just like dev
# on unix, we use EMERGENTDIR for dev, and have hier includes, else flat includes
isEmpty( EMERGENT_DIR ) {
  TEMT_INC_DIR_taiqtso = $${EMERGENT_INC_DIR}
  TEMT_INC_DIR_ta = $${EMERGENT_INC_DIR}
  TEMT_INC_DIR_css = $${EMERGENT_INC_DIR}
  EMERGENT_INC_DIR_network = $${EMERGENT_INC_DIR}
  EMERGENT_INC_DIR_bp = $${EMERGENT_INC_DIR}
  EMERGENT_INC_DIR_cs = $${EMERGENT_INC_DIR}
  EMERGENT_INC_DIR_leabra = $${EMERGENT_INC_DIR}
  EMERGENT_INC_DIR_so = $${EMERGENT_INC_DIR}
  
} else { # unix
  TEMT_INC_DIR_taiqtso = $${EMERGENT_INC_DIR}/temt/taiqtso
  TEMT_INC_DIR_ta = $${EMERGENT_INC_DIR}/temt/ta
  TEMT_INC_DIR_css = $${EMERGENT_INC_DIR}/temt/css
  EMERGENT_INC_DIR_network = $${EMERGENT_INC_DIR}/network
  EMERGENT_INC_DIR_bp = $${EMERGENT_INC_DIR}/bp
  EMERGENT_INC_DIR_cs = $${EMERGENT_INC_DIR}/cs
  EMERGENT_INC_DIR_leabra = $${EMERGENT_INC_DIR}/leabra
  EMERGENT_INC_DIR_so = $${EMERGENT_INC_DIR}/so
  
  INCLUDEPATH +=\
	$${EMERGENT_INC_DIR}/temt/taiqtso \
	$${EMERGENT_INC_DIR}/temt/ta \
	$${EMERGENT_INC_DIR}/temt/ta/ios-g++-3.1 \
	$${EMERGENT_INC_DIR}/temt/css \
	$${EMERGENT_INC_DIR}/emergent/network \
	$${EMERGENT_INC_DIR}/emergent/bp \
	$${EMERGENT_INC_DIR}/emergent/cs \
	$${EMERGENT_INC_DIR}/emergent/leabra \
	$${EMERGENT_INC_DIR}/emergent/so
	
  MAKETA_INCLUDEPATH +=\
	-I$${EMERGENT_INC_DIR}/temt/taiqtso \
	-I$${EMERGENT_INC_DIR}/temt/ta \
	-I$${EMERGENT_INC_DIR}/temt/ta/ios-g++-3.1 \
	-I$${EMERGENT_INC_DIR}/temt/css \
	-I$${EMERGENT_INC_DIR}/emergent/network \
	-I$${EMERGENT_INC_DIR}/emergent/bp \
	-I$${EMERGENT_INC_DIR}/emergent/cs \
	-I$${EMERGENT_INC_DIR}/emergent/leabra \
	-I$${EMERGENT_INC_DIR}/emergent/so	
}

win32 {
  INCLUDEPATH +=\
	$${BUILD_MSVC} \
	$${BUILD_MSVC}\\include
	
  MAKETA_INCLUDEPATH +=\
	-I$${BUILD_MSVC}
  DESTDIR = $${EMERGENT_DIR}/lib/plugins$${BUILD_EXT_SF}
} else {
  INCLUDEPATH +=\
	$${EMERGENT_DIR}
	
  MAKETA_INCLUDEPATH +=\
	-I$${EMERGENT_DIR} \
	-I$${QMAKE_INCDIR_QT}/Qt
}

DESTDIR = $${THIS_ROOT}/lib/plugins$${BUILD_EXT_SF}

