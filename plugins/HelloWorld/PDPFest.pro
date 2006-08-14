# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

# Replace w/ your plugin name
TARGET = PDPFest
TEMPLATE = lib
CONFIG += plugin warn_off

INCLUDEPATH +=\
	. \
	$\${PDP4DIR} \
	$\${PDP4DIR}/src/ta \
	$\${PDP4DIR}/src/ta/ios-g++-3.1 \
	$\${PDP4DIR}/src/pdp \
	$\${PDP4DIR}/src/ta \
	$\${PDP4DIR}/src/css \
	$\${PDP4DIR}/src/leabra \
	$\${PDP4DIR}/src/tamisc \
	$\${PDP4DIR}/src/taiqtso \
	$\${PDP4DIR}/src/bp \
	$\${PDP4DIR}/src/tamisc

# Tried to do -I w/ a var, but qmake didn't support it
MAKETA_INCLUDEPATH +=\
	-I. \
	-I$\${PDP4DIR} \
	-I$\${PDP4DIR}/src/ta \
	-I$\${PDP4DIR}/src/ta/ios-g++-3.1 \
	-I$\${PDP4DIR}/src/pdp \
	-I$\${PDP4DIR}/src/ta \
	-I$\${PDP4DIR}/src/css \
	-I$\${PDP4DIR}/src/leabra \
	-I$\${PDP4DIR}/src/tamisc \
	-I$\${PDP4DIR}/src/taiqtso \
	-I$\${PDP4DIR}/src/bp \
	-I$\${PDP4DIR}/src/tamisc

PRECOMPILED_HEADER +=\
	PDPFest_TA_inst.h \
	PDPFest_TA_type.h

HEADERS += PDPFest.h
SOURCES += PDPFest.cpp PDPFest_TA.cpp

#TODO: how to dynamically determine this...?
#TODO: i bet Qt has a way of snagging an environment var...put it in $\${PDP4DIR}
#TODO: PDP4DIR won't work...or maybe it will. will it try to read directories as plugs? test...
DESTDIR = $\${PDP4DIR}/../plugins

LIBS +=\
	$\${PDP4DIR}/src/ta_lib/.libs/libta4.so \
	$\${PDP4DIR}/src/pdp_lib/.libs/libpdp4.so \
	-lFestival \
	-lestools \
	-lestbase \
	-leststring

maketa.target = $$PRECOMPILED_HEADER
maketa.commands = $\${PDP4DIR}/src/maketa/maketa -D__MAKETA__  -css -cpp="g++ -E" $$MAKETA_INCLUDEPATH PDPFest PDPFest.h
maketa.depends = PDPFest.h

QMAKE_EXTRA_UNIX_TARGETS += maketa

QMAKE_CLEAN =\
	PDPFest_TA.cpp \
	PDPFest_TA_inst.h \
	PDPFest_TA_type.h \
	*~ \
	\#*
