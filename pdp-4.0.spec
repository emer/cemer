Summary: The PDP++ software is a neural-network simulation system written in C++.
Name: pdp
Version: 4.0
Release: 0
License: GPL
Group: Productivity/Networking
Source0: pdp-4.0.tar.gz
URL: http://psych.colorado.edu/~oreilly/PDP++/PDP++.html
BuildRoot: %{_tmppath}/%{name}-root
Prefix: %{_prefix}

%description
The PDP++ software is a neural-network simulation system written in C++. It represents the next generation of the PDP software originally released with the McClelland and Rumelhart "Explorations in Parallel Distributed Processing Handbook", MIT Press, 1987. It is easy enough for novice users, but very powerful and flexible for research use.

%prep
%setup

%build
%configure
make

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/maketa
%{_bindir}/css++
%{_bindir}/pdp++
%{_bindir}/leabra++
%{_bindir}/bp++
%{_libdir}/libtaiqtso.la
%{_libdir}/libtaiqtso.a
%{_libdir}/libta.la
%{_libdir}/libta.a
%{_libdir}/libcss.la
%{_libdir}/libcss.a
%{_libdir}/libtamisc.la
%{_libdir}/libtamisc.a
%{_libdir}/libpdp.la
%{_libdir}/libpdp.a
%{_datadir}/pdp/AUTHORS
%{_datadir}/pdp/ChangeLog
%{_datadir}/pdp/COPYING
%{_datadir}/pdp/COPYING.LIB
%{_datadir}/pdp/INSTALL
%{_datadir}/pdp/NEWS
%{_datadir}/pdp/README

%doc AUTHORS ChangeLog COPYING COPYING.LIB INSTALL NEWS README

%changelog
* Fri Nov 04 2005 23:39 mingus

- pdp4.0/trunk/Makefile.am, pdp4.0/trunk/configure.ac: isolating
	  maketa

* Fri Nov 04 2005 23:33 mingus

- pdp4.0/trunk/src/maketa/Makefile.am,
	  pdp4.0/trunk/src/maketa/configure.ac,
	  pdp4.0/trunk/src/maketa/maketa.cpp,
	  pdp4.0/trunk/src/maketa/maketa.h,
	  pdp4.0/trunk/src/maketa/mta_constr.cpp,
	  pdp4.0/trunk/src/maketa/ta_base.h,
	  pdp4.0/trunk/src/maketa/ta_constr.h,
	  pdp4.0/trunk/src/maketa/ta_def.h,
	  pdp4.0/trunk/src/maketa/ta_dump.h,
	  pdp4.0/trunk/src/maketa/ta_filer.h,
	  pdp4.0/trunk/src/maketa/ta_group.h,
	  pdp4.0/trunk/src/maketa/ta_list.cpp,
	  pdp4.0/trunk/src/maketa/ta_list.h,
	  pdp4.0/trunk/src/maketa/ta_matrix.h,
	  pdp4.0/trunk/src/maketa/ta_parse.h,
	  pdp4.0/trunk/src/maketa/ta_platform.cpp,
	  pdp4.0/trunk/src/maketa/ta_platform.h,
	  pdp4.0/trunk/src/maketa/ta_platform_unix.cpp,
	  pdp4.0/trunk/src/maketa/ta_script.h,
	  pdp4.0/trunk/src/maketa/ta_stdef.h,
	  pdp4.0/trunk/src/maketa/ta_string.cpp,
	  pdp4.0/trunk/src/maketa/ta_string.h,
	  pdp4.0/trunk/src/maketa/ta_ti.cpp,
	  pdp4.0/trunk/src/maketa/ta_type.cpp,
	  pdp4.0/trunk/src/maketa/ta_type.h,
	  pdp4.0/trunk/src/maketa/taglobal.h: isolating maketa

* Fri Nov 04 2005 22:52 baisa

- pdp4.0/trunk/src/maketa/maketa_ti.cpp,
	  pdp4.0/trunk/src/maketa/test_maketa.h,
	  pdp4.0/trunk/src/pdp/netstru_qtso.cpp,
	  pdp4.0/trunk/src/pdp/netstru_qtso.h,
	  pdp4.0/trunk/src/pdp/pdp_ti.cpp,
	  pdp4.0/trunk/src/ta/ta_list.cpp, pdp4.0/trunk/src/ta/ta_list.h:
	  array changes

* Thu Nov 03 2005 23:08 mingus

- pdp4.0/trunk/configure.ac:

* Sat Oct 29 2005 00:15 mingus

- pdp4.0/trunk/src/ta/ta_TA.cpp:

* Sat Oct 29 2005 00:14 mingus

- pdp4.0/trunk/configure.ac, pdp4.0/trunk/pdp-4.0.spec,
	  pdp4.0/trunk/src/ta/ta_TA.cpp: spec for an rpm file. can't
	  finish this until gui building is fixed

* Fri Oct 28 2005 23:26 mingus

- pdp4.0/trunk/src/pdp/sched_proc.cpp: In --disable-gui
	  --enable-mpi mode, src/pdp/sched_proc.cpp:644 had an #ifdef
	  DMEM_COMPILE that relied on cssiSession which is declared in
	  css_qt.h which itself is only included #ifdef TA_GUI. This fix
	  wraps it in an #ifdef TA_GUI conditional.

* Fri Oct 28 2005 22:59 mingus

- pdp4.0/trunk/Makefile.am, pdp4.0/trunk/Maketa.am,
	  pdp4.0/trunk/Moc.am, pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/maketa/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am: New rules for maketa do not
	  result in a race condition when doing a parallel build.

* Fri Oct 28 2005 02:22 mingus

- pdp4.0/trunk/src/taiqtso/irenderarea.h,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.h: had to also #include
	  config.h in these files because they check for the definition of
	  TA_USE_INVENTOR /before/ they include taiqtso_def.h, which was
	  their only access to taglobal.h, which includes config.h and
	  holds the definition of TA_USE_INVENTOR

* Fri Oct 28 2005 02:19 mingus

- pdp4.0/trunk/src/ta/taglobal.h: was even able to remove
	  -DHAVE_CONFIG_H from command line

* Fri Oct 28 2005 02:14 mingus

- pdp4.0/trunk/src/css/Makefile.am:

* Fri Oct 28 2005 02:07 mingus

- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/maketa/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am:

* Fri Oct 28 2005 02:06 mingus

- pdp4.0/trunk/Maketa.am:

* Fri Oct 28 2005 02:05 mingus

- pdp4.0/trunk/Makefile.am:
- pdp4.0/trunk/custom.h:
- pdp4.0/trunk/configure.ac:
- pdp4.0/trunk/acsite.m4: more define elimination coming

* Thu Oct 27 2005 16:23 mingus

- pdp4.0/trunk/src/ta/taglobal.h: README: config.h is generated
	  dynamically, so to use this version of taglobal with the old
	  makefiles you need to run autoreconf and configure. configure
	  will run config.status which will generate config.h. then the
	  old makefiles will work. make sure you configure for e.g.,
	  mpi/debug/nogui etc..

* Thu Oct 27 2005 16:20 mingus

- pdp4.0/trunk/src/ta/ta_matrix.h: including <assert.h> lets us
	  build on gcc 3.x

* Thu Oct 27 2005 16:18 mingus

- pdp4.0/trunk/src/maketa/Makefile.am:

* Thu Oct 27 2005 16:17 mingus

- pdp4.0/trunk/acsite.m4:

* Thu Oct 27 2005 16:16 mingus

- pdp4.0/trunk/custom.h:
- pdp4.0/trunk/configure.ac: next few commits related to removing
	  command-line defines

* Thu Oct 27 2005 15:53 mingus

- pdp4.0/trunk/src/ta/ta_type.cpp: erronous space

* Thu Oct 13 2005 21:10 baisa

- pdp4.0/trunk/src/ta/ta_TA.cpp, pdp4.0/trunk/src/ta/ta_base.cpp,
	  pdp4.0/trunk/src/ta/ta_base.h, pdp4.0/trunk/src/ta/ta_list.cpp,
	  pdp4.0/trunk/src/ta/ta_list.h,
	  pdp4.0/trunk/src/ta/ta_matrix.cpp,
	  pdp4.0/trunk/src/ta/ta_matrix.h, pdp4.0/trunk/src/ta/ta_ti.cpp,
	  pdp4.0/trunk/src/ta/ta_type.cpp, pdp4.0/trunk/src/ta/ta_type.h:
	  working array changes working maketa

* Wed Oct 12 2005 17:11 baisa

- pdp4.0/trunk/src/ta/Makefile.am: changes for class browsing

* Wed Oct 12 2005 17:08 baisa

- pdp4.0/trunk/src/ta/ta_TA.cpp,
	  pdp4.0/trunk/src/ta/ta_classbrowse.cpp,
	  pdp4.0/trunk/src/ta/ta_classbrowse.h,
	  pdp4.0/trunk/src/ta/ta_list.cpp, pdp4.0/trunk/src/ta/ta_list.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.cpp,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.h,
	  pdp4.0/trunk/src/ta/ta_qtdialog.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdialog.h,
	  pdp4.0/trunk/src/ta/ta_qtviewer.cpp,
	  pdp4.0/trunk/src/ta/ta_qtviewer.h,
	  pdp4.0/trunk/src/ta/ta_type.cpp, pdp4.0/trunk/src/ta/ta_type.h:
	  changes for class browsing

* Sat Oct 08 2005 22:14 mingus

- pdp4.0/trunk/configure.ac: minor

* Sat Oct 08 2005 22:07 mingus

- pdp4.0/trunk/src/maketa/updatesrcs.cmd: this one actually
	  works..=)

* Sat Oct 08 2005 20:14 mingus

- pdp4.0/trunk/src/ta/ta_platform_win.cpp: changing include
	  directive to relative

* Wed Oct 05 2005 23:22 mingus

- pdp4.0/trunk/src/bp/bp.vcproj, pdp4.0/trunk/src/css/css.vcproj,
	  pdp4.0/trunk/src/maketa/maketa.vcproj,
	  pdp4.0/trunk/src/pdp++.sln, pdp4.0/trunk/src/pdp++.suo,
	  pdp4.0/trunk/src/ta/ta.vcproj,
	  pdp4.0/trunk/src/taiqtso/taiqtso.vcproj: removing old visual c++
	  project files

* Mon Oct 03 2005 19:17 mingus

- pdp4.0/trunk/configure.ac: misc changes

* Mon Oct 03 2005 19:15 mingus

- pdp4.0/trunk/src/bp/bp_TA.cpp, pdp4.0/trunk/src/bp/bp_TA_inst.h,
	  pdp4.0/trunk/src/bp/bp_TA_type.h,
	  pdp4.0/trunk/src/leabra/leabra_TA.cpp,
	  pdp4.0/trunk/src/leabra/leabra_TA_inst.h,
	  pdp4.0/trunk/src/leabra/leabra_TA_type.h,
	  pdp4.0/trunk/src/maketa/maketa_TA_inst.h,
	  pdp4.0/trunk/src/maketa/maketa_TA_type.h,
	  pdp4.0/trunk/src/maketa/test_TA.cpp,
	  pdp4.0/trunk/src/maketa/test_TA_inst.h,
	  pdp4.0/trunk/src/maketa/test_TA_type.h,
	  pdp4.0/trunk/src/pdp/pdp_TA.cpp,
	  pdp4.0/trunk/src/pdp/pdp_TA_inst.h,
	  pdp4.0/trunk/src/pdp/pdp_TA_type.h,
	  pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_inst.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_type.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_inst.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_type.h: removing TA files

* Sat Oct 01 2005 04:21 mingus

- pdp4.0/trunk/Makefile.am, pdp4.0/trunk/configure.ac:

* Sat Oct 01 2005 04:20 mingus

- pdp4.0/trunk/aux, pdp4.0/trunk/aux/config.guess,
	  pdp4.0/trunk/aux/config.sub, pdp4.0/trunk/aux/depcomp,
	  pdp4.0/trunk/aux/install-sh, pdp4.0/trunk/aux/ltmain.sh,
	  pdp4.0/trunk/aux/missing: finished move

* Sat Oct 01 2005 04:19 mingus

