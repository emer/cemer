# see Unix section for documentation of switches
# this file is copied directly to .pri on Windows

# for some whacky reason, CONFIG had way too much default stuff, so we init it here:
CONFIG = lex yacc plugin warn_off uic resources qt incremental exceptions rtti stl link_prl thread
# NOTE: nasty on Mac! causes libtool to fail to find libs
#   but it was in the default list on Linux
!macx {
  CONFIG += qt_no_framework
}
# on Unix, if empty, then inc/lib etc. paths are from install
EMERGENT_DIR = $$(EMERGENTDIR)

# the temt/emergent version string, for adding to libs and folder (Unix only)
  LIB_VER = 4.0.18

win32 {
  CONFIG += dll
# TODO: SSE2 generates harmless warning on win64; try to modalize for win64
  QMAKE_CXXFLAGS += /arch:SSE2
  EMER_TARGET = $$(EMER_TARGET)

  contains( EMER_TARGET, Debug ) {
    CONFIG += debug
    QMAKE_CXXFLAGS += /RTC1 /GS
  } else { 
    contains( EMER_TARGET, Release ) {
      CONFIG += release
    } else {
      EMER_TARGET = Release
      CONFIG += release
      warning("EMER_TARGET variable should be defined as either Debug or Release; 'Release' has been assumed")
    }
  }
  
  SYS_PLUGIN_DIR = $$(EMERGENTDIR)/plugins
  CONFIG_PRI = $$(EMERGENTDIR)/plugins/config.pri
  EMERGENT_SYS_DIR = $$(EMERGENTDIR)
  EMERGENT_INC_DIR = $${EMERGENT_DIR}/src
  MAKETA = $$(EMERGENTDIR)/bin/maketa
  
#TODO: add our emergent project depends on Windows, ex ode etc.
#  LIBS +=   
  
} else { // mac and unix -- have common elements and disparate ones too...
  # configuration for all cases
  # the modal paths in case that EMERGENTDIR is defined -- same for both
  isEmpty( EMERGENT_DIR ) {
    # config in this case defaults to Release unless overridden
    # only warn for explicit Debug case, which may not be intended
    contains( EMER_TARGET, Debug ) {
      CONFIG += debug
      QMAKE_CXXFLAGS += /RTC1 /GS
      warning("EMER_TARGET variable was defined as 'Debug' which is unexpected when building plugins without EMERGENTDIR defined (i.e. for system installation) -- 'unset EMER_TARGET' if you want to build Release version of plugin")
    } else { 
      CONFIG += release
    }
  } else { // EMERGENTDIR env variable, assume dev install
    CONFIG_PRI = $${EMERGENT_DIR}/plugins/config.pri
    EMERGENT_INC_DIR = $${EMERGENT_DIR}/src
    LIBS += -L$${EMERGENT_DIR}/src/temt/lib/.libs
    LIBS += -L$${EMERGENT_DIR}/src/emergent/lib/.libs
    MAKETA = $${EMERGENT_DIR}/src/temt/maketa/maketa
    # config in this case defaults to Debug...
    contains( EMER_TARGET, Release ) {
      CONFIG += release
    } else { 
      CONFIG += debug
      QMAKE_CXXFLAGS += /RTC1 /GS
      !contains( EMER_TARGET, Debug ) {
        warning("EMER_TARGET variable should be defined as either Debug or Release; 'Debug' has been assumed")
      }
    }
  }
  
# standard retail install paths
  prefix = /usr/local
  bindir = $${prefix}/bin
  datadir = $${prefix}/share
  includedir = $${prefix}/include
    
  
  macx {
    LIBS +=  -lgslcblas -lm -lode -ldl -lgsl -lreadline -Wl,-F/Library/Frameworks -Wl,-framework,QtGui -Wl,-framework,QtOpenGL -Wl,-framework,QtCore -Wl,-framework,Qt3Support -Wl,-framework,QtXml -Wl,-framework,QtNetwork -Wl,-framework,QtSql 
  } else { // unix
  #TODO: also add support for mpi
  
    # now, the modal paths for when EMERGENTDIR is *not* defined
    isEmpty( EMERGENT_DIR ) {
      CONFIG_PRI = $${datadir}/Emergent/plugins/config.pri
      EMERGENT_INC_DIR = $${includedir}/emergent
      MAKETA = $${bindir}/maketa
    }
    
    LIBS +=  -lgslcblas -lm -lode -ldl -lgsl -lreadline -lQtGui -lQt3Support -lQtNetwork -lQtOpenGL -lQtCore -lCoin -lGL -lXext -lSM -lICE -lX11 -lpthread -lSoQt -lXmu -lXi 
  }
  
}

