# Shared declarations for libpdp-based plugin
# Assumes you have ${PDP4DIR} AND ${QTDIR} set

HEADERS += $${TARGET}_TA_type.h $${TARGET}_TA_inst.h
SOURCES += $${TARGET}_TA.cpp
MSVC_FOLDER = msvc7

# in the following, we create a new target 'maketa' which has the indicated properties
maketa.target = $${TARGET}_TA_type.h

win32 {
  maketa.commands = $$(PDP4DIR)\\build\\$${MSVC_FOLDER}\\bin\\maketa.exe -css /D DEBUG -win_dll \
  /I $$(PDP4DIR)\\build\\$${MSVC_FOLDER}\\include \
  /I $$(PDP4DIR)\\build\\$${MSVC_FOLDER} \
  $${MAKETA_INCLUDEPATH} $${TARGET} $${MAKETA_HEADERS}
} else {
  maketa.commands = $$(PDP4DIR)/bin/maketa -css -cpp=\"g++ -E\" $${MAKETA_INCLUDEPATH} $${TARGET} $${MAKETA_HEADERS}
}
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

