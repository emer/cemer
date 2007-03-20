# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

include( ../pdp_shared_pre.pri )

# Replace hw w/ your plugin name
TARGET = psg

# Declare all the headers in your project -- the following assumes just one
# that is based on the TARGET name

HEADERS += $${TARGET}.h 

# Declare all the maketa headers (usually the same as HEADERS) --
# you can just refer to the HEADERS if they are the same
MAKETA_HEADERS = $${HEADERS}
MAKETA_INCLUDEPATH += -I/usr/local/include/player-2.0

PSG_HEADERS = libplayerc++/player.h libplayerc++/playerc.h libplayerc++/playerc++.h libplayerc++/error.h libplayerc++/functiontable.h libplayerc++/playerxdr.h libplayerc++/debug.h libplayerc++/playerclient.h libplayerc++/playererror.h

INCLUDEPATH += libplayerc++

HEADERS += $${HEADERS}

# Declare all the source files, *except* the following:
#  * moc files -- qmake handles those
#  * xx_TA.cpp file -- the shared.pri handles those

SOURCES += $${TARGET}.cpp libplayerc++/playerxdr.c libplayerc++/error.c libplayerc++/functiontable.c libplayerc++/playerc++.cc libplayerc++/playerclient.cc libplayerc++/playererror.cc libplayerc++/clientproxy.cc libplayerc++/cameraproxy.cc libplayerc++/graphics2dproxy.cc libplayerc++/graphics3dproxy.cc libplayerc++/gripperproxy.cc libplayerc++/laserproxy.cc libplayerc++/limbproxy.cc libplayerc++/position2dproxy.cc libplayerc++/position3dproxy.cc libplayerc++/simulationproxy.cc


include( ../pdp_shared.pri )

CONFIG += link_pkgconfig
PKGCONFIG += playerc++
