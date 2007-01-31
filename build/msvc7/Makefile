VCINSTALLDIR = C:\Program Files\Microsoft Visual Studio .NET 2003
INCLUDE = "-I$(PDP4DIR)\src\ta" "-I$(PDP4DIR)/src/ta/ios-g++-3.1" \
  "-I$(PDP4DIR)\src\taiqtso" "-I$(PDP4DIR)\src\css" \
  "-I$(PDP4DIR)\src\pdp" "-I$(PDP4DIR)\src\bp"  "-I$(PDP4DIR)\src\leabra" \
  "-I$(QTDIR)/include/Qt" "-I$(QTDIR)/include/Qt3Support" \
  "-I$(QTDIR)/include/QtCore" "-I$(QTDIR)/include/QtGui" \
  "-I$(QTDIR)/include/QtNetwork" "-I$(QTDIR)/include/QtOpenGL" \
  "-I$(QTDIR)/include/QtXml" "-I$(COINDIR)/include/Inventor" \
  "-I$(PDP4DIR)\build\msvc7" "-I$(VCDIR)\Vc7\include" 
CD = cd   
MOC = $(QTDIR)\bin\moc $(INCLUDE)
MAKETA = $(PDP4DIR)\build\msvc7\maketa\Debug\maketa.exe
#MAKETA = $(PDP4DIR)\build\msvc7\maketa\Release\maketa.exe


all: maketa_files moc_files

TAIQTSO_MOCS: \
  ilabel.moc.cpp \
  idimedit.moc.cpp \
  icheckbox.moc.cpp \
  icliptoolwidget.moc.cpp \
  icombobox.moc.cpp \
  ieditgrid.moc.cpp \
  interceptor.moc.cpp \
  qconsole.moc.cpp \
  ilabel.moc.cpp \
  iflowlayout.moc.cpp \
  ilineedit.moc.cpp \
  igeometry.moc.cpp \
  irenderarea.moc.cpp \
  ispinbox.moc.cpp \
  itreewidget.moc.cpp 

icheckbox.moc.cpp: $(PDP4DIR)\src\taiqtso\icheckbox.h
	$(MOC) %s -o $@

icombobox.moc.cpp: $(PDP4DIR)\src\taiqtso\icombobox.h
	$(MOC) %s -o $@

icliptoolwidget.moc.cpp: $(PDP4DIR)\src\taiqtso\icliptoolwidget.h
	$(MOC) %s -o $@

ieditgrid.moc.cpp: $(PDP4DIR)\src\taiqtso\ieditgrid.h
	$(MOC) %s -o $@

idimedit.moc.cpp: $(PDP4DIR)\src\taiqtso\idimedit.h
	$(MOC) %s -o $@

iflowlayout.moc.cpp: $(PDP4DIR)\src\taiqtso\iflowlayout.h
	$(MOC) %s -o $@

igeometry.moc.cpp: $(PDP4DIR)\src\taiqtso\igeometry.h
	$(MOC) %s -o $@

ilabel.moc.cpp: $(PDP4DIR)\src\taiqtso\ilabel.h
	$(MOC) %s -o $@

ilineedit.moc.cpp: $(PDP4DIR)\src\taiqtso\ilineedit.h
	$(MOC) %s -o $@

interceptor.moc.cpp: $(PDP4DIR)\src\taiqtso\interceptor.h
	$(MOC) %s -o $@

irenderarea.moc.cpp: $(PDP4DIR)\src\taiqtso\irenderarea.h
	$(MOC) %s -o $@

ispinbox.moc.cpp: $(PDP4DIR)\src\taiqtso\ispinbox.h
	$(MOC) %s -o $@

itreewidget.moc.cpp: $(PDP4DIR)\src\taiqtso\itreewidget.h
	$(MOC) %s -o $@

qconsole.moc.cpp: $(PDP4DIR)\src\taiqtso\qconsole.h
	$(MOC) %s -o $@

TAHEADS_TAMISC_GUI =\
	colorbar_qt.h \
	$(PDP4DIR)/src/taiqtso/safeptr_so.h \
	t3node_so.h \
	t3viewer.h \
	colorbar_so.h \
	ta_datatable_qtso.h \
	ta_datatable_so.h 

TAHEADS_GUI =\
	ta_qt.h \
	ta_qtdata_def.h \
	ta_qtdata.h \
	ta_qtdialog.h \
	ta_qttype_def.h \
	ta_qttype.h \
	ta_qtclipdata.h \
	ta_qtviewer.h \
	ta_matrix_qt.h \
	ta_qtclassbrowse.h \
	ta_qtgroup.h \
	ta_seledit.h \
	ta_program_qt.h \

TAHEADS_TAMISC =\
	ta_time.h \
	fun_lookup.h \
	ta_data_extras.h 

