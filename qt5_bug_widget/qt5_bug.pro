# qmake project file for qt5 bug report
# do qmake then make

# on mac, do qmake -spec macx-g++ to not build an xcode project, and just do Makefile

CONFIG += qt debug
QT += opengl
SOURCES += qt5_bug.cpp
OBJECTIVE_SOURCES += mac_objc_code.mm
HEADERS += qt5_bug.h
LIBS += -lobjc

