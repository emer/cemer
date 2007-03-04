VCINSTALLDIR = C:\Program Files\Microsoft Visual Studio .NET 2003
INCLUDE = "-I$(PDP4DIR)\src\ta" "-I$(PDP4DIR)/src/ta/ios-g++-3.1" \
  "-I$(PDP4DIR)\src\taiqtso" "-I$(PDP4DIR)\src\css" \
  "-I$(PDP4DIR)\src\pdp" "-I$(PDP4DIR)\src\bp"  "-I$(PDP4DIR)\src\leabra" \
  "-I$(QTDIR)/include/Qt" "-I$(QTDIR)/include/Qt3Support" \
  "-I$(QTDIR)/include/QtCore" "-I$(QTDIR)/include/QtGui" \
  "-I$(QTDIR)/include/QtNetwork" "-I$(QTDIR)/include/QtOpenGL" \
  "-I$(QTDIR)/include/QtXml" "-I$(COINDIR)/include/Inventor" \
  "-I$(PDP4DIR)\build\msvc7" "-I$(PDP4DIR)\build\msvc7\include" \
  "-I$(VCDIR)\Vc7\include" 
CD = cd   
MOC = $(QTDIR)\bin\moc $(INCLUDE)
MAKETA = $(PDP4DIR)\build\msvc7\maketa\Debug\maketa.exe -v1
#MAKETA = $(PDP4DIR)\build\msvc7\maketa\Release\maketa.exe -v1


all: maketa_files moc_files

TAIQTSO_MOCS: \
  ilabel.moc.cpp \
  idimedit.moc.cpp \
  ibutton.moc.cpp \
  icheckbox.moc.cpp \
  icliptoolwidget.moc.cpp \
  icombobox.moc.cpp \
  ieditgrid.moc.cpp \
  ilabel.moc.cpp \
  iflowlayout.moc.cpp \
  ilineedit.moc.cpp \
  igeometry.moc.cpp \
  irenderarea.moc.cpp \
  iscrollarea.moc.cpp \
  ispinbox.moc.cpp \
  itreewidget.moc.cpp 

ibutton.moc.cpp: $(PDP4DIR)\src\taiqtso\ibutton.h
	$(MOC) %s -o $@

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

iscrollarea.moc.cpp: $(PDP4DIR)\src\taiqtso\iscrollarea.h
	$(MOC) %s -o $@

ispinbox.moc.cpp: $(PDP4DIR)\src\taiqtso\ispinbox.h
	$(MOC) %s -o $@

itreewidget.moc.cpp: $(PDP4DIR)\src\taiqtso\itreewidget.h
	$(MOC) %s -o $@

qconsole.moc.cpp: $(PDP4DIR)\src\taiqtso\qconsole.h
	$(MOC) %s -o $@

TAHEADS_TAMISC_GUI =\
	$(PDP4DIR)/src/ta/colorbar_qt.h \
	$(PDP4DIR)/src/taiqtso/safeptr_so.h \
	$(PDP4DIR)/src/ta/t3node_so.h \
	$(PDP4DIR)/src/ta/t3viewer.h \
	$(PDP4DIR)/src/ta/colorbar_so.h \
	$(PDP4DIR)/src/ta/ta_datatable_qtso.h \
	$(PDP4DIR)/src/ta/ta_datatable_so.h 

TAHEADS_GUI =\
	$(PDP4DIR)/src/ta/ta_qt.h \
	$(PDP4DIR)/src/ta/ta_qtdata_def.h \
	$(PDP4DIR)/src/ta/ta_qtdata.h \
	$(PDP4DIR)/src/ta/ta_qtdialog.h \
	$(PDP4DIR)/src/ta/ta_qttype_def.h \
	$(PDP4DIR)/src/ta/ta_qttype.h \
	$(PDP4DIR)/src/ta/ta_qtclipdata.h \
	$(PDP4DIR)/src/ta/ta_qtviewer.h \
	$(PDP4DIR)/src/ta/ta_matrix_qt.h \
	$(PDP4DIR)/src/ta/ta_qtclassbrowse.h \
	$(PDP4DIR)/src/ta/ta_qtgroup.h \
	$(PDP4DIR)/src/ta/ta_seledit.h \
	$(PDP4DIR)/src/ta/ta_program_qt.h

TAHEADS_TAMISC =\
	$(PDP4DIR)/src/ta/ta_time.h \
	$(PDP4DIR)/src/ta/fun_lookup.h \
	$(PDP4DIR)/src/ta/ta_data_extras.h 

