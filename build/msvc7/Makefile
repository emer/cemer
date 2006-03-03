VCINSTALLDIR = C:\Program Files\Microsoft Visual Studio .NET 2003
INCLUDE = "-I$(PDP4DIR)\src\ta" "-I$(PDP4DIR)/src/ta/ios-g++-3.1" \
  "-I$(PDP4DIR)\src\taiqtso" "-I$(PDP4DIR)\src\css" "-I$(PDP4DIR)\src\tamisc" \
  "-I$(PDP4DIR)\src\pdp" "-I$(PDP4DIR)\src\bp"  "-I$(PDP4DIR)\src\leabra" \
  "-I$(QTDIR)/include/Qt" "-I$(QTDIR)/include/Qt3Support" \
  "-I$(QTDIR)/include/QtCore" "-I$(QTDIR)/include/QtGui" \
  "-I$(QTDIR)/include/QtNetwork" "-I$(QTDIR)/include/QtOpenGL" \
  "-I$(QTDIR)/include/QtXml" "-I$(COINDIR)/include/Inventor" \
  "-I$(PDP4DIR)\build\msvc7" "-I$(VCINSTALLDIR)\Vc7\include" 
CD = cd   
MOC = $(QTDIR)\bin\moc $(INCLUDE)
#MAKETA = $(PDP4DIR)\build\msvc7\maketa\Debug\maketa.exe
MAKETA = $(PDP4DIR)\build\msvc7\maketa\Release\maketa.exe


all: maketa_files moc_files

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
	ta_qt.h \
	ta_matrix_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtclipdata.h \
	ta_qtbrowse_def.h \
	ta_qtviewer.h \
	ta_qtbrowse.h \
	ta_classbrowse.h \
	ta_qtgroup.h \
	ta_seledit.h \
	$(PDP4DIR)/src/taiqtso/igeometry.h

TA_MAKETA =\
	$(PDP4DIR)/src/ta/ios-g++-3.1/streambuf.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/iostream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/fstream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/sstream.h \
	taglobal.h \
	ta_def.h \
	ta_string.h \
	ta_stdef.h \
	ta_variant.h \
	ifont.h \
	ta_list.h \
	ta_filer.h \
	icolor.h \
	ta_type.h \
	ta_base.h \
	ta_group.h \
	ta_matrix.h \
	ta_dump.h \
	ta_defaults.h \
	ta_script.h \
	$(TA_MAKETA_GUI)

TA_MOCS: \
  ta_qt.moc.cpp \
  ta_matrix_qt.moc.cpp \
  ta_qtdata_def.moc.cpp \
  ta_qtdata.moc.cpp \
  ta_qtdialog.moc.cpp \
  ta_qttype_def.moc.cpp \
  ta_qttype.moc.cpp \
  ta_qtclipdata.moc.cpp \
  ta_qtbrowse_def.moc.cpp \
  ta_qtviewer.moc.cpp \
  ta_qtbrowse.moc.cpp \
  ta_classbrowse.moc.cpp \
  ta_qtgroup.moc.cpp \
  ta_seledit.moc.cpp

ta_qt.moc.cpp: $(PDP4DIR)\src\ta\ta_qt.h
	$(MOC) %s -o $@

ta_matrix_qt.moc.cpp: $(PDP4DIR)\src\ta\ta_matrix_qt.h
	$(MOC) %s -o $@

ta_qtdata_def.moc.cpp: $(PDP4DIR)\src\ta\ta_qtdata_def.h
	$(MOC) %s -o $@

ta_qtdata.moc.cpp: $(PDP4DIR)\src\ta\ta_qtdata.h
	$(MOC) %s -o $@

ta_qtdialog.moc.cpp: $(PDP4DIR)\src\ta\ta_qtdialog.h
	$(MOC) %s -o $@

ta_qttype_def.moc.cpp: $(PDP4DIR)\src\ta\ta_qttype_def.h
	$(MOC) %s -o $@

ta_qttype.moc.cpp: $(PDP4DIR)\src\ta\ta_qttype.h
	$(MOC) %s -o $@

ta_qtclipdata.moc.cpp: $(PDP4DIR)\src\ta\ta_qtclipdata.h
	$(MOC) %s -o $@

ta_qtbrowse_def.moc.cpp: $(PDP4DIR)\src\ta\ta_qtbrowse_def.h
	$(MOC) %s -o $@

ta_qtviewer.moc.cpp: $(PDP4DIR)\src\ta\ta_qtviewer.h
	$(MOC) %s -o $@

ta_qtbrowse.moc.cpp: $(PDP4DIR)\src\ta\ta_qtbrowse.h
	$(MOC) %s -o $@

ta_classbrowse.moc.cpp: $(PDP4DIR)\src\ta\ta_classbrowse.h
	$(MOC) %s -o $@

ta_qtgroup.moc.cpp: $(PDP4DIR)\src\ta\ta_qtgroup.h
	$(MOC) %s -o $@

ta_seledit.moc.cpp: $(PDP4DIR)\src\ta\ta_seledit.h
	$(MOC) %s -o $@


CSS_MOCS: \
	css_qttype.moc.cpp \
	css_qt.moc.cpp \
	css_qtdata.moc.cpp \
	css_qtdialog.moc.cpp

css_qttype.moc.cpp: $(PDP4DIR)\src\css\css_qttype.h
	$(MOC) %s -o $@

css_qt.moc.cpp: $(PDP4DIR)\src\css\css_qt.h
	$(MOC) %s -o $@

css_qtdata.moc.cpp: $(PDP4DIR)\src\css\css_qtdata.h
	$(MOC) %s -o $@

css_qtdialog.moc.cpp: $(PDP4DIR)\src\css\css_qtdialog.h
	$(MOC) %s -o $@

