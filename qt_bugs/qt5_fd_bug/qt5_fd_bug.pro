# qmake project file for qt5 bug report
# do qmake then make

# on mac, do qmake -spec macx-g++ to not build an xcode project, and just do Makefile
# ~/Qt5.2.0/5.2.0-beta1/clang_64/bin/qmake -spec macx-g++ -o Makefile qt5_fd_bug.pro

CONFIG += qt debug
QT += opengl
SOURCES += qt5_fd_bug.cpp 
# HEADERS += qt5_fd_bug.h