TAHEADS =\
	$(PDP4DIR)/src/ta/ios-g++-3.1/streambuf.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/iostream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/fstream.h \
	$(PDP4DIR)/src/ta/ios-g++-3.1/sstream.h \
	$(PDP4DIR)/src/ta/ta_global.h \
	$(PDP4DIR)/src/taiqtso/igeometry.h \
	$(PDP4DIR)/src/ta/ta_def.h \
	$(PDP4DIR)/src/ta/ta_platform.h \
	$(PDP4DIR)/src/ta/ta_thread.h \
	$(PDP4DIR)/src/ta/ta_string.h \
	$(PDP4DIR)/src/ta/ta_stdef.h \
	$(PDP4DIR)/src/ta/ta_variant.h \
	$(PDP4DIR)/src/ta/ifont.h \
	$(PDP4DIR)/src/ta/ta_mtrnd.h \
	$(PDP4DIR)/src/ta/ta_list.h \
	$(PDP4DIR)/src/ta/ta_filer.h \
	$(PDP4DIR)/src/ta/icolor.h \
	$(PDP4DIR)/src/ta/ta_type.h \
	$(PDP4DIR)/src/ta/ta_base.h \
	$(PDP4DIR)/src/ta/ta_fontspec.h \
	$(PDP4DIR)/src/ta/minmax.h \
	$(PDP4DIR)/src/ta/ta_group.h \
	$(PDP4DIR)/src/ta/ta_qtclipdata.h \
	$(PDP4DIR)/src/ta/colorscale.h \
	$(PDP4DIR)/src/ta/ta_dmem.h \
	$(PDP4DIR)/src/ta/ta_script.h \
	$(PDP4DIR)/src/ta/ta_dynenum.h \
	$(PDP4DIR)/src/ta/ta_viewspec.h \
	$(PDP4DIR)/src/ta/ta_viewer.h \
	$(PDP4DIR)/src/ta/ta_program.h \
	$(PDP4DIR)/src/ta/ta_matrix.h \
	$(PDP4DIR)/src/ta/ta_math.h \
	$(PDP4DIR)/src/ta/ta_geometry.h \
	$(PDP4DIR)/src/ta/ta_data.h \
	$(PDP4DIR)/src/ta/ta_datatable.h \
	$(PDP4DIR)/src/ta/ta_dataproc.h \
	$(PDP4DIR)/src/ta/ta_dataanal.h \
	$(PDP4DIR)/src/ta/ta_datagen.h \
	$(PDP4DIR)/src/ta/ta_imgproc.h \
	$(PDP4DIR)/src/ta/ta_dump.h \
	$(PDP4DIR)/src/ta/ta_defaults.h \
	$(PDP4DIR)/src/ta/ta_plugin.h \
	$(PDP4DIR)/src/ta/ta_plugin_p.h \
	$(PDP4DIR)/src/ta/ta_project.h \
	$(TAHEADS_GUI) \
	$(TAHEADS_TAMISC) \
	$(TAHEADS_TAMISC_GUI)

TA_MOCS: \
  colorbar_qt.moc.cpp \
  t3viewer.moc.cpp \
  ta_datatable.moc.cpp \
  ta_datatable_qtso.moc.cpp \
  ta_matrix.moc.cpp \
  ta_matrix_qt.moc.cpp \
  ta_plugin_p.moc.cpp \
  ta_program_qt.moc.cpp \
  ta_qt.moc.cpp \
  ta_qtclassbrowse.moc.cpp \
  ta_qtclipdata.moc.cpp \
  ta_qtdata_def.moc.cpp \
  ta_qtdata.moc.cpp \
  ta_qtdialog.moc.cpp \
  ta_qtgroup.moc.cpp \
  ta_qttype_def.moc.cpp \
  ta_qttype.moc.cpp \
  ta_qtviewer.moc.cpp \
  ta_qtgroup.moc.cpp \
  ta_type.moc.cpp \
  ta_project.moc.cpp

colorbar_qt.moc.cpp: $(PDP4DIR)\src\ta\colorbar_qt.h
	$(MOC) %s -o $@

t3viewer.moc.cpp: $(PDP4DIR)\src\ta\t3viewer.h
	$(MOC) %s -o $@
	
ta_datatable.moc.cpp: $(PDP4DIR)\src\ta\ta_datatable.h
	$(MOC) %s -o $@

