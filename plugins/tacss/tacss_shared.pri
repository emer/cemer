# Shared declarations for libtacss-based plugin
# Assumes you have ${PDP4DIR} AND ${QTDIR} set

# CONFIG info (created by configure)
!include(../shared.pri) {
  message( "../shared.pri file is missing or could not be included" )
}

DESTDIR = $${THIS_ROOT}/lib/plugins_tacss$${BUILD_EXT_SF}


# note: all following base values are defined for tacss in shared, so none needed here

# LIBS +=

# INCLUDEPATH +=

# MAKETA_INCLUDEPATH +=

