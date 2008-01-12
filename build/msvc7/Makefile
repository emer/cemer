VCINSTALLDIR = C:\Program Files\Microsoft Visual Studio .NET 2003
INCLUDE = "-I$(EMERGENTDIR)\src\temt\ta" "-I$(EMERGENTDIR)\src\temt\ta\ios-g++-3.1" \
  "-I$(EMERGENTDIR)\src\temt\taiqtso" "-I$(EMERGENTDIR)\src\temt\css" \
  "-I$(EMERGENTDIR)\src\emergent\network" "-I$(EMERGENTDIR)\src\emergent\bp"  "-I$(EMERGENTDIR)\src\emergent\leabra" \
  "-I$(QTDIR)/include/Qt" "-I$(QTDIR)/include/Qt3Support" \
  "-I$(QTDIR)/include/QtCore" "-I$(QTDIR)/include/QtGui" \
  "-I$(QTDIR)/include/QtNetwork" "-I$(QTDIR)/include/QtOpenGL" \
  "-I$(QTDIR)/include/QtXml" "-I$(COINDIR)/include/Inventor" \
  "-I$(EMERGENTDIR)\build\msvc7" "-I$(EMERGENTDIR)\build\msvc7\include" \
  "-I$(VCDIR)\Vc7\include" "-I$(ODEDIR)\include"
CD = cd   
MOC = $(QTDIR)\bin\moc $(INCLUDE)
#MAKETA = $(EMERGENTDIR)\build\msvc7\maketa\Debug\maketa.exe -v1
MAKETA = $(EMERGENTDIR)\build\msvc7\bin\maketa.exe 


all: maketa_files moc_files

taiqtso_mocs: \
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
  itextedit.moc.cpp \
  itextbrowser.moc.cpp \
  itreewidget.moc.cpp 

ibutton.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\ibutton.h
	$(MOC) %s -o $@

icheckbox.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\icheckbox.h
	$(MOC) %s -o $@

icombobox.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\icombobox.h
	$(MOC) %s -o $@

icliptoolwidget.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\icliptoolwidget.h
	$(MOC) %s -o $@

ieditgrid.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\ieditgrid.h
	$(MOC) %s -o $@

idimedit.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\idimedit.h
	$(MOC) %s -o $@

iflowlayout.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\iflowlayout.h
	$(MOC) %s -o $@

igeometry.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\igeometry.h
	$(MOC) %s -o $@

ilabel.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\ilabel.h
	$(MOC) %s -o $@

ilineedit.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\ilineedit.h
	$(MOC) %s -o $@

interceptor.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\interceptor.h
	$(MOC) %s -o $@

irenderarea.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\irenderarea.h
	$(MOC) %s -o $@

iscrollarea.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\iscrollarea.h
	$(MOC) %s -o $@

ispinbox.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\ispinbox.h
	$(MOC) %s -o $@

itextbrowser.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\itextbrowser.h
	$(MOC) %s -o $@

itextedit.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\itextedit.h
	$(MOC) %s -o $@

itreewidget.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\itreewidget.h
	$(MOC) %s -o $@

qconsole.moc.cpp: $(EMERGENTDIR)\src\temt\taiqtso\qconsole.h
	$(MOC) %s -o $@

TA_HEADS_TAMISC_GUI =\
	$(EMERGENTDIR)/src/temt/ta/colorbar_qt.h \
	$(EMERGENTDIR)/src/temt/taiqtso/safeptr_so.h \
	$(EMERGENTDIR)/src/temt/ta/t3node_so.h \
	$(EMERGENTDIR)/src/temt/ta/SoCapsule.h \
	$(EMERGENTDIR)/src/temt/ta/t3viewer.h \
	$(EMERGENTDIR)/src/temt/ta/colorbar_so.h \
	$(EMERGENTDIR)/src/temt/ta/ta_datatable_qtso.h \
	$(EMERGENTDIR)/src/temt/ta/ta_datatable_so.h \
	$(EMERGENTDIR)/src/temt/ta/ta_virtenv_qtso.h \
	$(EMERGENTDIR)/src/temt/ta/ta_virtenv_so.h 

