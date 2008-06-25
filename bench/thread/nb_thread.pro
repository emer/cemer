#CONFIG		+= console debug
CONFIG		+= console 
DEFINES		+= NB_THREAD SEND_CONS
QMAKE_INCDIR	+= ../shared
HEADERS       = ../shared/nb_util.h \
		../shared/nb_netstru.h
SOURCES       = ../shared/nb_util.cpp \
		../shared/nb_netstru.cpp \
		nb_thread.cpp

