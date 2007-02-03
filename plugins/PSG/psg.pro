# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

# Replace hw w/ your plugin name
TARGET = psg

# Declare all the headers in your project -- the following assumes just one
# that is based on the TARGET name

HEADERS += $${TARGET}.h

# Declare all the maketa headers (usually the same as HEADERS) --
# you can just refer to the HEADERS if they are the same

MAKETA_HEADERS = $${HEADERS}

# Declare all the source files, *except* the following:
#  * moc files -- qmake handles those
#  * xx_TA.cpp file -- the shared.pri handles those

SOURCES += $${TARGET}.cpp

include( ../pdp_shared.pri )

CONFIG += link_pkgconfig
PKGCONFIG += playerc++