- pdp4.0/trunk/config.guess, pdp4.0/trunk/config.sub,
	  pdp4.0/trunk/depcomp, pdp4.0/trunk/install-sh,
	  pdp4.0/trunk/ltmain.sh, pdp4.0/trunk/missing: moving these into
	  /aux/

* Sat Oct 01 2005 03:54 mingus

- pdp4.0/trunk/src/pdp/pdpshell.cpp,
	  pdp4.0/trunk/src/pdp/sched_proc.cpp: minor changes. looks like
	  these were overlooked

* Sat Oct 01 2005 03:48 mingus

- pdp4.0/trunk/acsite.m4, pdp4.0/trunk/configure.ac,
	  pdp4.0/trunk/src/pdp/Makefile.am:

* Sat Oct 01 2005 02:22 mingus

- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/maketa/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am:
- pdp4.0/trunk/acsite.m4, pdp4.0/trunk/configure.ac:

* Thu Sep 29 2005 22:08 mingus

- pdp4.0/trunk/acsite.m4: ACX_PTHREAD

* Thu Sep 29 2005 19:28 baisa

- pdp4.0/trunk/src/bp/bp_TA.cpp,
	  pdp4.0/trunk/src/leabra/leabra_TA.cpp,
	  pdp4.0/trunk/src/maketa/maketa_ti.cpp,
	  pdp4.0/trunk/src/pdp/pdp_TA.cpp, pdp4.0/trunk/src/ta/ta_TA.cpp,
	  pdp4.0/trunk/src/ta/ta_base.h, pdp4.0/trunk/src/ta/ta_list.cpp,
	  pdp4.0/trunk/src/ta/ta_list.h, pdp4.0/trunk/src/ta/ta_type.h,
	  pdp4.0/trunk/src/ta/taglobal.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA.cpp,
	  pdp4.0/trunk/src/test/test_TA.cpp,
	  pdp4.0/trunk/src/test/test_TA_type.h: misc changes

* Wed Sep 28 2005 23:01 mingus

- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am: cleanup

* Wed Sep 28 2005 22:46 mingus

- pdp4.0/trunk/src/css/css_parse.y,
	  pdp4.0/trunk/src/maketa/mta_parse.y,
	  pdp4.0/trunk/src/pdp/enviro_qtso.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdialog.cpp,
	  pdp4.0/trunk/src/ta/ta_qttype.cpp: misc copyright

* Wed Sep 28 2005 22:34 mingus

- pdp4.0/trunk/src/tamisc/aggregate.cpp,
	  pdp4.0/trunk/src/tamisc/aggregate.h,
	  pdp4.0/trunk/src/tamisc/axis.cpp,
	  pdp4.0/trunk/src/tamisc/axis.h,
	  pdp4.0/trunk/src/tamisc/colorbar_qt.cpp,
	  pdp4.0/trunk/src/tamisc/colorbar_qt.h,
	  pdp4.0/trunk/src/tamisc/colorbar_so.cpp,
	  pdp4.0/trunk/src/tamisc/colorbar_so.h,
	  pdp4.0/trunk/src/tamisc/colorscale.cpp,
	  pdp4.0/trunk/src/tamisc/colorscale.h,
	  pdp4.0/trunk/src/tamisc/datagraph.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph.h,
	  pdp4.0/trunk/src/tamisc/datagraph_qtso.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph_qtso.h,
	  pdp4.0/trunk/src/tamisc/datagraph_so.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph_so.h,
	  pdp4.0/trunk/src/tamisc/datatable.cpp,
	  pdp4.0/trunk/src/tamisc/datatable.h,
	  pdp4.0/trunk/src/tamisc/datatable_qtso.cpp,
	  pdp4.0/trunk/src/tamisc/datatable_qtso.h,
	  pdp4.0/trunk/src/tamisc/fontspec.cpp,
	  pdp4.0/trunk/src/tamisc/fontspec.h,
	  pdp4.0/trunk/src/tamisc/fun_lookup.cpp,
	  pdp4.0/trunk/src/tamisc/fun_lookup.h,
	  pdp4.0/trunk/src/tamisc/graphic.cpp,
	  pdp4.0/trunk/src/tamisc/graphic.h,
	  pdp4.0/trunk/src/tamisc/graphic_objs.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_objs.h,
	  pdp4.0/trunk/src/tamisc/graphic_text.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_text.h,
	  pdp4.0/trunk/src/tamisc/graphic_viewer.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_viewer.h,
	  pdp4.0/trunk/src/tamisc/minmax.cpp,
	  pdp4.0/trunk/src/tamisc/minmax.h,
	  pdp4.0/trunk/src/tamisc/t3node_so.cpp,
	  pdp4.0/trunk/src/tamisc/t3node_so.h,
	  pdp4.0/trunk/src/tamisc/t3viewer.cpp,
	  pdp4.0/trunk/src/tamisc/t3viewer.h,
	  pdp4.0/trunk/src/tamisc/ta_data.cpp,
	  pdp4.0/trunk/src/tamisc/ta_data.h,
	  pdp4.0/trunk/src/tamisc/ta_data_defs.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_inst.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_type.h,
	  pdp4.0/trunk/src/tamisc/tamisc_maketa_defs.h,
	  pdp4.0/trunk/src/tamisc/tamisc_stdef.h,
	  pdp4.0/trunk/src/tamisc/tamisc_test.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_ti.cpp,
	  pdp4.0/trunk/src/tamisc/tarandom.cpp,
	  pdp4.0/trunk/src/tamisc/tarandom.h,
	  pdp4.0/trunk/src/tamisc/tdgeometry.cpp,
	  pdp4.0/trunk/src/tamisc/tdgeometry.h,
	  pdp4.0/trunk/src/tamisc/xform.cpp,
	  pdp4.0/trunk/src/tamisc/xform.h: LGPL - not sure why some files
	  end up with the copyright change and others don't.

* Wed Sep 28 2005 22:25 mingus

- pdp4.0/trunk/src/taiqtso/icheckbox.cpp,
	  pdp4.0/trunk/src/taiqtso/icheckbox.h,
	  pdp4.0/trunk/src/taiqtso/ichooserdialog.cpp,
	  pdp4.0/trunk/src/taiqtso/ichooserdialog.h,
	  pdp4.0/trunk/src/taiqtso/icombobox.cpp,
	  pdp4.0/trunk/src/taiqtso/icombobox.h,
	  pdp4.0/trunk/src/taiqtso/ieditgrid.cpp,
	  pdp4.0/trunk/src/taiqtso/ieditgrid.h,
	  pdp4.0/trunk/src/taiqtso/iflowbox.cpp,
	  pdp4.0/trunk/src/taiqtso/iflowbox.h,
	  pdp4.0/trunk/src/taiqtso/iflowlayout.cpp,
	  pdp4.0/trunk/src/taiqtso/iflowlayout.h,
	  pdp4.0/trunk/src/taiqtso/igeometry.cpp,
	  pdp4.0/trunk/src/taiqtso/igeometry.h,
	  pdp4.0/trunk/src/taiqtso/igroup_so.h,
	  pdp4.0/trunk/src/taiqtso/ihilightbutton.cpp,
	  pdp4.0/trunk/src/taiqtso/ihilightbutton.h,
	  pdp4.0/trunk/src/taiqtso/ilineedit.cpp,
	  pdp4.0/trunk/src/taiqtso/ilineedit.h,
	  pdp4.0/trunk/src/taiqtso/imisc_so.cpp,
	  pdp4.0/trunk/src/taiqtso/imisc_so.h,
	  pdp4.0/trunk/src/taiqtso/irenderarea.cpp,
	  pdp4.0/trunk/src/taiqtso/irenderarea.h,
	  pdp4.0/trunk/src/taiqtso/ispinbox.cpp,
	  pdp4.0/trunk/src/taiqtso/ispinbox.h,
	  pdp4.0/trunk/src/taiqtso/itransformer.cpp,
	  pdp4.0/trunk/src/taiqtso/itransformer.h,
	  pdp4.0/trunk/src/taiqtso/itreeview.cpp,
	  pdp4.0/trunk/src/taiqtso/itreeview.h,
	  pdp4.0/trunk/src/taiqtso/ivcrbuttons.cpp,
	  pdp4.0/trunk/src/taiqtso/ivcrbuttons.h,
	  pdp4.0/trunk/src/taiqtso/iwidgetlist.cpp,
	  pdp4.0/trunk/src/taiqtso/iwidgetlist.h,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.cpp,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.h,
	  pdp4.0/trunk/src/taiqtso/stdafx.h,
	  pdp4.0/trunk/src/taiqtso/taiqtdefs.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso.cpp,
	  pdp4.0/trunk/src/taiqtso/taiqtso.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA.cpp,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_inst.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_type.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_def.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_ti.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowbox.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowbox.h,
	  pdp4.0/trunk/src/taiqtso/test_iflowlayout.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowlayout.h,
	  pdp4.0/trunk/src/taiqtso/test_ispinbox.cpp,
	  pdp4.0/trunk/src/taiqtso/test_ispinbox.h,
	  pdp4.0/trunk/src/taiqtso/test_itreeview.cpp,
	  pdp4.0/trunk/src/taiqtso/test_itreeview.h,
	  pdp4.0/trunk/src/taiqtso/test_main.cpp,
	  pdp4.0/trunk/src/taiqtso/test_mainwindow.cpp,
	  pdp4.0/trunk/src/taiqtso/test_mainwindow.h,
	  pdp4.0/trunk/src/taiqtso/test_simplewidgets.cpp,
	  pdp4.0/trunk/src/taiqtso/test_simplewidgets.h,
	  pdp4.0/trunk/src/taiqtso/vcrbitmaps.h: LGPL

* Wed Sep 28 2005 22:12 mingus

- pdp4.0/trunk/src/ta/icolor.cpp, pdp4.0/trunk/src/ta/icolor.h,
	  pdp4.0/trunk/src/ta/ifont.cpp, pdp4.0/trunk/src/ta/ifont.h,
	  pdp4.0/trunk/src/ta/platform.h, pdp4.0/trunk/src/ta/qtdefs.h,
	  pdp4.0/trunk/src/ta/stdafx.h, pdp4.0/trunk/src/ta/ta.cpp,
	  pdp4.0/trunk/src/ta/ta.h, pdp4.0/trunk/src/ta/ta_TA.cpp,
	  pdp4.0/trunk/src/ta/ta_TA.msvc.cpp,
	  pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h,
	  pdp4.0/trunk/src/ta/ta_base.cpp, pdp4.0/trunk/src/ta/ta_base.h,
	  pdp4.0/trunk/src/ta/ta_def.h,
	  pdp4.0/trunk/src/ta/ta_defaults.cpp,
	  pdp4.0/trunk/src/ta/ta_defaults.h,
	  pdp4.0/trunk/src/ta/ta_dump.cpp, pdp4.0/trunk/src/ta/ta_dump.h,
	  pdp4.0/trunk/src/ta/ta_filer.cpp,
	  pdp4.0/trunk/src/ta/ta_filer.h,
	  pdp4.0/trunk/src/ta/ta_group.cpp,
	  pdp4.0/trunk/src/ta/ta_group.h, pdp4.0/trunk/src/ta/ta_list.cpp,
	  pdp4.0/trunk/src/ta/ta_list.h,
	  pdp4.0/trunk/src/ta/ta_maketa_defs.h,
	  pdp4.0/trunk/src/ta/ta_math.h,
	  pdp4.0/trunk/src/ta/ta_matrix.cpp,
	  pdp4.0/trunk/src/ta/ta_matrix.h,
	  pdp4.0/trunk/src/ta/ta_platform.cpp,
	  pdp4.0/trunk/src/ta/ta_platform.h,
	  pdp4.0/trunk/src/ta/ta_platform_cygwin.cpp,
	  pdp4.0/trunk/src/ta/ta_platform_unix.cpp,
	  pdp4.0/trunk/src/ta/ta_platform_win.cpp,
	  pdp4.0/trunk/src/ta/ta_qt.cpp, pdp4.0/trunk/src/ta/ta_qt.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.cpp,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse_def.h,
	  pdp4.0/trunk/src/ta/ta_qtclipdata.cpp,
	  pdp4.0/trunk/src/ta/ta_qtclipdata.h,
	  pdp4.0/trunk/src/ta/ta_qtcursors.h,
	  pdp4.0/trunk/src/ta/ta_qtdata.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdata.h,
	  pdp4.0/trunk/src/ta/ta_qtdata_def.h,
	  pdp4.0/trunk/src/ta/ta_qtdialog.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdialog.h,
	  pdp4.0/trunk/src/ta/ta_qtgroup.cpp,
	  pdp4.0/trunk/src/ta/ta_qtgroup.h,
	  pdp4.0/trunk/src/ta/ta_qttype.cpp,
	  pdp4.0/trunk/src/ta/ta_qttype.h,
	  pdp4.0/trunk/src/ta/ta_qttype_def.h,
	  pdp4.0/trunk/src/ta/ta_qtviewer.cpp,
	  pdp4.0/trunk/src/ta/ta_qtviewer.h,
	  pdp4.0/trunk/src/ta/ta_script.cpp,
	  pdp4.0/trunk/src/ta/ta_script.h,
	  pdp4.0/trunk/src/ta/ta_seledit.cpp,
	  pdp4.0/trunk/src/ta/ta_seledit.h,
	  pdp4.0/trunk/src/ta/ta_stdef.h,
	  pdp4.0/trunk/src/ta/ta_string.cpp,
	  pdp4.0/trunk/src/ta/ta_string.h, pdp4.0/trunk/src/ta/ta_ti.cpp,
	  pdp4.0/trunk/src/ta/ta_type.cpp, pdp4.0/trunk/src/ta/ta_type.h,
	  pdp4.0/trunk/src/ta/ta_type_constr.cpp,
	  pdp4.0/trunk/src/ta/ta_type_constr.h,
	  pdp4.0/trunk/src/ta/ta_type_qt.cpp,
	  pdp4.0/trunk/src/ta/ta_uifiler_qt.cpp,
	  pdp4.0/trunk/src/ta/ta_uifiler_qt.h,
	  pdp4.0/trunk/src/ta/taglobal.h: LGPL