TA_HEADS_GUI =\
	$(EMERGENTDIR)/src/temt/ta/ta_qt.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtdata_def.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtdata.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtdialog.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qttype_def.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qttype.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtviewer.h \
	$(EMERGENTDIR)/src/temt/ta/ta_matrix_qt.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtclassbrowse.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtgroup.h \
	$(EMERGENTDIR)/src/temt/ta/ta_seledit_qt.h \
	$(EMERGENTDIR)/src/temt/ta/ta_program_qt.h

TA_HEADS_TAMISC =\
	$(EMERGENTDIR)/src/temt/ta/ta_time.h \
	$(EMERGENTDIR)/src/temt/ta/fun_lookup.h \
	$(EMERGENTDIR)/src/temt/ta/ta_data_extras.h \
	$(EMERGENTDIR)/src/temt/ta/ta_virtenv.h 

TA_HEADS =\
	$(EMERGENTDIR)/src/temt/ta/ios-g++-3.1/streambuf.h \
	$(EMERGENTDIR)/src/temt/ta/ios-g++-3.1/iostream.h \
	$(EMERGENTDIR)/src/temt/ta/ios-g++-3.1/fstream.h \
	$(EMERGENTDIR)/src/temt/ta/ios-g++-3.1/sstream.h \
	$(EMERGENTDIR)/build/msvc7/config.h \
	$(EMERGENTDIR)/src/temt/ta/ta_global.h \
	$(EMERGENTDIR)/src/temt/taiqtso/igeometry.h \
	$(EMERGENTDIR)/src/temt/ta/ta_def.h \
	$(EMERGENTDIR)/src/temt/ta/ta_platform.h \
	$(EMERGENTDIR)/src/temt/ta/ta_string.h \
	$(EMERGENTDIR)/src/temt/ta/ta_stdef.h \
	$(EMERGENTDIR)/src/temt/ta/ta_variant.h \
	$(EMERGENTDIR)/src/temt/ta/ifont.h \
	$(EMERGENTDIR)/src/temt/ta/ta_mtrnd.h \
	$(EMERGENTDIR)/src/temt/ta/ta_list.h \
	$(EMERGENTDIR)/src/temt/ta/ta_filer.h \
	$(EMERGENTDIR)/src/temt/ta/icolor.h \
	$(EMERGENTDIR)/src/temt/ta/ta_type.h \
	$(EMERGENTDIR)/src/temt/ta/ta_base.h \
	$(EMERGENTDIR)/src/temt/ta/ta_fontspec.h \
	$(EMERGENTDIR)/src/temt/ta/minmax.h \
	$(EMERGENTDIR)/src/temt/ta/ta_group.h \
	$(EMERGENTDIR)/src/temt/ta/ta_qtclipdata.h \
	$(EMERGENTDIR)/src/temt/ta/colorscale.h \
	$(EMERGENTDIR)/src/temt/ta/ta_dmem.h \
	$(EMERGENTDIR)/src/temt/ta/ta_script.h \
	$(EMERGENTDIR)/src/temt/ta/ta_viewspec.h \
	$(EMERGENTDIR)/src/temt/ta/ta_viewer.h \
	$(EMERGENTDIR)/src/temt/ta/ta_program.h \
	$(EMERGENTDIR)/src/temt/ta/ta_program_els.h \
	$(EMERGENTDIR)/src/temt/ta/ta_matrix.h \
	$(EMERGENTDIR)/src/temt/ta/ta_math.h \
	$(EMERGENTDIR)/src/temt/ta/ta_geometry.h \
	$(EMERGENTDIR)/src/temt/ta/ta_data.h \
	$(EMERGENTDIR)/src/temt/ta/ta_datatable.h \
	$(EMERGENTDIR)/src/temt/ta/ta_dataproc.h \
	$(EMERGENTDIR)/src/temt/ta/ta_dataanal.h \
	$(EMERGENTDIR)/src/temt/ta/ta_datagen.h \
	$(EMERGENTDIR)/src/temt/ta/ta_imgproc.h \
	$(EMERGENTDIR)/src/temt/ta/ta_dump.h \
	$(EMERGENTDIR)/src/temt/ta/ta_defaults.h \
	$(EMERGENTDIR)/src/temt/ta/ta_plugin.h \
	$(EMERGENTDIR)/src/temt/ta/ta_plugin_p.h \
	$(EMERGENTDIR)/src/temt/ta/ta_engine.h \
	$(EMERGENTDIR)/src/temt/ta/ta_seledit.h \
	$(EMERGENTDIR)/src/temt/ta/ta_project.h \
	$(EMERGENTDIR)/src/temt/ta/ta_server.h \
	$(TA_HEADS_GUI) \
	$(TA_HEADS_TAMISC) \
	$(TA_HEADS_TAMISC_GUI)

