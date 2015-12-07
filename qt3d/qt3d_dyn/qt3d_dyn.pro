TEMPLATE = app

QT += 3dcore 3drender 3dinput

SOURCES += \
    main.cpp \
    T3LineStrip.cpp \
    T3TriangleStrip.cpp \
    T3Frame.cpp
    
HEADERS += T3LineStrip.h T3TriangleStrip.h T3Frame.h

CONFIG+=debug