* Wed Sep 28 2005 22:10 mingus

- pdp4.0/trunk/src/maketa/maketa.cpp,
	  pdp4.0/trunk/src/maketa/maketa.h,
	  pdp4.0/trunk/src/maketa/maketa_TA.cpp,
	  pdp4.0/trunk/src/maketa/maketa_TA_inst.h,
	  pdp4.0/trunk/src/maketa/maketa_TA_type.h,
	  pdp4.0/trunk/src/maketa/maketa_ti.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.h,
	  pdp4.0/trunk/src/maketa/mta_lex.cpp,
	  pdp4.0/trunk/src/maketa/mta_parse.cpp,
	  pdp4.0/trunk/src/maketa/mta_parse.h,
	  pdp4.0/trunk/src/maketa/mta_parse.y,
	  pdp4.0/trunk/src/maketa/stdafx.h,
	  pdp4.0/trunk/src/maketa/test_TA.cpp,
	  pdp4.0/trunk/src/maketa/test_TA_inst.h,
	  pdp4.0/trunk/src/maketa/test_TA_type.h,
	  pdp4.0/trunk/src/maketa/test_maketa.h: LGPL

* Wed Sep 28 2005 22:08 mingus

- pdp4.0/trunk/src/css/css_basic_types.cpp,
	  pdp4.0/trunk/src/css/css_basic_types.h,
	  pdp4.0/trunk/src/css/css_builtin.cpp,
	  pdp4.0/trunk/src/css/css_builtin.h,
	  pdp4.0/trunk/src/css/css_c_ptr_types.cpp,
	  pdp4.0/trunk/src/css/css_c_ptr_types.h,
	  pdp4.0/trunk/src/css/css_extern_support.h,
	  pdp4.0/trunk/src/css/css_lex.cpp,
	  pdp4.0/trunk/src/css/css_machine.cpp,
	  pdp4.0/trunk/src/css/css_machine.h,
	  pdp4.0/trunk/src/css/css_misc_funs.cpp,
	  pdp4.0/trunk/src/css/css_misc_funs.h,
	  pdp4.0/trunk/src/css/css_parse.cpp,
	  pdp4.0/trunk/src/css/css_parse.h,
	  pdp4.0/trunk/src/css/css_parse.y,
	  pdp4.0/trunk/src/css/css_qt.cpp, pdp4.0/trunk/src/css/css_qt.h,
	  pdp4.0/trunk/src/css/css_qtdata.cpp,
	  pdp4.0/trunk/src/css/css_qtdata.h,
	  pdp4.0/trunk/src/css/css_qtdialog.cpp,
	  pdp4.0/trunk/src/css/css_qtdialog.h,
	  pdp4.0/trunk/src/css/css_qttype.cpp,
	  pdp4.0/trunk/src/css/css_qttype.h,
	  pdp4.0/trunk/src/css/css_special_math.cpp,
	  pdp4.0/trunk/src/css/css_special_math.h,
	  pdp4.0/trunk/src/css/css_ti.cpp,
	  pdp4.0/trunk/src/css/hard_of_css.h,
	  pdp4.0/trunk/src/css/main.cpp,
	  pdp4.0/trunk/src/css/readline_win.cpp,
	  pdp4.0/trunk/src/css/special_math.old.c,
	  pdp4.0/trunk/src/css/ta_css.cpp, pdp4.0/trunk/src/css/ta_css.h,
	  pdp4.0/trunk/src/css/xmemory.cpp: LGPL

* Wed Sep 28 2005 21:57 mingus

- pdp4.0/trunk/COPYING.LIB: lgpl

* Wed Sep 28 2005 21:52 mingus

- pdp4.0/trunk/COPYING: COPYING contains the GPL, while
	  COPYING.LIB contains the LGPL. This followes the GNU coding
	  standards. See: http://www.gnu.org/prep/standards/standards.html

* Wed Sep 28 2005 21:42 mingus

- pdp4.0/trunk/src/tamisc/aggregate.cpp,
	  pdp4.0/trunk/src/tamisc/aggregate.h,
	  pdp4.0/trunk/src/tamisc/axis.cpp,
	  pdp4.0/trunk/src/tamisc/axis.h,
	  pdp4.0/trunk/src/tamisc/colorbar_qt.cpp,
	  pdp4.0/trunk/src/tamisc/colorbar_qt.h,
	  pdp4.0/trunk/src/tamisc/colorbar_so.cpp,
	  pdp4.0/trunk/src/tamisc/colorbar_so.h,
	  pdp4.0/trunk/src/tamisc/colorscale.cpp,
	  pdp4.0/trunk/src/tamisc/colorscale.h,
	  pdp4.0/trunk/src/tamisc/datagraph.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph.h,
	  pdp4.0/trunk/src/tamisc/datagraph_qtso.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph_qtso.h,
	  pdp4.0/trunk/src/tamisc/datagraph_so.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph_so.h,
	  pdp4.0/trunk/src/tamisc/datatable.cpp,
	  pdp4.0/trunk/src/tamisc/datatable.h,
	  pdp4.0/trunk/src/tamisc/datatable_qtso.cpp,
	  pdp4.0/trunk/src/tamisc/datatable_qtso.h,
	  pdp4.0/trunk/src/tamisc/fontspec.cpp,
	  pdp4.0/trunk/src/tamisc/fontspec.h,
	  pdp4.0/trunk/src/tamisc/fun_lookup.cpp,
	  pdp4.0/trunk/src/tamisc/fun_lookup.h,
	  pdp4.0/trunk/src/tamisc/graphic.cpp,
	  pdp4.0/trunk/src/tamisc/graphic.h,
	  pdp4.0/trunk/src/tamisc/graphic_objs.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_objs.h,
	  pdp4.0/trunk/src/tamisc/graphic_text.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_text.h,
	  pdp4.0/trunk/src/tamisc/graphic_viewer.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_viewer.h,
	  pdp4.0/trunk/src/tamisc/minmax.cpp,
	  pdp4.0/trunk/src/tamisc/minmax.h,
	  pdp4.0/trunk/src/tamisc/t3node_so.cpp,
	  pdp4.0/trunk/src/tamisc/t3node_so.h,
	  pdp4.0/trunk/src/tamisc/t3viewer.cpp,
	  pdp4.0/trunk/src/tamisc/t3viewer.h,
	  pdp4.0/trunk/src/tamisc/ta_data.cpp,
	  pdp4.0/trunk/src/tamisc/ta_data.h,
	  pdp4.0/trunk/src/tamisc/ta_data_defs.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_inst.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_type.h,
	  pdp4.0/trunk/src/tamisc/tamisc_maketa_defs.h,
	  pdp4.0/trunk/src/tamisc/tamisc_stdef.h,
	  pdp4.0/trunk/src/tamisc/tamisc_test.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_ti.cpp,
	  pdp4.0/trunk/src/tamisc/tarandom.cpp,
	  pdp4.0/trunk/src/tamisc/tarandom.h,
	  pdp4.0/trunk/src/tamisc/tdgeometry.cpp,
	  pdp4.0/trunk/src/tamisc/tdgeometry.h,
	  pdp4.0/trunk/src/tamisc/xform.cpp,
	  pdp4.0/trunk/src/tamisc/xform.h: tamisc copyright update LGPL

* Wed Sep 28 2005 21:41 mingus

- pdp4.0/trunk/src/taiqtso/icheckbox.cpp,
	  pdp4.0/trunk/src/taiqtso/icheckbox.h,
	  pdp4.0/trunk/src/taiqtso/ichooserdialog.cpp,
	  pdp4.0/trunk/src/taiqtso/ichooserdialog.h,
	  pdp4.0/trunk/src/taiqtso/icombobox.cpp,
	  pdp4.0/trunk/src/taiqtso/icombobox.h,
	  pdp4.0/trunk/src/taiqtso/ieditgrid.cpp,
	  pdp4.0/trunk/src/taiqtso/ieditgrid.h,
	  pdp4.0/trunk/src/taiqtso/iflowbox.cpp,
	  pdp4.0/trunk/src/taiqtso/iflowbox.h,
	  pdp4.0/trunk/src/taiqtso/iflowlayout.cpp,
	  pdp4.0/trunk/src/taiqtso/iflowlayout.h,
	  pdp4.0/trunk/src/taiqtso/igeometry.cpp,
	  pdp4.0/trunk/src/taiqtso/igeometry.h,
	  pdp4.0/trunk/src/taiqtso/igroup_so.h,
	  pdp4.0/trunk/src/taiqtso/ihilightbutton.cpp,
	  pdp4.0/trunk/src/taiqtso/ihilightbutton.h,
	  pdp4.0/trunk/src/taiqtso/ilineedit.cpp,
	  pdp4.0/trunk/src/taiqtso/ilineedit.h,
	  pdp4.0/trunk/src/taiqtso/imisc_so.cpp,
	  pdp4.0/trunk/src/taiqtso/imisc_so.h,
	  pdp4.0/trunk/src/taiqtso/irenderarea.cpp,
	  pdp4.0/trunk/src/taiqtso/irenderarea.h,
	  pdp4.0/trunk/src/taiqtso/ispinbox.cpp,
	  pdp4.0/trunk/src/taiqtso/ispinbox.h,
	  pdp4.0/trunk/src/taiqtso/itransformer.cpp,
	  pdp4.0/trunk/src/taiqtso/itransformer.h,
	  pdp4.0/trunk/src/taiqtso/itreeview.cpp,
	  pdp4.0/trunk/src/taiqtso/itreeview.h,
	  pdp4.0/trunk/src/taiqtso/ivcrbuttons.cpp,
	  pdp4.0/trunk/src/taiqtso/ivcrbuttons.h,
	  pdp4.0/trunk/src/taiqtso/iwidgetlist.cpp,
	  pdp4.0/trunk/src/taiqtso/iwidgetlist.h,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.cpp,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.h,
	  pdp4.0/trunk/src/taiqtso/stdafx.h,
	  pdp4.0/trunk/src/taiqtso/taiqtdefs.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso.cpp,
	  pdp4.0/trunk/src/taiqtso/taiqtso.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA.cpp,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_inst.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_type.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_def.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_ti.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowbox.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowbox.h,
	  pdp4.0/trunk/src/taiqtso/test_iflowlayout.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowlayout.h,
	  pdp4.0/trunk/src/taiqtso/test_ispinbox.cpp,
	  pdp4.0/trunk/src/taiqtso/test_ispinbox.h,
	  pdp4.0/trunk/src/taiqtso/test_itreeview.cpp,
	  pdp4.0/trunk/src/taiqtso/test_itreeview.h,
	  pdp4.0/trunk/src/taiqtso/test_main.cpp,
	  pdp4.0/trunk/src/taiqtso/test_mainwindow.cpp,
	  pdp4.0/trunk/src/taiqtso/test_mainwindow.h,
	  pdp4.0/trunk/src/taiqtso/test_simplewidgets.cpp,
	  pdp4.0/trunk/src/taiqtso/test_simplewidgets.h,
	  pdp4.0/trunk/src/taiqtso/vcrbitmaps.h: taiqtso copyright update
	  LGPL