ta_datatable_qtso.moc.cpp: $(PDP4DIR)\src\ta\ta_datatable_qtso.h
	$(MOC) %s -o $@

ta_matrix.moc.cpp: $(PDP4DIR)\src\ta\ta_matrix.h
	$(MOC) %s -o $@

ta_matrix_qt.moc.cpp: $(PDP4DIR)\src\ta\ta_matrix_qt.h
	$(MOC) %s -o $@

ta_plugin_p.moc.cpp: $(PDP4DIR)\src\ta\ta_plugin_p.h
	$(MOC) %s -o $@

ta_program_qt.moc.cpp: $(PDP4DIR)\src\ta\ta_program_qt.h
	$(MOC) %s -o $@

ta_project.moc.cpp: $(PDP4DIR)\src\ta\ta_project.h
	$(MOC) %s -o $@

ta_qt.moc.cpp: $(PDP4DIR)\src\ta\ta_qt.h
	$(MOC) %s -o $@

ta_qtclassbrowse.moc.cpp: $(PDP4DIR)\src\ta\ta_qtclassbrowse.h
	$(MOC) %s -o $@

ta_qtclipdata.moc.cpp: $(PDP4DIR)\src\ta\ta_qtclipdata.h
	$(MOC) %s -o $@

ta_qtdata.moc.cpp: $(PDP4DIR)\src\ta\ta_qtdata.h
	$(MOC) %s -o $@

ta_qtdata_def.moc.cpp: $(PDP4DIR)\src\ta\ta_qtdata_def.h
	$(MOC) %s -o $@

ta_qtdialog.moc.cpp: $(PDP4DIR)\src\ta\ta_qtdialog.h
	$(MOC) %s -o $@

ta_qtgroup.moc.cpp: $(PDP4DIR)\src\ta\ta_qtgroup.h
	$(MOC) %s -o $@

ta_qttype_def.moc.cpp: $(PDP4DIR)\src\ta\ta_qttype_def.h
	$(MOC) %s -o $@

ta_qttype.moc.cpp: $(PDP4DIR)\src\ta\ta_qttype.h
	$(MOC) %s -o $@

ta_qtviewer.moc.cpp: $(PDP4DIR)\src\ta\ta_qtviewer.h
	$(MOC) %s -o $@

ta_type.moc.cpp: $(PDP4DIR)\src\ta\ta_type.h
	$(MOC) %s -o $@





CSS_MOCS: \
	css_console.moc.cpp \
	css_machine.moc.cpp \
	css_qttype.moc.cpp \
	css_qt.moc.cpp \
	css_qtdata.moc.cpp \
	css_qtdialog.moc.cpp

css_console.moc.cpp: $(PDP4DIR)\src\css\css_console.h
	$(MOC) %s -o $@

css_machine.moc.cpp: $(PDP4DIR)\src\css\css_machine.h
	$(MOC) %s -o $@

css_qttype.moc.cpp: $(PDP4DIR)\src\css\css_qttype.h
	$(MOC) %s -o $@

css_qt.moc.cpp: $(PDP4DIR)\src\css\css_qt.h
	$(MOC) %s -o $@

css_qtdata.moc.cpp: $(PDP4DIR)\src\css\css_qtdata.h
	$(MOC) %s -o $@

css_qtdialog.moc.cpp: $(PDP4DIR)\src\css\css_qtdialog.h
	$(MOC) %s -o $@


PDP_HEADS_GUI =\
	$(PDP4DIR)/src/pdp/pdpview_qt.h \
	$(PDP4DIR)/src/pdp/pdpserver_qt.h \
	$(PDP4DIR)/src/pdp/netstru_so.h \
	$(PDP4DIR)/src/pdp/netstru_qtso.h \
	$(PDP4DIR)/src/pdp/pdp_qtso.h \
	$(PDP4DIR)/src/pdp/pdp_program_qtso.h 

PDP_HEADS =\
	$(PDP4DIR)/src/pdp/pdpdeclare.h \
	$(PDP4DIR)/src/pdp/pdp_base.h \
	$(PDP4DIR)/src/pdp/spec.h \
	$(PDP4DIR)/src/pdp/netstru.h \
	$(PDP4DIR)/src/pdp/netstru_extra.h \
	$(PDP4DIR)/src/pdp/netdata.h \
	$(PDP4DIR)/src/pdp/pdp_program.h \
	$(PDP4DIR)/src/pdp/pdp_project.h \
	$(PDP4DIR)/src/pdp/pdpserver.h \
	$(PDP_HEADS_GUI) \
	$(PDP4DIR)/src/pdp/v3_compat.h

