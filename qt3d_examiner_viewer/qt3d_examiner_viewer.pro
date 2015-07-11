# qmake project file for quarter examiner viewer example
# do qmake then make

# on mac, do qmake -spec macx-g++ to not build an xcode project, and just do Makefile

CONFIG += qt debug warn_off
QT += 3dcore 3drenderer 3dinput
QT += printsupport
SOURCES += qt3d_examiner_viewer.cpp qtthumbwheel.cpp qev_example.cpp window.cpp
HEADERS += qt3d_examiner_viewer.h qtthumbwheel.h window.h

macx {
     QMAKE_CXXFLAGS += -Wno-unknown-pragmas
#     INCLUDEPATH += /Library/Frameworks/Quarter.framework/Headers
#     INCLUDEPATH += /Library/Frameworks/Inventor.framework/Headers
#     LIBS += -framework Quarter -framework Inventor
}
else {
#     LIBS += -lQuarter -lCoin
}
