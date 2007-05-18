# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

!include( ../pdp_shared_pre.pri ) {
  message( "../pdp_shared_pre.pri file is missing or could not be included" )
}

# Replace hw w/ your plugin name
TARGET = virtenv_ode

# Declare all the headers in your project -- the following assumes just one
# that is based on the TARGET name

HEADERS += $${TARGET}.h

# Declare all the maketa headers (usually the same as HEADERS) --
# you can just refer to the HEADERS if they are the same

MAKETA_HEADERS = $${HEADERS}
MAKETA_INCLUDEPATH += -I/usr/local/include/ode 

# Declare all the source files, *except* the following:
#  * moc files -- qmake handles those
#  * xx_TA.cpp file -- the shared.pri handles those

SOURCES += $${TARGET}.cpp

!include( ../pdp_shared.pri ) {
  message( "../pdp_shared.pri file is missing or could not be included" )
}

LIBS += -lode -framework Inventor -framework SoQt

# presumably this allows you to link in an additional package

#CONFIG += link_pkgconfig
#PKGCONFIG += ode