TAHEADS =\
	$(PDP4DIR)/src/taiqtso/igeometry.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/streambuf.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/iostream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/fstream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/sstream.h \
	$(PDP4DIR)/src/ta/ta_global.h \
	ta_def.h \
	ta_platform.h \
	ta_thread.h \
	ta_string.h \
	ta_stdef.h \
	ta_variant.h \
	ifont.h \
	ta_mtrnd.h \
	ta_list.h \
	ta_filer.h \
	icolor.h \
	ta_type.h \
	ta_base.h \
	ta_fontspec.h \
	minmax.h \
	ta_group.h \
	ta_qtclipdata.h \
	colorscale.h \
	ta_dmem.h \
	ta_script.h \
	ta_dynenum.h \
	ta_viewspec.h \
	ta_viewer.h \
	ta_program.h \
	ta_matrix.h \
	ta_math.h \
	ta_geometry.h \
	ta_data.h \
	ta_datatable.h \
	ta_dataproc.h \
	ta_dataanal.h \
	ta_datagen.h \
	ta_imgproc.h \
	ta_dump.h \
	ta_defaults.h \
	ta_plugin.h \
	ta_plugin_p.h \
	ta_project.h \
	$(TAHEADS_GUI) \
	$(TAHEADS_TAMISC) \
	$(TAHEADS_TAMISC_GUI)

TA_MOCS: \
  ta_qt.moc.cpp \
  ta_matrix_qt.moc.cpp \
  ta_qtdata_def.moc.cpp \
  ta_qtdata.moc.cpp \
  ta_qtdialog.moc.cpp \
  ta_qttype_def.moc.cpp \
  ta_qttype.moc.cpp \
  ta_qtclipdata.moc.cpp \
  ta_qtviewer.moc.cpp \
  ta_qtmodel.moc.cpp \
  ta_qtclassbrowse.moc.cpp \
  ta_qtgroup.moc.cpp \
  ta_seledit.moc.cpp \
  ta_program_qt.moc.cpp \
  t3viewer.moc.cpp \
  colorbar_qt.moc.cpp \
  datatable_qtso.moc.cpp

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

ta_qtviewer.moc.cpp: $(PDP4DIR)\src\ta\ta_qtviewer.h
	$(MOC) %s -o $@

ta_qtclassbrowse.moc.cpp: $(PDP4DIR)\src\ta\ta_qtclassbrowse.h
	$(MOC) %s -o $@

ta_qtgroup.moc.cpp: $(PDP4DIR)\src\ta\ta_qtgroup.h
	$(MOC) %s -o $@

ta_seledit.moc.cpp: $(PDP4DIR)\src\ta\ta_seledit.h
	$(MOC) %s -o $@

ta_program_qt.moc.cpp: $(PDP4DIR)\src\ta\ta_program_qt.h
	$(MOC) %s -o $@

t3viewer.moc.cpp: $(PDP4DIR)\src\ta\t3viewer.h
	$(MOC) %s -o $@

colorbar_qt.moc.cpp: $(PDP4DIR)\src\ta\colorbar_qt.h
	$(MOC) %s -o $@

ta_datatable_qtso.moc.cpp: $(PDP4DIR)\src\ta\ta_datatable_qtso.h
	$(MOC) %s -o $@

#datagraph_qtso.moc.cpp: $(PDP4DIR)\src\ta\datagraph_qtso.h
#	$(MOC) %s -o $@



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


PDP_MAKETA_GUI =\
	pdpview_qt.h \
	pdpserver_qt.h \
	netstru_so.h \
	netstru_qtso.h \
	pdp_qtso.h \
	pdp_program_qtso.h 


PDP_MAKETA =\
	$(PDP4DIR)\src\ta\ta_TA_type.h \
	pdpdeclare.h \
	pdp_base.h \
	spec.h \
	netstru.h \
	netstru_extra.h \
	netdata.h \
	pdp_program.h \
	pdp_project.h \
	pdpserver.h \
	$(PDP_MAKETA_GUI) \
	v3_compat.h


PDP_MOCS: \
	pdp_program_qt.moc.cpp \
	pdp_qtso.moc.cpp \
	pdp_server_qt.moc.cpp \
	netstru_qtso.moc.cpp \
	pdplog_qtso.moc.cpp \
	v3_compat_qtso.moc.cpp 

pdp_program_qt.moc.cpp: $(PDP4DIR)\src\pdp\pdp_program_qt.h
	$(MOC) %s -o $@

pdp_qtso.moc.cpp: $(PDP4DIR)\src\pdp\pdp_qtso.h
	$(MOC) %s -o $@

pdp_server_qt.moc.cpp: $(PDP4DIR)\src\pdp\pdp_server_qt.h
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
MAKETA_FLAGS = -css -autohx $(INCLUDE) /D DEBUG -win_dll

#note: also makes _TA_inst.h and .cpp
ta_TA_type.h: $(TAHEADS)
  $(CD) $(PDP4DIR)/src/ta
  $(MAKETA) -v1 $(MAKETA_FLAGS) ta $(TAHEADS) 
  
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
 
ta_files: ta_TA_type.h $(TAHEADS) TA_MOCS
  
css_files: CSS_MOCS
 
pdp_files: pdp_TA_type.h $(PDP_MAKETA) PDP_MOCS

bp_files: bp_TA_type.h $(BP_MAKETA)

maketa_files: \
  ta_TA_type.h 
 
moc_files: \
  TAIQTSO_MOCS \
  TA_MOCS
