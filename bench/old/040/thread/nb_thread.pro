#CONFIG		+= console debug
CONFIG		+= console release
windows {
#TODO need to modalize for Windows -- add cc flag that adds debug symbols
} else {
  QMAKE_CXXFLAGS_RELEASE += -g
}
DEFINES		+= NB_THREAD 
# note: for some reason, using debug here was still true
release {
} else {
  DEFINES	+= DEBUG
}
QMAKE_INCDIR	+= ../shared
HEADERS       = ../shared/nb_util.h \
		../shared/nb_netstru.h
SOURCES       = ../shared/nb_util.cpp \
		../shared/nb_netstru.cpp \
		nb_thread.cpp

