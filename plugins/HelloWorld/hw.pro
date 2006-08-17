# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

# Replace hw w/ your plugin name
TARGET = hw
TEMPLATE = lib
CONFIG += plugin warn_off

INCLUDEPATH +=\
	. \
	$$(PDP4DIR) \
	$$(PDP4DIR)/src/ta \
	$$(PDP4DIR)/src/ta/ios-g++-3.1 \
	$$(PDP4DIR)/src/pdp \
	$$(PDP4DIR)/src/ta \
	$$(PDP4DIR)/src/css \
	$$(PDP4DIR)/src/leabra \
	$$(PDP4DIR)/src/tamisc \
	$$(PDP4DIR)/src/taiqtso \
	$$(PDP4DIR)/src/bp \
	$$(PDP4DIR)/src/tamisc

# Tried to do -I w/ a var, but qmake didn't support it
# TODO: There is probably a way to do this w/o making two lists
MAKETA_INCLUDEPATH +=\
	-I. \
	-I$$(PDP4DIR) \
	-I$$(PDP4DIR)/src/ta \
	-I$$(PDP4DIR)/src/ta/ios-g++-3.1 \
	-I$$(PDP4DIR)/src/pdp \
	-I$$(PDP4DIR)/src/ta \
	-I$$(PDP4DIR)/src/css \
	-I$$(PDP4DIR)/src/leabra \
	-I$$(PDP4DIR)/src/tamisc \
	-I$$(PDP4DIR)/src/taiqtso \
	-I$$(PDP4DIR)/src/bp \
	-I$$(PDP4DIR)/src/tamisc

# TODO: Parallel build broken due to maketa being called
# simultaneously, and maketa dependencies are improperly set up
PRECOMPILED_HEADER += $${TARGET}_TA_inst.h $${TARGET}_TA_type.h

HEADERS += $${TARGET}.h
SOURCES += $${TARGET}.cpp $${TARGET}_TA.cpp

#TODO: Need standardized places to put plugins
DESTDIR = $$(PDP4DIR)/../plugins

LIBS +=	-lta4 -lpdp4 -L$$(HOME)/lib

maketa.target = $$PRECOMPILED_HEADER
maketa.commands = $$(PDP4DIR)/src/maketa/maketa -D__MAKETA__  -css -cpp="g++ -E" $$MAKETA_INCLUDEPATH $${TARGET} $${TARGET}.h
maketa.depends = $${TARGET}.h

QMAKE_EXTRA_UNIX_TARGETS += maketa

QMAKE_CLEAN =\
	$${TARGET}_TA.cpp \
	$${TARGET}_TA_inst.h \
	$${TARGET}_TA_type.h \
	*~ \
	\#*