TAMISC_MAKETA_GUI =\
	$(PDP4DIR)/src/tamisc/colorbar_qt.h \
	$(PDP4DIR)/src/tamisc/../taiqtso/safeptr_so.h \
	$(PDP4DIR)/src/tamisc/t3node_so.h \
	$(PDP4DIR)/src/tamisc/t3viewer.h \
	$(PDP4DIR)/src/tamisc/colorbar_so.h \
	$(PDP4DIR)/src/tamisc/datatable_qtso.h \
	$(PDP4DIR)/src/tamisc/datagraph_so.h \
	$(PDP4DIR)/src/tamisc/datagraph_qtso.h

TAMISC_MAKETA =\
	$(PDP4DIR)/src/ta/ta_TA_type.h \
	tamisc_def.h \
	fontspec.h \
	tarandom.h \
	tdgeometry.h \
	aggregate.h \
	minmax.h \
	ta_data.h \
	colorscale.h \
	datatable.h \
	datagraph.h \
	fun_lookup.h \
	$(TAMISC_MAKETA_GUI) \
	tamisc_maketa_defs.h \
	tamisc_stdef.h

TAMISC_MOCS: \
	t3viewer.moc.cpp \
	colorbar_qt.moc.cpp \
	datatable_qtso.moc.cpp \
	datagraph_qtso.moc.cpp

t3viewer.moc.cpp: $(PDP4DIR)\src\tamisc\t3viewer.h
	$(MOC) %s -o $@

colorbar_qt.moc.cpp: $(PDP4DIR)\src\tamisc\colorbar_qt.h
	$(MOC) %s -o $@

datatable_qtso.moc.cpp: $(PDP4DIR)\src\tamisc\datatable_qtso.h
	$(MOC) %s -o $@

datagraph_qtso.moc.cpp: $(PDP4DIR)\src\tamisc\datagraph_qtso.h
	$(MOC) %s -o $@

PDP_MAKETA_GUI =\
	pdpview_qt.h \
	netstru_so.h \
	netstru_qtso.h \
	pdp_qtso.h \
	pdplog_qtso.h \
	v3_compat_qtso.h

PDP_MAKETA =\
	$(PDP4DIR)/src/ta/ta_TA_type.h \
	$(PDP4DIR)/src/tamisc/tamisc_TA_type.h \
	pdp_def.h \
	pdpdeclare.h \
	pdpbase.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	program.h \
	pdplog.h \
	pdpshell.h \
	v3_compat.h \
	$(PDP_MAKETA_GUI)

PDP_MOCS: \
	pdp_qtso.moc.cpp \
	netstru_qtso.moc.cpp \
	pdplog_qtso.moc.cpp \
	v3_compat_qtso.moc.cpp 

pdp_qtso.moc.cpp: $(PDP4DIR)\src\pdp\pdp_qtso.h
	$(MOC) %s -o $@

netstru_qtso.moc.cpp: $(PDP4DIR)\src\pdp\netstru_qtso.h
	$(MOC) %s -o $@

pdplog_qtso.moc.cpp: $(PDP4DIR)\src\pdp\pdplog_qtso.h
	$(MOC) %s -o $@

v3_compat_qtso.moc.cpp: $(PDP4DIR)\src\pdp\v3_compat_qtso.h
	$(MOC) %s -o $@

BP_MAKETA =\
	$(PDP4DIR)/src/ta/ta_TA_type.h \
	$(PDP4DIR)/src/tamisc/tamisc_TA_type.h \
	$(PDP4DIR)/src/pdp/pdp_TA_type.h \
	bp_def.h \
	bp.h \
	rbp.h \
	bp_v3_compat.h \

#MAKETA_FLAGS = /I "$(VCINSTALLDIR)\Vc7\include"
#TODO: need to enable DEBUG to be parameterized somehow
MAKETA_FLAGS = $(INCLUDE) /D DEBUG -win_dll

#note: also makes _TA_inst.h and .cpp
ta_TA_type.h: $(TA_MAKETA)
  $(CD) $(PDP4DIR)/src/ta
  $(MAKETA) -v1 $(MAKETA_FLAGS) ta $(TA_MAKETA) 
  
tamisc_TA_type.h: $(TAMISC_MAKETA)
  $(CD) $(PDP4DIR)/src/tamisc
  $(MAKETA) -v1 $(MAKETA_FLAGS) tamisc $(TAMISC_MAKETA) 
  
pdp_TA_type.h: $(PDP_MAKETA)
  $(CD) $(PDP4DIR)/src/pdp
  $(MAKETA) -v1 $(MAKETA_FLAGS) pdp $(PDP_MAKETA) 
  
bp_TA_type.h: $(BP_MAKETA)
  $(CD) $(PDP4DIR)/src/bp
  $(MAKETA) -v1 $(MAKETA_FLAGS) bp $(BP_MAKETA) 
  
leabra_TA_type.h: $(LEABRA_MAKETA)
  $(CD) $(PDP4DIR)/src/leabra
  $(MAKETA) -v1 $(MAKETA_FLAGS) leabra $(LEABRA_MAKETA) 
  
clean: 
  del *.moc.cpp
 
taiqtso_files: TAIQTSO_MOCS
 
ta_files: ta_TA_type.h $(TA_MAKETA) TA_MOCS
  
css_files: CSS_MOCS
 
tamisc_files: tamisc_TA_type.h $(TAMISC_MAKETA) TAMISC_MOCS

pdp_files: pdp_TA_type.h $(PDP_MAKETA) PDP_MOCS

bp_files: bp_TA_type.h $(BP_MAKETA)

maketa_files: \
  ta_TA_type.h 
 
moc_files: \
  TAIQTSO_MOCS \
  TA_MOCS