ta_mocs: \
  colorbar_qt.moc.cpp \
  t3viewer.moc.cpp \
  ta_datatable.moc.cpp \
  ta_datatable_qtso.moc.cpp \
  ta_matrix.moc.cpp \
  ta_matrix_qt.moc.cpp \
  ta_plugin_p.moc.cpp \
  ta_project.moc.cpp \
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
  ta_seledit_qt.moc.cpp \
  ta_server.moc.cpp \
  ta_type.moc.cpp \
  ta_virtenv_qtso.moc.cpp

colorbar_qt.moc.cpp: $(EMERGENTDIR)\src\temt\ta\colorbar_qt.h
	$(MOC) %s -o $@

t3viewer.moc.cpp: $(EMERGENTDIR)\src\temt\ta\t3viewer.h
	$(MOC) %s -o $@
	
ta_datatable.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_datatable.h
	$(MOC) %s -o $@

ta_datatable_qtso.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_datatable_qtso.h
	$(MOC) %s -o $@

ta_matrix.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_matrix.h
	$(MOC) %s -o $@

ta_matrix_qt.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_matrix_qt.h
	$(MOC) %s -o $@

ta_plugin_p.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_plugin_p.h
	$(MOC) %s -o $@

ta_program_qt.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_program_qt.h
	$(MOC) %s -o $@

ta_project.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_project.h
	$(MOC) %s -o $@

ta_qt.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qt.h
	$(MOC) %s -o $@

ta_qtclassbrowse.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtclassbrowse.h
	$(MOC) %s -o $@

ta_qtclipdata.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtclipdata.h
	$(MOC) %s -o $@

ta_qtdata.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtdata.h
	$(MOC) %s -o $@

ta_qtdata_def.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtdata_def.h
	$(MOC) %s -o $@

ta_qtdialog.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtdialog.h
	$(MOC) %s -o $@

ta_qtgroup.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtgroup.h
	$(MOC) %s -o $@

ta_qttype_def.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qttype_def.h
	$(MOC) %s -o $@

ta_qttype.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qttype.h
	$(MOC) %s -o $@

ta_qtviewer.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_qtviewer.h
	$(MOC) %s -o $@

ta_seledit_qt.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_seledit_qt.h
	$(MOC) %s -o $@

ta_server.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_server.h
	$(MOC) %s -o $@

ta_type.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_type.h
	$(MOC) %s -o $@

ta_virtenv_qtso.moc.cpp: $(EMERGENTDIR)\src\temt\ta\ta_virtenv_qtso.h
	$(MOC) %s -o $@




css_mocs: \
	css_console.moc.cpp \
	css_machine.moc.cpp \
	css_qttype.moc.cpp \
	css_qt.moc.cpp \
	css_qtdata.moc.cpp \
	css_qtdialog.moc.cpp

