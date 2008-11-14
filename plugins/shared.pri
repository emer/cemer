# Shared declarations for libpdp-based plugin
# Assumes you have ${EMERGENTDIR} AND ${QTDIR} set

# add the master config file as a pseudo-header so we are dependent on it
HEADERS += $${CONFIG_PRI}
# standard headers
HEADERS += $${TARGET}_TA_type.h $${TARGET}_TA_inst.h
SOURCES += $${TARGET}_TA.cpp

# in the following, we create a new target 'maketa' which has the indicated properties
maketa.target = $${TARGET}_TA_type.h

win32 {
  maketa.commands = $$(EMERGENTDIR)\\bin\\maketa.exe -css /D DEBUG -win_dll \
  /I $$(EMERGENT_VC_DIR)\\include \
  /I $$(EMERGENT_VC_DIR) /I $$(QTDIR)\\include\Qt \
  $${MAKETA_INCLUDEPATH} $${TARGET} $${MAKETA_HEADERS}
} else {
  maketa.commands = $${MAKETA} -css -cpp=\"g++ -E\" $${MAKETA_INCLUDEPATH} $${TARGET} $${MAKETA_HEADERS}
}
maketa.depends = $${CONFIG_PRI} $${MAKETA_HEADERS}

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
	
warning($${CONFIG})