PDP_MAKETA =\
	$(PDP4DIR)/src/ta/ta_TA_type.h \
	$(PDP_HEADS)


PDP_MOCS: \
	netstru_qtso.moc.cpp \
	pdp_program_qtso.moc.cpp

netstru_qtso.moc.cpp: $(PDP4DIR)\src\pdp\netstru_qtso.h
	$(MOC) %s -o $@
	
pdp_program_qtso.moc.cpp: $(PDP4DIR)\src\pdp\pdp_program_qtso.h
	$(MOC) %s -o $@

pdp_qtso.moc.cpp: $(PDP4DIR)\src\pdp\pdp_qtso.h
	$(MOC) %s -o $@

pdp_server_qt.moc.cpp: $(PDP4DIR)\src\pdp\pdp_server_qt.h
	$(MOC) %s -o $@



BP_HEADS =\
	$(PDP4DIR)/src/bp/bp.h \
	$(PDP4DIR)/src/bp/rbp.h \
	$(PDP4DIR)/src/bp/bp_v3_compat.h

BP_MAKETA =\
	$(PDP4DIR)/src/ta/ta_TA_type.h \
	$(PDP4DIR)/src/pdp/pdp_TA_type.h \
	$(BP_HEADS)

LEABRA_HEADS =\
	$(PDP4DIR)/src/leabra/leabra.h \
	$(PDP4DIR)/src/leabra/leabra_extra.h \
	$(PDP4DIR)/src/leabra/leabra_td.h \
	$(PDP4DIR)/src/leabra/leabra_pvlv.h \
	$(PDP4DIR)/src/leabra/leabra_pbwm.h \
	$(PDP4DIR)/src/leabra/leabra_v3_compat.h

LEABRA_MAKETA =\
	$(PDP4DIR)/src/ta/ta_TA_type.h \
	$(PDP4DIR)/src/pdp/pdp_TA_type.h \
	$(LEABRA_HEADS)

#MAKETA_FLAGS = /I "$(VCINSTALLDIR)\Vc7\include"
#TODO: need to enable DEBUG to be parameterized somehow
#MAKETA_FLAGS = -css -autohx $(INCLUDE) /D DEBUG -win_dll
MAKETA_FLAGS = -css $(INCLUDE) /D DEBUG -win_dll -w

#note: also makes _TA_inst.h and .cpp
ta_TA_type.h: $(TAHEADS) $(CSS_HEADS) $(TAIQTSO_HEADS)
  $(CD) $(PDP4DIR)/src/ta
  $(MAKETA) $(MAKETA_FLAGS) ta $(TAHEADS) 
  
pdp_TA_type.h: $(PDP_HEADS) $(BP_HEADS) $(LEABRA_HEADS)
  $(CD) $(PDP4DIR)/src/pdp
  $(MAKETA) $(MAKETA_FLAGS) pdp $(PDP_MAKETA) $(BP_HEADS)  $(LEABRA_HEADS)
  
#NOTE: following is defunct -- we put everything in pdp_TA
bp_TA_type.h: $(BP_HEADS)
  $(CD) $(PDP4DIR)/src/bp
  $(MAKETA) $(MAKETA_FLAGS) bp $(BP_MAKETA) 
  
#NOTE: following is defunct -- we put everything in pdp_TA
leabra_TA_type.h: $(LEABRA_HEADS)
  $(CD) $(PDP4DIR)/src/leabra
  $(MAKETA) $(MAKETA_FLAGS) leabra $(LEABRA_MAKETA) 
  
clean: 
  del *.moc.cpp
 
# NOTE: 'ta' and 'pdp' are the two libraries

taiqtso_files: TAIQTSO_MOCS
 
css_files: CSS_MOCS

ta_mocs: $(TA_MOCS)

ta_files: ta_TA_type.h ta_mocs
  

pdp_mocs: $(PDP_MOCS)

pdp_files: pdp_TA_type.h pdp_mocs

#bp_files: bp_TA_type.h
bp_files: 

#leabra_files: leabra_TA_type.h
leabra_files: 


maketa_files: \
  ta_TA_type.h 
 
moc_files: \
  $(TAIQTSO_MOCS) \
  $(TA_MOCS)