* Wed Sep 28 2005 21:40 mingus

- pdp4.0/trunk/src/ta/icolor.cpp, pdp4.0/trunk/src/ta/icolor.h,
	  pdp4.0/trunk/src/ta/ifont.cpp, pdp4.0/trunk/src/ta/ifont.h,
	  pdp4.0/trunk/src/ta/platform.h, pdp4.0/trunk/src/ta/qtdefs.h,
	  pdp4.0/trunk/src/ta/stdafx.h, pdp4.0/trunk/src/ta/ta.cpp,
	  pdp4.0/trunk/src/ta/ta.h, pdp4.0/trunk/src/ta/ta_TA.cpp,
	  pdp4.0/trunk/src/ta/ta_TA.msvc.cpp,
	  pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h,
	  pdp4.0/trunk/src/ta/ta_base.cpp, pdp4.0/trunk/src/ta/ta_base.h,
	  pdp4.0/trunk/src/ta/ta_def.h,
	  pdp4.0/trunk/src/ta/ta_defaults.cpp,
	  pdp4.0/trunk/src/ta/ta_defaults.h,
	  pdp4.0/trunk/src/ta/ta_dump.cpp, pdp4.0/trunk/src/ta/ta_dump.h,
	  pdp4.0/trunk/src/ta/ta_filer.cpp,
	  pdp4.0/trunk/src/ta/ta_filer.h,
	  pdp4.0/trunk/src/ta/ta_group.cpp,
	  pdp4.0/trunk/src/ta/ta_group.h, pdp4.0/trunk/src/ta/ta_list.cpp,
	  pdp4.0/trunk/src/ta/ta_list.h,
	  pdp4.0/trunk/src/ta/ta_maketa_defs.h,
	  pdp4.0/trunk/src/ta/ta_math.h,
	  pdp4.0/trunk/src/ta/ta_matrix.cpp,
	  pdp4.0/trunk/src/ta/ta_matrix.h,
	  pdp4.0/trunk/src/ta/ta_platform.cpp,
	  pdp4.0/trunk/src/ta/ta_platform.h,
	  pdp4.0/trunk/src/ta/ta_platform_cygwin.cpp,
	  pdp4.0/trunk/src/ta/ta_platform_unix.cpp,
	  pdp4.0/trunk/src/ta/ta_platform_win.cpp,
	  pdp4.0/trunk/src/ta/ta_qt.cpp, pdp4.0/trunk/src/ta/ta_qt.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.cpp,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse_def.h,
	  pdp4.0/trunk/src/ta/ta_qtclipdata.cpp,
	  pdp4.0/trunk/src/ta/ta_qtclipdata.h,
	  pdp4.0/trunk/src/ta/ta_qtcursors.h,
	  pdp4.0/trunk/src/ta/ta_qtdata.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdata.h,
	  pdp4.0/trunk/src/ta/ta_qtdata_def.h,
	  pdp4.0/trunk/src/ta/ta_qtdialog.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdialog.h,
	  pdp4.0/trunk/src/ta/ta_qtgroup.cpp,
	  pdp4.0/trunk/src/ta/ta_qtgroup.h,
	  pdp4.0/trunk/src/ta/ta_qttype.cpp,
	  pdp4.0/trunk/src/ta/ta_qttype.h,
	  pdp4.0/trunk/src/ta/ta_qttype_def.h,
	  pdp4.0/trunk/src/ta/ta_qtviewer.cpp,
	  pdp4.0/trunk/src/ta/ta_qtviewer.h,
	  pdp4.0/trunk/src/ta/ta_script.cpp,
	  pdp4.0/trunk/src/ta/ta_script.h,
	  pdp4.0/trunk/src/ta/ta_seledit.cpp,
	  pdp4.0/trunk/src/ta/ta_seledit.h,
	  pdp4.0/trunk/src/ta/ta_stdef.h,
	  pdp4.0/trunk/src/ta/ta_string.cpp,
	  pdp4.0/trunk/src/ta/ta_string.h, pdp4.0/trunk/src/ta/ta_ti.cpp,
	  pdp4.0/trunk/src/ta/ta_type.cpp, pdp4.0/trunk/src/ta/ta_type.h,
	  pdp4.0/trunk/src/ta/ta_type_constr.cpp,
	  pdp4.0/trunk/src/ta/ta_type_constr.h,
	  pdp4.0/trunk/src/ta/ta_type_qt.cpp,
	  pdp4.0/trunk/src/ta/ta_uifiler_qt.cpp,
	  pdp4.0/trunk/src/ta/ta_uifiler_qt.h,
	  pdp4.0/trunk/src/ta/taglobal.h: ta copyright update LGPL

* Wed Sep 28 2005 21:38 mingus

- pdp4.0/trunk/src/maketa/maketa.cpp,
	  pdp4.0/trunk/src/maketa/maketa.h,
	  pdp4.0/trunk/src/maketa/maketa_ti.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.h,
	  pdp4.0/trunk/src/maketa/mta_lex.cpp,
	  pdp4.0/trunk/src/maketa/mta_parse.cpp,
	  pdp4.0/trunk/src/maketa/mta_parse.h,
	  pdp4.0/trunk/src/maketa/mta_parse.y,
	  pdp4.0/trunk/src/maketa/stdafx.h,
	  pdp4.0/trunk/src/maketa/test_maketa.h: maketa copyright update
	  LGPL (until told otherwise)

* Wed Sep 28 2005 21:36 mingus

- pdp4.0/trunk/src/css/css_basic_types.cpp,
	  pdp4.0/trunk/src/css/css_basic_types.h,
	  pdp4.0/trunk/src/css/css_builtin.cpp,
	  pdp4.0/trunk/src/css/css_builtin.h,
	  pdp4.0/trunk/src/css/css_c_ptr_types.cpp,
	  pdp4.0/trunk/src/css/css_c_ptr_types.h,
	  pdp4.0/trunk/src/css/css_extern_support.h,
	  pdp4.0/trunk/src/css/css_lex.cpp,
	  pdp4.0/trunk/src/css/css_machine.cpp,
	  pdp4.0/trunk/src/css/css_machine.h,
	  pdp4.0/trunk/src/css/css_misc_funs.cpp,
	  pdp4.0/trunk/src/css/css_misc_funs.h,
	  pdp4.0/trunk/src/css/css_parse.cpp,
	  pdp4.0/trunk/src/css/css_parse.h,
	  pdp4.0/trunk/src/css/css_parse.y,
	  pdp4.0/trunk/src/css/css_qt.cpp, pdp4.0/trunk/src/css/css_qt.h,
	  pdp4.0/trunk/src/css/css_qtdata.cpp,
	  pdp4.0/trunk/src/css/css_qtdata.h,
	  pdp4.0/trunk/src/css/css_qtdialog.cpp,
	  pdp4.0/trunk/src/css/css_qtdialog.h,
	  pdp4.0/trunk/src/css/css_qttype.cpp,
	  pdp4.0/trunk/src/css/css_qttype.h,
	  pdp4.0/trunk/src/css/css_special_math.cpp,
	  pdp4.0/trunk/src/css/css_special_math.h,
	  pdp4.0/trunk/src/css/css_ti.cpp,
	  pdp4.0/trunk/src/css/hard_of_css.h,
	  pdp4.0/trunk/src/css/main.cpp,
	  pdp4.0/trunk/src/css/readline_win.cpp,
	  pdp4.0/trunk/src/css/special_math.old.c,
	  pdp4.0/trunk/src/css/ta_css.cpp, pdp4.0/trunk/src/css/ta_css.h,
	  pdp4.0/trunk/src/css/xmemory.cpp: css copyright update LGPL

* Wed Sep 28 2005 21:33 mingus

- pdp4.0/trunk/src/so/cl.cpp, pdp4.0/trunk/src/so/cl.h,
	  pdp4.0/trunk/src/so/main.cpp, pdp4.0/trunk/src/so/so.cpp,
	  pdp4.0/trunk/src/so/so.h, pdp4.0/trunk/src/so/so_ti.cpp,
	  pdp4.0/trunk/src/so/som.cpp, pdp4.0/trunk/src/so/som.h,
	  pdp4.0/trunk/src/so/zsh.cpp, pdp4.0/trunk/src/so/zsh.h: so
	  copyright update GPL

* Wed Sep 28 2005 21:32 mingus

- pdp4.0/trunk/src/so/main.cc, pdp4.0/trunk/src/so/main.cpp:
	  updating main.cc to main.cpp

* Wed Sep 28 2005 21:31 mingus

- pdp4.0/trunk/src/pdp/enviro.cpp, pdp4.0/trunk/src/pdp/enviro.h,
	  pdp4.0/trunk/src/pdp/enviro_extra.cpp,
	  pdp4.0/trunk/src/pdp/enviro_extra.h,
	  pdp4.0/trunk/src/pdp/enviro_qt.cpp,
	  pdp4.0/trunk/src/pdp/enviro_qt.h,
	  pdp4.0/trunk/src/pdp/enviro_qtso.cpp,
	  pdp4.0/trunk/src/pdp/enviro_qtso.h,
	  pdp4.0/trunk/src/pdp/main.cpp, pdp4.0/trunk/src/pdp/netstru.cpp,
	  pdp4.0/trunk/src/pdp/netstru.h,
	  pdp4.0/trunk/src/pdp/netstru_extra.cpp,
	  pdp4.0/trunk/src/pdp/netstru_extra.h,
	  pdp4.0/trunk/src/pdp/netstru_qtso.cpp,
	  pdp4.0/trunk/src/pdp/netstru_qtso.h,
	  pdp4.0/trunk/src/pdp/netstru_so.cpp,
	  pdp4.0/trunk/src/pdp/netstru_so.h,
	  pdp4.0/trunk/src/pdp/pdp_qtso.cpp,
	  pdp4.0/trunk/src/pdp/pdp_qtso.h,
	  pdp4.0/trunk/src/pdp/pdp_ti.cpp,
	  pdp4.0/trunk/src/pdp/pdpbase.cpp,
	  pdp4.0/trunk/src/pdp/pdpbase.h,
	  pdp4.0/trunk/src/pdp/pdpbase_unix.cpp,
	  pdp4.0/trunk/src/pdp/pdpbase_win.cpp,
	  pdp4.0/trunk/src/pdp/pdpbitmaps.h,
	  pdp4.0/trunk/src/pdp/pdpdeclare.h,
	  pdp4.0/trunk/src/pdp/pdplog.cpp, pdp4.0/trunk/src/pdp/pdplog.h,
	  pdp4.0/trunk/src/pdp/pdplog_qtso.cpp,
	  pdp4.0/trunk/src/pdp/pdplog_qtso.h,
	  pdp4.0/trunk/src/pdp/pdpshell.cpp,
	  pdp4.0/trunk/src/pdp/pdpshell.h,
	  pdp4.0/trunk/src/pdp/pdpview_qt.cpp,
	  pdp4.0/trunk/src/pdp/pdpview_qt.h,
	  pdp4.0/trunk/src/pdp/process.cpp,
	  pdp4.0/trunk/src/pdp/process.h,
	  pdp4.0/trunk/src/pdp/procs_extra.cpp,
	  pdp4.0/trunk/src/pdp/procs_extra.h,
	  pdp4.0/trunk/src/pdp/sched_proc.cpp,
	  pdp4.0/trunk/src/pdp/sched_proc.h,
	  pdp4.0/trunk/src/pdp/script_base.h,
	  pdp4.0/trunk/src/pdp/spec.cpp, pdp4.0/trunk/src/pdp/spec.h,
	  pdp4.0/trunk/src/pdp/stats.cpp, pdp4.0/trunk/src/pdp/stats.h:
	  update pdp copyright GPL

