# qmake project file for qt5 bug report
# do qmake then make

# on mac, do qmake -spec macx-clang to not build an xcode project, and just do Makefile

TEMPLATE = app

CONFIG += qt debug
QT += widgets webengine webenginewidgets
SOURCES += qt5_bug.cpp
HEADERS += qt5_bug.h
