CONFIG		+= console debug
#CONFIG		+= console 
DEFINES		+= NB_CUDA SEND_CONS
QMAKE_INCDIR	+= ../shared
HEADERS       = ../shared/nb_util.h \
		../shared/nb_netstru.h
SOURCES       = ../shared/nb_util.cpp \
		../shared/nb_netstru.cpp \
		nb_cuda.cpp
# lib stuff
LIBS		+= -lembench_cuda -L./release	

