INCLUDE = /I "$(PDP4DIR)\src\ta" /I "$(PDP4DIR)/src/ta/ios-g++-3.1" \
  /I "$(PDP4DIR)\src\taiqtso" /I "$(PDP4DIR)\src\css" /I "$(PDP4DIR)\src\tamisc" \
  /I "$(PDP4DIR)\src\pdp" /I "$(PDP4DIR)\src\bp"  /I "$(PDP4DIR)\src\leabra" \
  /I "$(QTDIR)/include/Qt" /I "$(QTDIR)/include/Qt3Support" \
  /I "$(QTDIR)/include/QtCore" /I "$(QTDIR)/include/QtGui" \
  /I "$(QTDIR)/include/QtNetwork" /I "$(QTDIR)/include/QtOpenGL" \
  /I "$(QTDIR)/include/QtXml" /I "$(COINDIR)/include/Inventor" \
  /I "$(PDP4DIR)\build\msvc7" /I "$(VCINSTALLDIR)\Vc7\include" 
CD = cd   
MOC = $(QTDIR)\bin\moc $(INCLUDE)
#MAKETA = $(PDP4DIR)\build\msvc7\maketa\Debug\maketa.exe
MAKETA = $(PDP4DIR)\build\msvc7\maketa\Release\maketa.exe


all: moc_files

TAIQTSO_MOCS: \
  ilabel.moc.cpp \
  idimedit.moc.cpp \
  icheckbox.moc.cpp \
  icombobox.moc.cpp \
  ieditgrid.moc.cpp \
  iflowlayout.moc.cpp \
  ilineedit.moc.cpp \
  igeometry.moc.cpp \
  irenderarea.moc.cpp \
  ispinbox.moc.cpp \
  itreeview.moc.cpp 

idimedit.moc.cpp: $(PDP4DIR)\src\taiqtso\idimedit.h
	$(MOC) %s -o $@

ilabel.moc.cpp: $(PDP4DIR)\src\taiqtso\ilabel.h
	$(MOC) %s -o $@

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

TA_MAKETA_GUI =\
	$(PDP4DIR)/src/ta/ta_qt.h \
	$(PDP4DIR)/src/ta/ta_matrix_qt.h \
	$(PDP4DIR)/src/ta/ta_qtdata_def.h \
	$(PDP4DIR)/src/ta/ta_qtdata.h \
	$(PDP4DIR)/src/ta/ta_qtdialog.h \
	$(PDP4DIR)/src/ta/ta_qttype_def.h \
	$(PDP4DIR)/src/ta/ta_qttype.h \
	$(PDP4DIR)/src/ta/ta_qtclipdata.h \
	$(PDP4DIR)/src/ta/ta_qtbrowse_def.h \
	$(PDP4DIR)/src/ta/ta_qtviewer.h \
	$(PDP4DIR)/src/ta/ta_qtbrowse.h \
	$(PDP4DIR)/src/ta/ta_classbrowse.h \
	$(PDP4DIR)/src/ta/ta_qtgroup.h \
	$(PDP4DIR)/src/ta/ta_seledit.h \
	$(PDP4DIR)/src/taiqtso/igeometry.h

TA_MAKETA =\
	$(PDP4DIR)/src/ta/ios-g++-3.1/streambuf.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/iostream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/fstream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/sstream.h \
	$(PDP4DIR)/src/ta/ta_def.h \
	$(PDP4DIR)/src/ta/ta_string.h \
	$(PDP4DIR)/src/ta/ta_stdef.h \
	$(PDP4DIR)/src/ta/ta_variant.h \
	$(PDP4DIR)/src/ta/ifont.h \
	$(PDP4DIR)/src/ta/ta_list.h \
	$(PDP4DIR)/src/ta/ta_filer.h \
	$(PDP4DIR)/src/ta/icolor.h \
	$(PDP4DIR)/src/ta/ta_type.h \
	$(PDP4DIR)/src/ta/ta_base.h \
	$(PDP4DIR)/src/ta/ta_group.h \
	$(PDP4DIR)/src/ta/ta_matrix.h \
	$(PDP4DIR)/src/ta/ta_dump.h \
	$(PDP4DIR)/src/ta/ta_defaults.h \
	$(PDP4DIR)/src/ta/ta_script.h \
	$(TA_MAKETA_GUI)

#MAKETA_FLAGS = /I "$(VCINSTALLDIR)\Vc7\include"
MAKETA_FLAGS = $(INCLUDE) 

#note: also makes _TA_inst.h and .cpp
ta_TA_type.h: $(TA_MAKETA)
  $(CD) $(PDP4DIR)/src/ta
  $(MAKETA) -v1 $(MAKETA_FLAGS) ta $(TA_MAKETA) 
  
clean: 
  del *.moc.cpp
  
moc_files: \
  TAIQTSO_MOCS
