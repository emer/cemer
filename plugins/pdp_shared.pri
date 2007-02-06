# Shared declarations for any pdp++ plugin
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

# Replace hw w/ your plugin name
TEMPLATE = lib
CONFIG += plugin warn_off

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

# standard location for system (shared by all users) plugins
# we will install in system area if compiling there, or user area, if there
DESTDIR = ../../lib/plugins

#TODO: modalize for debug vs. non-debug
LIBS +=	-lpdp_debug-3.5 -L$$(PDP4DIR)/lib -L$$../../lib

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