* Wed Sep 28 2005 21:29 mingus

- pdp4.0/trunk/src/leabra/leabra.cpp,
	  pdp4.0/trunk/src/leabra/leabra.h,
	  pdp4.0/trunk/src/leabra/leabra_TA.cpp,
	  pdp4.0/trunk/src/leabra/leabra_TA_inst.h,
	  pdp4.0/trunk/src/leabra/leabra_TA_type.h,
	  pdp4.0/trunk/src/leabra/leabra_ti.cpp,
	  pdp4.0/trunk/src/leabra/main.cpp: leabra copyright update GPL

* Wed Sep 28 2005 21:20 mingus

- pdp4.0/trunk/src/cs/cs.cpp, pdp4.0/trunk/src/cs/cs.h,
	  pdp4.0/trunk/src/cs/cs_ti.cpp, pdp4.0/trunk/src/cs/cs_vt.cpp,
	  pdp4.0/trunk/src/cs/main.cpp: cs copyright data updated

* Wed Sep 28 2005 21:17 mingus

- pdp4.0/trunk/src/bp/Resource.h, pdp4.0/trunk/src/bp/bp.cpp,
	  pdp4.0/trunk/src/bp/bp.h, pdp4.0/trunk/src/bp/bp_TA.cpp,
	  pdp4.0/trunk/src/bp/bp_TA_inst.h,
	  pdp4.0/trunk/src/bp/bp_TA_type.h, pdp4.0/trunk/src/bp/bp_ti.cpp,
	  pdp4.0/trunk/src/bp/bp_vt.cpp, pdp4.0/trunk/src/bp/main.cpp,
	  pdp4.0/trunk/src/bp/main_debug.cpp, pdp4.0/trunk/src/bp/rbp.cpp,
	  pdp4.0/trunk/src/bp/rbp.h: bp copyright update

* Wed Sep 28 2005 20:46 mingus

- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/maketa/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am:

* Wed Sep 28 2005 20:45 mingus

- pdp4.0/trunk/Conditionals.am:
- pdp4.0/trunk/configure.ac:

* Tue Sep 27 2005 22:14 baisa

- pdp4.0/trunk/src/ta/ta_TA.cpp, pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h, pdp4.0/trunk/src/ta/ta_type.h:
	  fixed String_PArray for maketa

* Tue Sep 27 2005 18:26 baisa

- pdp4.0/trunk/src/css/css_basic_types.cpp,
	  pdp4.0/trunk/src/css/css_basic_types.h,
	  pdp4.0/trunk/src/css/css_builtin.cpp,
	  pdp4.0/trunk/src/css/css_misc_funs.cpp: data-related changes
- pdp4.0/trunk/src/leabra/leabra_TA.cpp: data-related changes
- pdp4.0/trunk/src/bp/bp_TA.cpp, pdp4.0/trunk/src/bp/bp_TA_inst.h,
	  pdp4.0/trunk/src/bp/rbp.cpp, pdp4.0/trunk/src/bp/rbp.h:
	  data-related changes
- pdp4.0/trunk/src/pdp/enviro.cpp, pdp4.0/trunk/src/pdp/enviro.h,
	  pdp4.0/trunk/src/pdp/enviro_extra.cpp,
	  pdp4.0/trunk/src/pdp/enviro_extra.h,
	  pdp4.0/trunk/src/pdp/netstru.cpp,
	  pdp4.0/trunk/src/pdp/netstru.h,
	  pdp4.0/trunk/src/pdp/netstru_qtso.cpp,
	  pdp4.0/trunk/src/pdp/netstru_qtso.h,
	  pdp4.0/trunk/src/pdp/pdp_TA.cpp,
	  pdp4.0/trunk/src/pdp/pdp_TA_type.h,
	  pdp4.0/trunk/src/pdp/pdp_ti.cpp,
	  pdp4.0/trunk/src/pdp/pdplog.cpp,
	  pdp4.0/trunk/src/pdp/procs_extra.cpp: data-related changes

* Tue Sep 27 2005 18:24 baisa

- pdp4.0/trunk/src/tamisc/Makefile.am,
	  pdp4.0/trunk/src/tamisc/datatable.cpp,
	  pdp4.0/trunk/src/tamisc/datatable.h,
	  pdp4.0/trunk/src/tamisc/fun_lookup.h,
	  pdp4.0/trunk/src/tamisc/ta_data.cpp,
	  pdp4.0/trunk/src/tamisc/ta_data.h,
	  pdp4.0/trunk/src/tamisc/ta_data_defs.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_inst.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_type.h,
	  pdp4.0/trunk/src/tamisc/tamisc_ti.cpp,
	  pdp4.0/trunk/src/tamisc/tdgeometry.h: added ta_data

* Tue Sep 27 2005 18:21 baisa

- pdp4.0/trunk/src/ta/Makefile.am: added ta_matrix

* Tue Sep 27 2005 02:00 mingus

- pdp4.0/trunk/src/maketa/Makefile.am:
- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am:

* Tue Sep 27 2005 01:59 mingus

- pdp4.0/trunk/configure.ac:
- pdp4.0/trunk/Conditionals.am, pdp4.0/trunk/Makefile.am,
	  pdp4.0/trunk/Maketa.am, pdp4.0/trunk/acsite.m4: flag cleanup,
	  suffixes etc..

* Mon Sep 26 2005 21:25 baisa

- pdp4.0/trunk/src/ta/taglobal.h: misc changes

* Mon Sep 26 2005 21:22 baisa

- pdp4.0/trunk/src/ta/ta_TA.cpp, pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h,
	  pdp4.0/trunk/src/ta/ta_base.cpp, pdp4.0/trunk/src/ta/ta_base.h,
	  pdp4.0/trunk/src/ta/ta_list.cpp, pdp4.0/trunk/src/ta/ta_list.h,
	  pdp4.0/trunk/src/ta/ta_matrix.cpp,
	  pdp4.0/trunk/src/ta/ta_matrix.h,
	  pdp4.0/trunk/src/ta/ta_qtdata.cpp,
	  pdp4.0/trunk/src/ta/ta_seledit.cpp,
	  pdp4.0/trunk/src/ta/ta_ti.cpp, pdp4.0/trunk/src/ta/ta_type.h:
	  massive changes for arrays and data

* Mon Sep 26 2005 16:48 mingus

- pdp4.0/trunk/acsite.m4: Contains PDP_DETERMINE_SUFFIX and also
	  ACX_MPI for probing MPI

* Sat Sep 24 2005 19:16 mingus

- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am: Made the program names e.g.,
	  bp++ pdp++. See
	  http://grey.colorado.edu/cgi-bin/trac.cgi/ticket/8 for details.

* Sat Sep 24 2005 01:53 mingus

- pdp4.0/trunk/Makefile.am:

* Sat Sep 24 2005 01:52 mingus

- pdp4.0/trunk/Makefile.am: unneeded

* Sat Sep 24 2005 01:47 mingus

- pdp4.0/trunk/Makefile.am: make linecount

* Thu Sep 22 2005 19:55 mingus

- pdp4.0/trunk/src/platform.h, pdp4.0/trunk/src/qtdefs.h,
	  pdp4.0/trunk/src/ta/platform.h, pdp4.0/trunk/src/ta/qtdefs.h,
	  pdp4.0/trunk/src/ta/taglobal.h, pdp4.0/trunk/src/taglobal.h:
	  Moving homeless files in /src/ into /ta

* Thu Sep 22 2005 19:54 mingus

- pdp4.0/trunk/src/maketa/ta_platform_cygwin.cpp,
	  pdp4.0/trunk/src/maketa/ta_platform_unix.cpp:
- pdp4.0/trunk/src/maketa/ta_list.cpp,
	  pdp4.0/trunk/src/maketa/ta_list.h,
	  pdp4.0/trunk/src/maketa/ta_platform.cpp,
	  pdp4.0/trunk/src/maketa/ta_platform.h,
	  pdp4.0/trunk/src/maketa/ta_platform_win.cpp,
	  pdp4.0/trunk/src/maketa/ta_string.cpp,
	  pdp4.0/trunk/src/maketa/ta_string.h,
	  pdp4.0/trunk/src/maketa/ta_type.cpp,
	  pdp4.0/trunk/src/maketa/ta_type.h:

* Thu Sep 22 2005 19:45 mingus

- pdp4.0/trunk/INSTALL, pdp4.0/trunk/README,
	  pdp4.0/trunk/configure.ac:

* Thu Sep 22 2005 19:36 mingus

- pdp4.0/trunk/AUTHORS: Populated AUTHORS with
	  http://www.cnbc.cmu.edu/Resources/PDP++//contributors.html and
	  some of http://www.cnbc.cmu.edu/Resources/PDP++//PDP++.html

* Thu Sep 22 2005 19:34 mingus

- pdp4.0/trunk/COPYING: Populated COPYING with
	  http://www.cnbc.cmu.edu/Resources/PDP++//copyright.html

* Thu Sep 22 2005 19:32 mingus

- pdp4.0/trunk/NEWS: Updated NEWS file with data from
	  http://www.cnbc.cmu.edu/Resources/PDP++//new_features.html

* Thu Sep 22 2005 19:22 mingus

- pdp4.0/trunk/ChangeLog: Created GNU compliant ChangeLog from
	  Subversion commits. See wiki for instructions on how to do this

* Thu Sep 22 2005 00:13 mingus

- pdp4.0/trunk/src/bp/Makefile.am,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am: misc
- pdp4.0/trunk/Conditionals.am, pdp4.0/trunk/Makefile.am,
	  pdp4.0/trunk/configure.ac: misc

* Wed Sep 21 2005 23:32 baisa

- pdp4.0/trunk/src/css/main.cpp: nuked malloc.h

* Tue Sep 20 2005 22:26 mingus

- pdp4.0/trunk/src/pdp/main.cpp: const char --> char

* Tue Sep 20 2005 01:28 mingus

- pdp4.0/trunk/src/tamisc/Makefile.am: Make distcheck changes

* Tue Sep 20 2005 01:27 mingus

- pdp4.0/trunk/src/pdp/Makefile.am: Make distcheck changes
- pdp4.0/trunk/src/leabra/Makefile.am: Make distcheck changes
- pdp4.0/trunk/src/ta/Makefile.am: Make distcheck changes
- pdp4.0/trunk/Conditionals.am, pdp4.0/trunk/Makefile.am,
	  pdp4.0/trunk/Maketa.am, pdp4.0/trunk/configure.ac: Make
	  distcheck changes

* Mon Sep 19 2005 23:31 mingus

- pdp4.0/trunk/src/css/css_qtdialog.cpp: fixing paths

* Mon Sep 19 2005 23:29 mingus

- pdp4.0/trunk/src/ta/ta_type_constr.cpp: ta/ta_type_constr.h -->
	  ta_type_constr.h

* Mon Sep 19 2005 21:52 mingus

- pdp4.0/trunk/src/ta/ta_type.cpp: maketa/maketa.h --> maketa.h

* Mon Sep 19 2005 19:56 mingus

- pdp4.0/trunk/src/maketa/maketa.cpp: ta/ta_platform.h -->
	  ta_platform.h

* Mon Sep 19 2005 19:53 mingus

- pdp4.0/trunk/src/ta/ta_type.h: ta/ta_def.h --> ta_def.h

* Mon Sep 19 2005 19:50 mingus

- pdp4.0/trunk/src/ta/ta_string.h: ta/ta_def.h --> ta_def.h

* Mon Sep 19 2005 19:33 mingus

