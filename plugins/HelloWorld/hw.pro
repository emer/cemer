# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

!include( ../shared_pre.pri ) {
  message( "../shared_pre.pri file is missing or could not be included" )
}

# Replace hw w/ your plugin name
TARGET = hw

# Declare all the headers in your project -- the following assumes just one
# that is based on the TARGET name

HEADERS += $${TARGET}.h

# Declare all the maketa headers (usually the same as HEADERS) --
# you can just refer to the HEADERS if they are the same
# you should also include xx_TA_type.h files that are referenced
# directly or indirectly by your own headers

MAKETA_HEADERS = $${PDP4_INC_DIR_ta}/ta_TA_type.h $${HEADERS}


# Declare all the source files, *except* the following:
#  * moc files -- qmake handles those
#  * xx_TA.cpp file -- the shared.pri handles those

SOURCES += $${TARGET}.cpp


!include( ../shared.pri ) {
  message( "../shared.pri file is missing or could not be included" )
}
