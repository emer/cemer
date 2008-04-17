# Audioproc plugin
# Assumes you have ${HOME}, ${EMERGENTDIR} AND ${QTDIR} set

!include( ../shared_pre.pri ) {
  message( "../shared_pri.pri file is missing or could not be included" )
}

# plugin name
TARGET = audioproc

# Declare all the headers in your project

HEADERS += $${TARGET}_pl.h $${TARGET}.h gammatone.h ap_files.h

# Declare all the maketa headers (usually the same as HEADERS) --
# you can just refer to the HEADERS if they are the same

warning($${EMERGENT_DIR})
warning($${EMERGENT_INC_DIR})
warning($${TEMT_INC_DIR_ta})

# TEMP: test adding the root headers, as we do in pdp
MAKETA_HEADERS = $${TEMT_INC_DIR_ta}/ta_TA_type.h $${HEADERS}


# Declare all the source files, *except* the following:
#  * moc files -- qmake handles those
#  * xx_TA.cpp file -- the shared.pri handles those

SOURCES += $${TARGET}_pl.cpp $${TARGET}.cpp gammatone.cpp ap_files.cpp

win32 {
  DEFINES += AUDIOPROC_DLL AUDIOPROC_EXPORTS
  LIBS += $${BUILD_MSVC}/lib/libsndfile-1.lib
}

# NOTE: macx also invokes unix, so we do an else
macx {
#  QMAKE_INCDIR += /System/Library/Frameworks/OpenAL.framework/Headers
  QMAKE_LFLAGS += -L/System/Library/Frameworks
#  LIBS += -framework OpenAL -L/usr/local/lib -lsndfile
  LIBS += -L/usr/local/lib -lsndfile
} else:unix {
  LIBS += -L/usr/local/lib -L/usr/lib -lsndfile
#  LIBS += -L/usr/local/lib -L/usr/lib -lsndfile -lopenal
}

!include( ../shared.pri ) {
  message( "../shared.pri file is missing or could not be included" )
}