- pdp4.0/trunk/src/ta/ta_def.h: ta/ta_stdef.h --> ta_stdef.h

* Mon Sep 19 2005 19:31 mingus

- pdp4.0/trunk/src/ta/ta_platform.h: ta/ta_def.h --> ta_def.h

* Mon Sep 19 2005 19:29 mingus

- pdp4.0/trunk/src/maketa/mta_lex.cpp: ta/ta_platform.h -->
	  ta_platform.h

* Mon Sep 19 2005 19:26 mingus

- pdp4.0/trunk/src/ta/ta_list.h: ta/ta_def.h --> ta_def.h

* Mon Sep 19 2005 19:22 mingus

- pdp4.0/trunk/src/ta/ta_platform_unix.cpp: ta/ta_platform.h -->
	  ta_platform.h

* Mon Sep 19 2005 19:14 mingus

- pdp4.0/trunk/src/maketa/maketa.h: ta/ta_type.h --> ta_type.h

* Mon Sep 19 2005 19:12 mingus

- pdp4.0/trunk/src/css/main.cpp: const char *argv[]

* Mon Sep 19 2005 19:00 mingus

- pdp4.0/trunk/Conditionals.am: minor

* Sun Sep 18 2005 21:14 mingus

- pdp4.0/trunk/src/tamisc/tarandom.cpp: missing newline @ EOF

* Sun Sep 18 2005 21:10 mingus

- pdp4.0/trunk/src/taiqtso/Makefile.am: reverting to old moc

* Sun Sep 18 2005 21:08 mingus

- pdp4.0/trunk/Conditionals.am, pdp4.0/trunk/Moc.am,
	  pdp4.0/trunk/configure.ac, pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/leabra/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am, pdp4.0/trunk/src/ta/ta_TA.cpp,
	  pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h,
	  pdp4.0/trunk/src/tamisc/Makefile.am: I had to revert moc back to
	  the old rules as the GNU Make pattern rules broke the automatic
	  dependency tracking and automake would not finish without
	  erroring out.

* Sun Sep 18 2005 20:39 mingus

- pdp4.0/trunk/src/pdp/Makefile.am: change paradigm from .moc.cpp
	  to moc_
- pdp4.0/trunk/src/pdp/Makefile.am: change paradigm from .moc.cpp
	  to moc_

* Sun Sep 18 2005 20:38 mingus

- pdp4.0/trunk/src/taiqtso/Makefile.am: change paradigm from
	  .moc.cpp to moc_

* Sun Sep 18 2005 20:37 mingus

- pdp4.0/trunk/src/tamisc/Makefile.am: change paradigm from
	  .moc.cpp to moc_

* Sun Sep 18 2005 20:35 mingus

- pdp4.0/trunk/src/css/Makefile.am: change paradigm from .moc.cpp
	  to moc_

* Sun Sep 18 2005 20:34 mingus

- pdp4.0/trunk/src/ta/Makefile.am: change paradigm from .moc.cpp
	  to moc_

* Sun Sep 18 2005 20:30 mingus

- pdp4.0/trunk/Moc.am: Change to GNU Make dependent moc_%.cpp: %.h
	  rule

* Sun Sep 18 2005 20:24 baisa

- pdp4.0/trunk/src/ta/ta_base.h: reverted byte_Array

* Sun Sep 18 2005 20:23 baisa

- pdp4.0/trunk/src/taglobal.h:

* Sun Sep 18 2005 20:18 mingus

- pdp4.0/trunk/Makefile.am: Updated to include all *TA* files
	  within EXTRA_DIST. This is necessary because we will be
	  distributing premade versions of these files. Since we have also
	  told the autotools that we explicitly make these files if they
	  are not present, we need to tell the autotools to also
	  distribute them initially. This update also creates a .PHONY
	  target for all the files in /src/ta/ios-g++-3.1/. This is
	  because these files are never made. In order to pass 'make
	  distcheck' there must be a rule to bring them up to date, but
	  since that never happens we'll use a phony one.

* Sun Sep 18 2005 19:55 mingus

- pdp4.0/trunk/src/bp/Makefile.am: adding bp makefile.am

* Sun Sep 18 2005 19:44 mingus

- pdp4.0/trunk/src/leabra/Makefile.am: Adding leabra makefile

* Fri Sep 16 2005 23:57 mingus

- pdp4.0/trunk/ltmain.sh: another required file

* Fri Sep 16 2005 23:53 mingus

- pdp4.0/trunk/config.guess, pdp4.0/trunk/config.sub,
	  pdp4.0/trunk/depcomp, pdp4.0/trunk/install-sh,
	  pdp4.0/trunk/missing: These files are generated by 'automake
	  -ac' and are required

* Fri Sep 16 2005 23:42 mingus

- pdp4.0/trunk/Conditionals.am, pdp4.0/trunk/Makefile.am,
	  pdp4.0/trunk/Maketa.am, pdp4.0/trunk/Moc.am,
	  pdp4.0/trunk/acsite.m4, pdp4.0/trunk/configure.ac,
	  pdp4.0/trunk/src/css/Makefile.am,
	  pdp4.0/trunk/src/maketa/Makefile.am,
	  pdp4.0/trunk/src/pdp/Makefile.am,
	  pdp4.0/trunk/src/ta/Makefile.am,
	  pdp4.0/trunk/src/taiqtso/Makefile.am,
	  pdp4.0/trunk/src/tamisc/Makefile.am: Adding autotools files

* Tue Sep 13 2005 01:41 baisa

- pdp4.0/trunk/src/ta/ta_TA.cpp, pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h, pdp4.0/trunk/src/ta/ta_base.h,
	  pdp4.0/trunk/src/ta/ta_string.h,
	  pdp4.0/trunk/src/ta/ta_type.cpp, pdp4.0/trunk/src/ta/ta_type.h:
	  added 'byte' type, and size to intrinsics
- pdp4.0/trunk/src/maketa/maketa.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.cpp,
	  pdp4.0/trunk/src/maketa/ta_type.cpp,
	  pdp4.0/trunk/src/maketa/ta_type.h: added 'byte' type, and size
	  to intrinsics

* Mon Sep 12 2005 19:03 baisa

- pdp4.0/trunk/src/maketa/maketa.cpp: change to cpp command to
	  support 'gcc -E'

* Fri Sep 09 2005 23:13 pdpsvn

- pdp4.0/trunk/src/cs, pdp4.0/trunk/src/cs/cs.cpp,
	  pdp4.0/trunk/src/cs/cs.def, pdp4.0/trunk/src/cs/cs.h,
	  pdp4.0/trunk/src/cs/cs_det.def, pdp4.0/trunk/src/cs/cs_iac.def,
	  pdp4.0/trunk/src/cs/cs_prob_env.def,
	  pdp4.0/trunk/src/cs/cs_ti.cpp, pdp4.0/trunk/src/cs/cs_vt.cpp,
	  pdp4.0/trunk/src/cs/main.cpp, pdp4.0/trunk/src/so,
	  pdp4.0/trunk/src/so/cl.cpp, pdp4.0/trunk/src/so/cl.h,
	  pdp4.0/trunk/src/so/main.cc, pdp4.0/trunk/src/so/so.cpp,
	  pdp4.0/trunk/src/so/so.def, pdp4.0/trunk/src/so/so.h,
	  pdp4.0/trunk/src/so/so_cl.def, pdp4.0/trunk/src/so/so_mxi.def,
	  pdp4.0/trunk/src/so/so_scl.def, pdp4.0/trunk/src/so/so_som.def,
	  pdp4.0/trunk/src/so/so_ti.cpp, pdp4.0/trunk/src/so/so_zsh.def,
	  pdp4.0/trunk/src/so/som.cpp, pdp4.0/trunk/src/so/som.h,
	  pdp4.0/trunk/src/so/zsh.cpp, pdp4.0/trunk/src/so/zsh.h:

* Wed Sep 07 2005 01:30 baisa

- pdp4.0/trunk/src/ta/fstream-maketa.h,
	  pdp4.0/trunk/src/ta/iostream-maketa.h,
	  pdp4.0/trunk/src/ta/sstream-maketa.h,
	  pdp4.0/trunk/src/ta/streambuf-maketa.h: deleted stream headers

* Wed Sep 07 2005 00:42 baisa

- pdp4.0/trunk/src/leabra/leabra_TA.cpp,
	  pdp4.0/trunk/src/leabra/leabra_TA_inst.h,
	  pdp4.0/trunk/src/leabra/leabra_TA_type.h,
	  pdp4.0/trunk/src/leabra/main.cpp: removed const from main()

* Sat Sep 03 2005 00:34 baisa

