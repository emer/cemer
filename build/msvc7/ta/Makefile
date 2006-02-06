INCLUDE = -I..\ -I$(PDP4DIR)\src -I$(PDP4DIR)\src\ta -I$(PDP4DIR)\src\taiqtso -I$(PDP4DIR)\src\css -I$(PDP4DIR)\src\tamisc
MOC = $(QTDIR)\bin\moc $(INCLUDE)

all: moc_files

TAIQTSO_MOCS: \
  icheckbox.moc.cpp \
  icombobox.moc.cpp \
  ieditgrid.moc.cpp \
  iflowlayout.moc.cpp \
  ilineedit.moc.cpp \
  igeometry.moc.cpp \
  irenderarea.moc.cpp \
  ispinbox.moc.cpp \
  itreeview.moc.cpp 

icheckbox.moc.cpp: $(PDP4DIR)\src\taiqtso\icheckbox.h
	$(MOC) %s -o $@

icombobox.moc.cpp: $(PDP4DIR)\src\taiqtso\icombobox.h
	$(MOC) %s -o $@

ieditgrid.moc.cpp: $(PDP4DIR)\src\taiqtso\ieditgrid.h
	$(MOC) %s -o $@

iflowlayout.moc.cpp: $(PDP4DIR)\src\taiqtso\iflowlayout.h
	$(MOC) %s -o $@

ilineedit.moc.cpp: $(PDP4DIR)\src\taiqtso\ilineedit.h
	$(MOC) %s -o $@

igeometry.moc.cpp: $(PDP4DIR)\src\taiqtso\igeometry.h
	$(MOC) %s -o $@

irenderarea.moc.cpp: $(PDP4DIR)\src\taiqtso\irenderarea.h
	$(MOC) %s -o $@

ispinbox.moc.cpp: $(PDP4DIR)\src\taiqtso\ispinbox.h
	$(MOC) %s -o $@

itreeview.moc.cpp: $(PDP4DIR)\src\taiqtso\itreeview.h
	$(MOC) %s -o $@

clean: 
  del *.moc.cpp
  
moc_files: \
  TAIQTSO_MOCS
