# qmake project file for quarter examiner viewer example
# do qmake then make

# on mac, do qmake -spec macx-g++ to not build an xcode project, and just do Makefile

CONFIG += qt
QT += opengl
SOURCES += quarter_examiner_viewer.cpp qtthumbwheel.cpp qev_example.cpp
HEADERS += quarter_examiner_viewer.h qtthumbwheel.h

macx {
     LIBS += -framework Quarter -framework Inventor
}
else {
     LIBS += -lQuarter -lCoin
}
