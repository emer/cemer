CONFIG		+= console debug
#CONFIG		+= console release
#note: need to modalize for Windows
QMAKE_CXXFLAGS_RELEASE += -g
DEFINES		+= NB_THREAD 
debug {
  DEFINES	+= DEBUG
}
#SEND_CONS
QMAKE_INCDIR	+= ../shared
HEADERS       = ../shared/nb_util.h \
		../shared/nb_netstru.h
SOURCES       = ../shared/nb_util.cpp \
		../shared/nb_netstru.cpp \
		nb_thread.cpp