css_console.moc.cpp: $(EMERGENTDIR)\src\temt\css\css_console.h
	$(MOC) %s -o $@

css_machine.moc.cpp: $(EMERGENTDIR)\src\temt\css\css_machine.h
	$(MOC) %s -o $@

css_qttype.moc.cpp: $(EMERGENTDIR)\src\temt\css\css_qttype.h
	$(MOC) %s -o $@

css_qt.moc.cpp: $(EMERGENTDIR)\src\temt\css\css_qt.h
	$(MOC) %s -o $@

css_qtdata.moc.cpp: $(EMERGENTDIR)\src\temt\css\css_qtdata.h
	$(MOC) %s -o $@

css_qtdialog.moc.cpp: $(EMERGENTDIR)\src\temt\css\css_qtdialog.h
	$(MOC) %s -o $@


NETWORK_HEADS_GUI =\
	$(EMERGENTDIR)/src/emergent/network/netstru_so.h \
	$(EMERGENTDIR)/src/emergent/network/netstru_qtso.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_qtso.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_program_qtso.h 

NETWORK_HEADS =\
	$(EMERGENTDIR)/src/emergent/network/emergent_base.h \
	$(EMERGENTDIR)/src/emergent/network/spec.h \
	$(EMERGENTDIR)/src/emergent/network/netstru.h \
	$(EMERGENTDIR)/src/emergent/network/netstru_extra.h \
	$(EMERGENTDIR)/src/emergent/network/netdata.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_program.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_project.h \
	$(NETWORK_HEADS_GUI) \
	$(EMERGENTDIR)/src/emergent/network/v3_compat.h

NETWORK_MAKETA =\
	$(EMERGENTDIR)/src/temt/ta/ta_TA_type.h \
	$(NETWORK_HEADS)


network_mocs: \
	netstru_qtso.moc.cpp \
	emergent_program_qtso.moc.cpp \
	emergent_qtso.moc.cpp 

netstru_qtso.moc.cpp: $(EMERGENTDIR)\src\emergent\network\netstru_qtso.h
	$(MOC) %s -o $@
	
emergent_program_qtso.moc.cpp: $(EMERGENTDIR)\src\emergent\network\emergent_program_qtso.h
	$(MOC) %s -o $@

emergent_qtso.moc.cpp: $(EMERGENTDIR)\src\emergent\network\emergent_qtso.h
	$(MOC) %s -o $@


CS_HEADS =\
	$(EMERGENTDIR)/src/emergent/cs/cs.h \
	$(EMERGENTDIR)/src/emergent/cs/cs_v3_compat.h

CS_MAKETA =\
	$(EMERGENTDIR)/src/temt/ta/ta_TA_type.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_TA_type.h \
	$(CS_HEADS)

BP_HEADS =\
	$(EMERGENTDIR)/src/emergent/bp/bp.h \
	$(EMERGENTDIR)/src/emergent/bp/rbp.h \
	$(EMERGENTDIR)/src/emergent/bp/bp_v3_compat.h

BP_MAKETA =\
	$(EMERGENTDIR)/src/temt/ta/ta_TA_type.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_TA_type.h \
	$(BP_HEADS)

LEABRA_HEADS =\
	$(EMERGENTDIR)/src/emergent/leabra/leabra.h \
	$(EMERGENTDIR)/src/emergent/leabra/leabra_extra.h \
	$(EMERGENTDIR)/src/emergent/leabra/leabra_td.h \
	$(EMERGENTDIR)/src/emergent/leabra/leabra_pvlv.h \
	$(EMERGENTDIR)/src/emergent/leabra/leabra_pbwm.h \
	$(EMERGENTDIR)/src/emergent/leabra/leabra_ct.h \
	$(EMERGENTDIR)/src/emergent/leabra/leabra_v3_compat.h

