# qmake project file for quarter examiner viewer example
# do qmake then make

# on mac, do qmake -spec macx-g++ to not build an xcode project, and just do Makefile

CONFIG += qt debug
QT += opengl
QT += printsupport
SOURCES += quarter_examiner_viewer.cpp qtthumbwheel.cpp qev_example.cpp
HEADERS += quarter_examiner_viewer.h qtthumbwheel.h

macx {
     QMAKE_LFLAGS += -F/usr/local/Qt5.3.0/5.3/clang_64/lib  -F/Library/Frameworks
     INCLUDEPATH += /Library/Frameworks/Quarter.framework/Headers
     INCLUDEPATH += /Library/Frameworks/Inventor.framework/Headers
     LIBS += -framework Quarter -framework Inventor
}
else {
     LIBS += -lQuarter -lCoin
}
