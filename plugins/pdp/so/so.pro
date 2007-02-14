# Make a plugin for pdp++
# Assumes you have ${HOME}, ${PDP4DIR} AND ${QTDIR} set

# root target name of the plugin
TARGET = so

# Declare all the headers in your project 

HEADERS +=  so.h # cl.h som.h zsh.h

# Declare all the maketa headers (usually the same as HEADERS) --
# you can just refer to the HEADERS if they are the same

MAKETA_HEADERS = $${HEADERS} 


# Declare all the source files, *except* the following:
#  * moc files -- qmake handles those
#  * xx_TA.cpp file -- the shared.pri handles those

SOURCES += so.cpp # cl.cpp som.cpp zsh.cpp


!include( ../pdp_shared.pri ) {
  message( "../pdp_shared.pri file is missing or could not be included" )
}