LEABRA_MAKETA =\
	$(EMERGENTDIR)/src/temt/ta/ta_TA_type.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_TA_type.h \
	$(LEABRA_HEADS)

SO_HEADS =\
	$(EMERGENTDIR)/src/emergent/so/so.h \
	$(EMERGENTDIR)/src/emergent/so/som.h \
	$(EMERGENTDIR)/src/emergent/so/cl.h \
	$(EMERGENTDIR)/src/emergent/so/zsh.h \
	$(EMERGENTDIR)/src/emergent/so/so_v3_compat.h

SO_MAKETA =\
	$(EMERGENTDIR)/src/temt/ta/ta_TA_type.h \
	$(EMERGENTDIR)/src/emergent/network/emergent_TA_type.h \
	$(SO_HEADS)

#MAKETA_FLAGS = /I "$(VCINSTALLDIR)\Vc7\include"
#TODO: need to enable DEBUG to be parameterized somehow

!IF "$(TARGET)" == "Release"
MAKETA_FLAGS = -css -autohx $(INCLUDE) -win_dll
!ELSEIF "$(TARGET)" == "Debug"
MAKETA_FLAGS = -css -autohx /D DEBUG $(INCLUDE) -win_dll
!ELSE
! ERROR TARGET macro must be defined as one of: Debug, Release 
!ENDIF

#MAKETA_FLAGS = -css $(INCLUDE) -win_dll

#note: also makes _TA_inst.h and .cpp
ta_TA_type.hx: $(TA_HEADS) $(CSS_HEADS) $(TAIQTSO_HEADS)
  $(CD) $(EMERGENTDIR)/src/temt/ta
  $(MAKETA) $(MAKETA_FLAGS) ta -f files.mta 
  
ta_TA_type.h: ta_TA_type.hx

emergent_TA_type.hx: $(TA_HEADS) $(NETWORK_HEADS) 
  $(CD) $(EMERGENTDIR)/src/emergent/network
  $(MAKETA) $(MAKETA_FLAGS) emergent -f files.mta
  
emergent_TA_type.h: emergent_TA_type.hx 

bp_TA_type.hx: $(TA_HEADS) $(NETWORK_MAKETA) $(BP_HEADS)
  $(CD) $(EMERGENTDIR)/src/emergent/bp
  $(MAKETA) $(MAKETA_FLAGS) bp -f files.mta 
  
bp_TA_type.h: bp_TA_type.hx

cs_TA_type.hx: $(TA_HEADS) $(NETWORK_MAKETA) $(CS_HEADS)
  $(CD) $(EMERGENTDIR)/src/emergent/cs
  $(MAKETA) $(MAKETA_FLAGS) cs -f files.mta 
  
cs_TA_type.h: cs_TA_type.hx

leabra_TA_type.hx: $(TA_HEADS) $(NETWORK_MAKETA) $(LEABRA_HEADS)
  $(CD) $(EMERGENTDIR)/src/emergent/leabra
  $(MAKETA) $(MAKETA_FLAGS) leabra -f files.mta 

leabra_TA_type.h: leabra_TA_type.hx
  
so_TA_type.hx: $(TA_HEADS) $(NETWORK_MAKETA) $(SO_HEADS)
  $(CD) $(EMERGENTDIR)/src/emergent/so
  $(MAKETA) $(MAKETA_FLAGS) so -f files.mta 
  
so_TA_type.h: so_TA_type.hx

clean: 
  del *.moc.cpp
 
# NOTE: 'ta' and 'pdp' are the two libraries

taiqtso_files: taiqtso_mocs

ta_files: ta_TA_type.h ta_mocs
  
css_files: css_mocs


network_files: emergent_TA_type.h network_mocs

bp_files: bp_TA_type.h

cs_files: cs_TA_type.h

leabra_files: leabra_TA_type.h

so_files: so_TA_type.h
