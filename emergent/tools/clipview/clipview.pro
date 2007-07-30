TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on debug
QT += qt3support

HEADERS     	= frm_main.h
SOURCES		= frm_main.cpp main.cpp























































unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}


