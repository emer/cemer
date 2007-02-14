# Shared declarations for libpdp-based plugin
# Assumes you have ${PDP4DIR} AND ${QTDIR} set

# CONFIG info (created by configure)
!include(../shared.pri) {
  message( "../shared.pri file is missing or could not be included" )
}

DESTDIR = $${THIS_ROOT}/lib/plugins_pdp$${BUILD_EXT_SF}

LIBS += -lpdp$${BUILD_EXT_US}-$${LIB_VER}
# shared file includes tacss plugin folder already
LIBS += -L$${THIS_ROOT}/lib/plugins_pdp$${BUILD_EXT_SF}
LIBS += -L$$(PDP4DIR)/lib/plugins_pdp$${BUILD_EXT_SF}

# shared.pri defined the basic ones for tacss -- we extend for pdp
INCLUDEPATH +=\
	$$(PDP4DIR)/src/pdp \
	$$(PDP4DIR)/src/leabra \
	$$(PDP4DIR)/src/bp 

# shared.pri defined the basic ones for tacss -- we extend for pdp
MAKETA_INCLUDEPATH +=\
	-I$$(PDP4DIR)/src/pdp \
	-I$$(PDP4DIR)/src/leabra \
	-I$$(PDP4DIR)/src/bp 