- pdp4.0/trunk/3rdparty, pdp4.0/trunk/3rdparty/zlib123.zip,
	  pdp4.0/trunk/bin, pdp4.0/trunk/bin/back.xpm,
	  pdp4.0/trunk/bin/bak, pdp4.0/trunk/bin/forward.xpm,
	  pdp4.0/trunk/bin/home.xpm, pdp4.0/trunk/bin/maketa,
	  pdp4.0/trunk/doc, pdp4.0/trunk/doc/ClassRedesign.txt,
	  pdp4.0/trunk/doc/Coin-Inventor.txt,
	  pdp4.0/trunk/doc/Coin-Inventor2.txt,
	  pdp4.0/trunk/doc/ConsoleAndGuiHandling.txt,
	  pdp4.0/trunk/doc/CrossPlatformNotes.txt,
	  pdp4.0/trunk/doc/DataEnv.txt, pdp4.0/trunk/doc/MacNotes.txt,
	  pdp4.0/trunk/doc/Properties.txt, pdp4.0/trunk/doc/Threading.txt,
	  pdp4.0/trunk/doc/WindowsNotes.txt, pdp4.0/trunk/doc/actions.txt,
	  pdp4.0/trunk/doc/bugs.txt, pdp4.0/trunk/doc/changed_files.txt,
	  pdp4.0/trunk/doc/compile_switches.txt,
	  pdp4.0/trunk/doc/dataviews.txt,
	  pdp4.0/trunk/doc/debug_winport.txt,
	  pdp4.0/trunk/doc/debugging.txt, pdp4.0/trunk/doc/drag_drop.sxc,
	  pdp4.0/trunk/doc/net3d_analysis.txt,
	  pdp4.0/trunk/doc/new_directives_4.0.txt,
	  pdp4.0/trunk/doc/robo-sim_feasibility.txt,
	  pdp4.0/trunk/doc/todo.txt, pdp4.0/trunk/findgrep,
	  pdp4.0/trunk/include, pdp4.0/trunk/include/zconf.h,
	  pdp4.0/trunk/include/zlib.h, pdp4.0/trunk/lib,
	  pdp4.0/trunk/lib/zdll.exp, pdp4.0/trunk/lib/zdll.lib,
	  pdp4.0/trunk/lib/zlib.def, pdp4.0/trunk/setshell,
	  pdp4.0/trunk/src, pdp4.0/trunk/src/bp,
	  pdp4.0/trunk/src/bp/Resource.h, pdp4.0/trunk/src/bp/bp.cpp,
	  pdp4.0/trunk/src/bp/bp.def, pdp4.0/trunk/src/bp/bp.h,
	  pdp4.0/trunk/src/bp/bp.ico, pdp4.0/trunk/src/bp/bp.rc,
	  pdp4.0/trunk/src/bp/bp.vcproj, pdp4.0/trunk/src/bp/bp_TA.cpp,
	  pdp4.0/trunk/src/bp/bp_TA_inst.h,
	  pdp4.0/trunk/src/bp/bp_TA_type.h,
	  pdp4.0/trunk/src/bp/bp_seq.def, pdp4.0/trunk/src/bp/bp_ti.cpp,
	  pdp4.0/trunk/src/bp/bp_vt.cpp, pdp4.0/trunk/src/bp/bpdefaults,
	  pdp4.0/trunk/src/bp/main.cpp,
	  pdp4.0/trunk/src/bp/main_debug.cpp, pdp4.0/trunk/src/bp/rbp.cpp,
	  pdp4.0/trunk/src/bp/rbp.def, pdp4.0/trunk/src/bp/rbp.h,
	  pdp4.0/trunk/src/bp/rbp_ap.def, pdp4.0/trunk/src/bp/small.ico,
	  pdp4.0/trunk/src/bp/startup.css,
	  pdp4.0/trunk/src/bp/xor_2nets.proj.gz, pdp4.0/trunk/src/css,
	  pdp4.0/trunk/src/css/NoMakeTA, pdp4.0/trunk/src/css/TODO,
	  pdp4.0/trunk/src/css/css.vcproj,
	  pdp4.0/trunk/src/css/css_basic_types.cpp,
	  pdp4.0/trunk/src/css/css_basic_types.h,
	  pdp4.0/trunk/src/css/css_builtin.cpp,
	  pdp4.0/trunk/src/css/css_builtin.h,
	  pdp4.0/trunk/src/css/css_c_ptr_types.cpp,
	  pdp4.0/trunk/src/css/css_c_ptr_types.h,
	  pdp4.0/trunk/src/css/css_extern_support.h,
	  pdp4.0/trunk/src/css/css_lex.cpp,
	  pdp4.0/trunk/src/css/css_machine.cpp,
	  pdp4.0/trunk/src/css/css_machine.h,
	  pdp4.0/trunk/src/css/css_main_qt.html,
	  pdp4.0/trunk/src/css/css_misc_funs.cpp,
	  pdp4.0/trunk/src/css/css_misc_funs.h,
	  pdp4.0/trunk/src/css/css_parse.cpp,
	  pdp4.0/trunk/src/css/css_parse.h,
	  pdp4.0/trunk/src/css/css_parse.y,
	  pdp4.0/trunk/src/css/css_qt.cpp, pdp4.0/trunk/src/css/css_qt.h,
	  pdp4.0/trunk/src/css/css_qtdata.cpp,
	  pdp4.0/trunk/src/css/css_qtdata.h,
	  pdp4.0/trunk/src/css/css_qtdialog.cpp,
	  pdp4.0/trunk/src/css/css_qtdialog.h,
	  pdp4.0/trunk/src/css/css_qttype.cpp,
	  pdp4.0/trunk/src/css/css_qttype.h,
	  pdp4.0/trunk/src/css/css_special_math.cpp,
	  pdp4.0/trunk/src/css/css_special_math.doc,
	  pdp4.0/trunk/src/css/css_special_math.h,
	  pdp4.0/trunk/src/css/css_ti.cpp,
	  pdp4.0/trunk/src/css/hard_of_css.h,
	  pdp4.0/trunk/src/css/main.cpp,
	  pdp4.0/trunk/src/css/readline_win.cpp,
	  pdp4.0/trunk/src/css/special_math.old.c,
	  pdp4.0/trunk/src/css/ta_css.cpp, pdp4.0/trunk/src/css/ta_css.h,
	  pdp4.0/trunk/src/css/xmemory.cpp, pdp4.0/trunk/src/leabra,
	  pdp4.0/trunk/src/leabra/leabra.cpp,
	  pdp4.0/trunk/src/leabra/leabra.h,
	  pdp4.0/trunk/src/leabra/leabra_TA.cpp,
	  pdp4.0/trunk/src/leabra/leabra_TA_inst.h,
	  pdp4.0/trunk/src/leabra/leabra_TA_type.h,
	  pdp4.0/trunk/src/leabra/leabra_ti.cpp,
	  pdp4.0/trunk/src/leabra/main.cpp, pdp4.0/trunk/src/leabra/proj,
	  pdp4.0/trunk/src/leabra/proj/objrec.proj.gz,
	  pdp4.0/trunk/src/maketa, pdp4.0/trunk/src/maketa/NoMakeTA,
	  pdp4.0/trunk/src/maketa/maketa.cpp,
	  pdp4.0/trunk/src/maketa/maketa.h,
	  pdp4.0/trunk/src/maketa/maketa.sln,
	  pdp4.0/trunk/src/maketa/maketa.suo,
	  pdp4.0/trunk/src/maketa/maketa.vcproj,
	  pdp4.0/trunk/src/maketa/maketa_TA.cpp,
	  pdp4.0/trunk/src/maketa/maketa_TA_inst.h,
	  pdp4.0/trunk/src/maketa/maketa_TA_type.h,
	  pdp4.0/trunk/src/maketa/maketa_ti.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.cpp,
	  pdp4.0/trunk/src/maketa/mta_constr.h,
	  pdp4.0/trunk/src/maketa/mta_lex.cpp,
	  pdp4.0/trunk/src/maketa/mta_parse.cpp,
	  pdp4.0/trunk/src/maketa/mta_parse.h,
	  pdp4.0/trunk/src/maketa/mta_parse.output,
	  pdp4.0/trunk/src/maketa/mta_parse.y,
	  pdp4.0/trunk/src/maketa/stdafx.h,
	  pdp4.0/trunk/src/maketa/ta_list.cpp,
	  pdp4.0/trunk/src/maketa/ta_list.h,
	  pdp4.0/trunk/src/maketa/ta_platform.cpp,
	  pdp4.0/trunk/src/maketa/ta_platform.h,
	  pdp4.0/trunk/src/maketa/ta_platform_cygwin.cpp,
	  pdp4.0/trunk/src/maketa/ta_platform_unix.cpp,
	  pdp4.0/trunk/src/maketa/ta_platform_win.cpp,
	  pdp4.0/trunk/src/maketa/ta_string.cpp,
	  pdp4.0/trunk/src/maketa/ta_string.h,
	  pdp4.0/trunk/src/maketa/ta_type.cpp,
	  pdp4.0/trunk/src/maketa/ta_type.h,
	  pdp4.0/trunk/src/maketa/test_TA.cpp,
	  pdp4.0/trunk/src/maketa/test_TA_inst.h,
	  pdp4.0/trunk/src/maketa/test_TA_type.h,
	  pdp4.0/trunk/src/maketa/test_maketa.h,
	  pdp4.0/trunk/src/maketa/ti_file,
	  pdp4.0/trunk/src/maketa/updatesrcs.cmd, pdp4.0/trunk/src/pdp,
	  pdp4.0/trunk/src/pdp++.sln, pdp4.0/trunk/src/pdp++.suo,
	  pdp4.0/trunk/src/pdp/enviro.cpp, pdp4.0/trunk/src/pdp/enviro.h,
	  pdp4.0/trunk/src/pdp/enviro_extra.cpp,
	  pdp4.0/trunk/src/pdp/enviro_extra.h,
	  pdp4.0/trunk/src/pdp/enviro_qt.cpp,
	  pdp4.0/trunk/src/pdp/enviro_qt.h,
	  pdp4.0/trunk/src/pdp/enviro_qtso.cpp,
	  pdp4.0/trunk/src/pdp/enviro_qtso.h,
	  pdp4.0/trunk/src/pdp/main.cpp, pdp4.0/trunk/src/pdp/netstru.cpp,
	  pdp4.0/trunk/src/pdp/netstru.h,
	  pdp4.0/trunk/src/pdp/netstru_extra.cpp,
	  pdp4.0/trunk/src/pdp/netstru_extra.h,
	  pdp4.0/trunk/src/pdp/netstru_qtso.cpp,
	  pdp4.0/trunk/src/pdp/netstru_qtso.h,
	  pdp4.0/trunk/src/pdp/netstru_so.cpp,
	  pdp4.0/trunk/src/pdp/netstru_so.h,
	  pdp4.0/trunk/src/pdp/pdp_TA.cpp,
	  pdp4.0/trunk/src/pdp/pdp_TA_inst.h,
	  pdp4.0/trunk/src/pdp/pdp_TA_type.h,
	  pdp4.0/trunk/src/pdp/pdp_qtso.cpp,
	  pdp4.0/trunk/src/pdp/pdp_qtso.h,
	  pdp4.0/trunk/src/pdp/pdp_ti.cpp,
	  pdp4.0/trunk/src/pdp/pdpbase.cpp,
	  pdp4.0/trunk/src/pdp/pdpbase.h,
	  pdp4.0/trunk/src/pdp/pdpbase_unix.cpp,
	  pdp4.0/trunk/src/pdp/pdpbase_win.cpp,
	  pdp4.0/trunk/src/pdp/pdpbitmaps.h,
	  pdp4.0/trunk/src/pdp/pdpdeclare.h,
	  pdp4.0/trunk/src/pdp/pdplog.cpp, pdp4.0/trunk/src/pdp/pdplog.h,
	  pdp4.0/trunk/src/pdp/pdplog_qtso.cpp,
	  pdp4.0/trunk/src/pdp/pdplog_qtso.h,
	  pdp4.0/trunk/src/pdp/pdpshell.cpp,
	  pdp4.0/trunk/src/pdp/pdpshell.h,
	  pdp4.0/trunk/src/pdp/pdpview_qt.cpp,
	  pdp4.0/trunk/src/pdp/pdpview_qt.h,
	  pdp4.0/trunk/src/pdp/process.cpp,
	  pdp4.0/trunk/src/pdp/process.h,
	  pdp4.0/trunk/src/pdp/procs_extra.cpp,
	  pdp4.0/trunk/src/pdp/procs_extra.h,
	  pdp4.0/trunk/src/pdp/sched_proc.cpp,
	  pdp4.0/trunk/src/pdp/sched_proc.h,
	  pdp4.0/trunk/src/pdp/script_base.h,
	  pdp4.0/trunk/src/pdp/spec.cpp, pdp4.0/trunk/src/pdp/spec.h,
	  pdp4.0/trunk/src/pdp/stats.cpp, pdp4.0/trunk/src/pdp/stats.h,
	  pdp4.0/trunk/src/platform.h, pdp4.0/trunk/src/qtdefs.h,
	  pdp4.0/trunk/src/ta, pdp4.0/trunk/src/ta/fstream-maketa.h,
	  pdp4.0/trunk/src/ta/icolor.cpp, pdp4.0/trunk/src/ta/icolor.h,
	  pdp4.0/trunk/src/ta/ifont.cpp, pdp4.0/trunk/src/ta/ifont.h,
	  pdp4.0/trunk/src/ta/ios-g++-3.1,
	  pdp4.0/trunk/src/ta/ios-g++-3.1/fstream.h,
	  pdp4.0/trunk/src/ta/ios-g++-3.1/iostream.h,
	  pdp4.0/trunk/src/ta/ios-g++-3.1/sstream.h,
	  pdp4.0/trunk/src/ta/ios-g++-3.1/streambuf.h,
	  pdp4.0/trunk/src/ta/ios-g++-3.1/strstream.h,
	  pdp4.0/trunk/src/ta/iostream-maketa.h,
	  pdp4.0/trunk/src/ta/sstream-maketa.h,
	  pdp4.0/trunk/src/ta/stdafx.h,
	  pdp4.0/trunk/src/ta/streambuf-maketa.h,
	  pdp4.0/trunk/src/ta/ta.cpp, pdp4.0/trunk/src/ta/ta.h,
	  pdp4.0/trunk/src/ta/ta.vcproj, pdp4.0/trunk/src/ta/ta_TA.cpp,
	  pdp4.0/trunk/src/ta/ta_TA.msvc.cpp,
	  pdp4.0/trunk/src/ta/ta_TA_inst.h,
	  pdp4.0/trunk/src/ta/ta_TA_type.h,
	  pdp4.0/trunk/src/ta/ta_base.cpp, pdp4.0/trunk/src/ta/ta_base.h,
	  pdp4.0/trunk/src/ta/ta_def.h,
	  pdp4.0/trunk/src/ta/ta_defaults.cpp,
	  pdp4.0/trunk/src/ta/ta_defaults.h,
	  pdp4.0/trunk/src/ta/ta_dump.cpp, pdp4.0/trunk/src/ta/ta_dump.h,
	  pdp4.0/trunk/src/ta/ta_filer.cpp,
	  pdp4.0/trunk/src/ta/ta_filer.h,
	  pdp4.0/trunk/src/ta/ta_group.cpp,
	  pdp4.0/trunk/src/ta/ta_group.h, pdp4.0/trunk/src/ta/ta_list.cpp,
	  pdp4.0/trunk/src/ta/ta_list.h,
	  pdp4.0/trunk/src/ta/ta_maketa_defs.h,
	  pdp4.0/trunk/src/ta/ta_math.h,
	  pdp4.0/trunk/src/ta/ta_platform.cpp,
	  pdp4.0/trunk/src/ta/ta_platform.h,
	  pdp4.0/trunk/src/ta/ta_platform_cygwin.cpp,
	  pdp4.0/trunk/src/ta/ta_platform_unix.cpp,
	  pdp4.0/trunk/src/ta/ta_platform_win.cpp,
	  pdp4.0/trunk/src/ta/ta_qt.cpp, pdp4.0/trunk/src/ta/ta_qt.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.cpp,
	  pdp4.0/trunk/src/ta/ta_qtbrowse.h,
	  pdp4.0/trunk/src/ta/ta_qtbrowse_def.h,
	  pdp4.0/trunk/src/ta/ta_qtclipdata.cpp,
	  pdp4.0/trunk/src/ta/ta_qtclipdata.h,
	  pdp4.0/trunk/src/ta/ta_qtcursors.h,
	  pdp4.0/trunk/src/ta/ta_qtdata.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdata.h,
	  pdp4.0/trunk/src/ta/ta_qtdata_def.h,
	  pdp4.0/trunk/src/ta/ta_qtdialog.cpp,
	  pdp4.0/trunk/src/ta/ta_qtdialog.h,
	  pdp4.0/trunk/src/ta/ta_qtgroup.cpp,
	  pdp4.0/trunk/src/ta/ta_qtgroup.h,
	  pdp4.0/trunk/src/ta/ta_qttype.cpp,
	  pdp4.0/trunk/src/ta/ta_qttype.h,
	  pdp4.0/trunk/src/ta/ta_qttype_def.h,
	  pdp4.0/trunk/src/ta/ta_qtviewer.cpp,
	  pdp4.0/trunk/src/ta/ta_qtviewer.h,
	  pdp4.0/trunk/src/ta/ta_script.cpp,
	  pdp4.0/trunk/src/ta/ta_script.h,
	  pdp4.0/trunk/src/ta/ta_seledit.cpp,
	  pdp4.0/trunk/src/ta/ta_seledit.h,
	  pdp4.0/trunk/src/ta/ta_stdef.h,
	  pdp4.0/trunk/src/ta/ta_string.cpp,
	  pdp4.0/trunk/src/ta/ta_string.h, pdp4.0/trunk/src/ta/ta_ti.cpp,
	  pdp4.0/trunk/src/ta/ta_type.cpp, pdp4.0/trunk/src/ta/ta_type.h,
	  pdp4.0/trunk/src/ta/ta_type_constr.cpp,
	  pdp4.0/trunk/src/ta/ta_type_constr.h,
	  pdp4.0/trunk/src/ta/ta_type_qt.cpp,
	  pdp4.0/trunk/src/ta/ta_uifiler_qt.cpp,
	  pdp4.0/trunk/src/ta/ta_uifiler_qt.h,
	  pdp4.0/trunk/src/taglobal.h, pdp4.0/trunk/src/taiqtso,
	  pdp4.0/trunk/src/taiqtso/NoMakeTA,
	  pdp4.0/trunk/src/taiqtso/icheckbox.cpp,
	  pdp4.0/trunk/src/taiqtso/icheckbox.h,
	  pdp4.0/trunk/src/taiqtso/ichooserdialog.cpp,
	  pdp4.0/trunk/src/taiqtso/ichooserdialog.h,
	  pdp4.0/trunk/src/taiqtso/icombobox.cpp,
	  pdp4.0/trunk/src/taiqtso/icombobox.h,
	  pdp4.0/trunk/src/taiqtso/ieditgrid.cpp,
	  pdp4.0/trunk/src/taiqtso/ieditgrid.h,
	  pdp4.0/trunk/src/taiqtso/iflowbox.cpp,
	  pdp4.0/trunk/src/taiqtso/iflowbox.h,
	  pdp4.0/trunk/src/taiqtso/iflowlayout.cpp,
	  pdp4.0/trunk/src/taiqtso/iflowlayout.h,
	  pdp4.0/trunk/src/taiqtso/igeometry.cpp,
	  pdp4.0/trunk/src/taiqtso/igeometry.h,
	  pdp4.0/trunk/src/taiqtso/igroup_so.h,
	  pdp4.0/trunk/src/taiqtso/ihilightbutton.cpp,
	  pdp4.0/trunk/src/taiqtso/ihilightbutton.h,
	  pdp4.0/trunk/src/taiqtso/ilineedit.cpp,
	  pdp4.0/trunk/src/taiqtso/ilineedit.h,
	  pdp4.0/trunk/src/taiqtso/imisc_so.cpp,
	  pdp4.0/trunk/src/taiqtso/imisc_so.h,
	  pdp4.0/trunk/src/taiqtso/irenderarea.cpp,
	  pdp4.0/trunk/src/taiqtso/irenderarea.h,
	  pdp4.0/trunk/src/taiqtso/ispinbox.cpp,
	  pdp4.0/trunk/src/taiqtso/ispinbox.h,
	  pdp4.0/trunk/src/taiqtso/itransformer.cpp,
	  pdp4.0/trunk/src/taiqtso/itransformer.h,
	  pdp4.0/trunk/src/taiqtso/itreeview.cpp,
	  pdp4.0/trunk/src/taiqtso/itreeview.h,
	  pdp4.0/trunk/src/taiqtso/ivcrbuttons.cpp,
	  pdp4.0/trunk/src/taiqtso/ivcrbuttons.h,
	  pdp4.0/trunk/src/taiqtso/iwidgetlist.cpp,
	  pdp4.0/trunk/src/taiqtso/iwidgetlist.h,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.cpp,
	  pdp4.0/trunk/src/taiqtso/safeptr_so.h,
	  pdp4.0/trunk/src/taiqtso/stdafx.h,
	  pdp4.0/trunk/src/taiqtso/taiqtdefs.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso.cpp,
	  pdp4.0/trunk/src/taiqtso/taiqtso.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso.vcproj,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA.cpp,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_inst.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_TA_type.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_def.h,
	  pdp4.0/trunk/src/taiqtso/taiqtso_ti.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowbox.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowbox.h,
	  pdp4.0/trunk/src/taiqtso/test_iflowlayout.cpp,
	  pdp4.0/trunk/src/taiqtso/test_iflowlayout.h,
	  pdp4.0/trunk/src/taiqtso/test_ispinbox.cpp,
	  pdp4.0/trunk/src/taiqtso/test_ispinbox.h,
	  pdp4.0/trunk/src/taiqtso/test_itreeview.cpp,
	  pdp4.0/trunk/src/taiqtso/test_itreeview.h,
	  pdp4.0/trunk/src/taiqtso/test_main.cpp,
	  pdp4.0/trunk/src/taiqtso/test_mainwindow.cpp,
	  pdp4.0/trunk/src/taiqtso/test_mainwindow.h,
	  pdp4.0/trunk/src/taiqtso/test_simplewidgets.cpp,
	  pdp4.0/trunk/src/taiqtso/test_simplewidgets.h,
	  pdp4.0/trunk/src/taiqtso/vcrbitmaps.h, pdp4.0/trunk/src/tamisc,
	  pdp4.0/trunk/src/tamisc/aggregate.cpp,
	  pdp4.0/trunk/src/tamisc/aggregate.h,
	  pdp4.0/trunk/src/tamisc/axis.cpp,
	  pdp4.0/trunk/src/tamisc/axis.h,
	  pdp4.0/trunk/src/tamisc/colorbar_qt.cpp,
	  pdp4.0/trunk/src/tamisc/colorbar_qt.h,
	  pdp4.0/trunk/src/tamisc/colorbar_so.cpp,
	  pdp4.0/trunk/src/tamisc/colorbar_so.h,
	  pdp4.0/trunk/src/tamisc/colorscale.cpp,
	  pdp4.0/trunk/src/tamisc/colorscale.h,
	  pdp4.0/trunk/src/tamisc/datagraph.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph.h,
	  pdp4.0/trunk/src/tamisc/datagraph_qtso.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph_qtso.h,
	  pdp4.0/trunk/src/tamisc/datagraph_so.cpp,
	  pdp4.0/trunk/src/tamisc/datagraph_so.h,
	  pdp4.0/trunk/src/tamisc/datatable.cpp,
	  pdp4.0/trunk/src/tamisc/datatable.h,
	  pdp4.0/trunk/src/tamisc/datatable_qtso.cpp,
	  pdp4.0/trunk/src/tamisc/datatable_qtso.h,
	  pdp4.0/trunk/src/tamisc/fontspec.cpp,
	  pdp4.0/trunk/src/tamisc/fontspec.h,
	  pdp4.0/trunk/src/tamisc/fun_lookup.cpp,
	  pdp4.0/trunk/src/tamisc/fun_lookup.h,
	  pdp4.0/trunk/src/tamisc/graphic.cpp,
	  pdp4.0/trunk/src/tamisc/graphic.h,
	  pdp4.0/trunk/src/tamisc/graphic_objs.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_objs.h,
	  pdp4.0/trunk/src/tamisc/graphic_text.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_text.h,
	  pdp4.0/trunk/src/tamisc/graphic_viewer.cpp,
	  pdp4.0/trunk/src/tamisc/graphic_viewer.h,
	  pdp4.0/trunk/src/tamisc/minmax.cpp,
	  pdp4.0/trunk/src/tamisc/minmax.h,
	  pdp4.0/trunk/src/tamisc/t3node_so.cpp,
	  pdp4.0/trunk/src/tamisc/t3node_so.h,
	  pdp4.0/trunk/src/tamisc/t3viewer.cpp,
	  pdp4.0/trunk/src/tamisc/t3viewer.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_inst.h,
	  pdp4.0/trunk/src/tamisc/tamisc_TA_type.h,
	  pdp4.0/trunk/src/tamisc/tamisc_maketa_defs.h,
	  pdp4.0/trunk/src/tamisc/tamisc_stdef.h,
	  pdp4.0/trunk/src/tamisc/tamisc_test.cpp,
	  pdp4.0/trunk/src/tamisc/tamisc_ti.cpp,
	  pdp4.0/trunk/src/tamisc/tarandom.cpp,
	  pdp4.0/trunk/src/tamisc/tarandom.h,
	  pdp4.0/trunk/src/tamisc/tdgeometry.cpp,
	  pdp4.0/trunk/src/tamisc/tdgeometry.h,
	  pdp4.0/trunk/src/tamisc/xform.cpp,
	  pdp4.0/trunk/src/tamisc/xform.h, pdp4.0/trunk/src/test,
	  pdp4.0/trunk/src/test/test.cpp, pdp4.0/trunk/src/test/test.h,
	  pdp4.0/trunk/src/test/test_TA.cpp,
	  pdp4.0/trunk/src/test/test_TA_inst.h,
	  pdp4.0/trunk/src/test/test_TA_type.h,
	  pdp4.0/trunk/src/test/test_maketa.h,
	  pdp4.0/trunk/src/test/test_ti.cpp, pdp4.0/trunk/src/todo,
	  pdp4.0/trunk/src/todo/todo.txt, pdp4.0/trunk/src/utils,
	  pdp4.0/trunk/src/utils/lef, pdp4.0/trunk/src/utils/lef/Debug,
	  pdp4.0/trunk/src/utils/lef/Debug/lef.exe,
	  pdp4.0/trunk/src/utils/lef/Debug/test1.txt,
	  pdp4.0/trunk/src/utils/lef/ReadMe.txt,
	  pdp4.0/trunk/src/utils/lef/lef.cpp,
	  pdp4.0/trunk/src/utils/lef/lef.vcproj,
	  pdp4.0/trunk/src/utils/lef/stdafx.cpp,
	  pdp4.0/trunk/src/utils/lef/stdafx.h,
	  pdp4.0/trunk/src/utils/lef/test2.txt,
	  pdp4.0/trunk/src/utils/utils.sln,
	  pdp4.0/trunk/src/utils/utils.suo: initial load

* Sat Jul 23 2005 05:17 root

- pdp4.0/branches, pdp4.0/tags, pdp4.0/trunk:

[Please see the ChangeLog file for older changes] - Ed.
