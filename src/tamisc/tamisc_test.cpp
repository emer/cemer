// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.



// C^c C Super Script

#include "css_machine.h"
#include "css_builtin.h"
#include "css_basic_types.h"
#include "ta_base.h"
#include "ta_css.h"
#include "css_qt.h"

#include "ta_qt.h"
#include "win_base.h"
#include "ta_misc_TA_type.h"
//#include "ta_qt_test_TA_type.h"

//extern void ta_Init_ta_qt_test();


#include <malloc.h>

extern int yydebug;


/* TODO: ???
#ifdef GUI_QT
static qtOptionDesc options[] = {
    { NULL }
};

static qtPropertyData app_defs[] = {
  {"css*gui", "sgimotif"},
  {"css*PopupWindow*overlay", "true"},
  {"css*PopupWindow*saveUnder", "on"},
  {"css*TransientWindow*saveUnder", "on"},
  {"css*double_buffered",	"on"},
  {"css*flat",			"#c0c4d3"},
  {"css*background",  		"#70c0d8"},
  {"css*name*flat",		"#70c0d8"},
  {"css*apply_button*flat",	"#c090b0"},
  {"css*FieldEditor*background", "white"},
  {"css*FileChooser*filter", 	"on"},
  {"css*FileChooser.rows", 	"20"},
  {"css*FileChooser.width", 	"300"},
  {"css*taivObjChooser.width", "300"},
  {"css*taivObjChooser.rows",	"20"},
  {"css*PaletteButton*minimumWidth", "72.0"},
  {"css*PushButton*minimumWidth", "72.0"},
  {"css*TaIVButton*SmallWidth", "46.0"},
  {"css*TaIVButton*MediumWidth", "72.0"},
  {"css*TaIVButton*BigWidth", "115.0"},
  {"css*toggleScale",		"1.5"},
#ifndef CYGWIN
  {"css*font",			"*-helvetica-medium-r-*-*-10*"},
  {"css*name*font",		"*-helvetica-medium-r-*-*-10*"},
  {"css*title*font",		"*-helvetica-bold-r-*-*-10*"},
  {"css*small_menu*font",	"*-helvetica-medium-r-*-*-10*"},
  {"css*small_submenu*font",	"*-helvetica-medium-r-*-*-10*"},
  {"css*big_menu*font",		"*-helvetica-medium-r-*-*-12*"},
  {"css*big_submenu*font",	"*-helvetica-medium-r-*-*-12*"},
  {"css*big_menubar*font",	"*-helvetica-bold-r-*-*-14*"},
  {"css*big_italic_menubar*font","*-helvetica-bold-o-*-*-14*"},
#else
  {"css*font",			"*Arial*medium*--10*"},
  {"css*name*font",		"*Arial*medium*--10*"},
  {"css*title*font",		"*Arial*bold*--10*"},
  {"css*small_menu*font",	"*Arial*medium*--10*"},
  {"css*small_submenu*font",	"*Arial*medium*--10*"},
  {"css*big_menu*font",		"*Arial*medium*--12*"},
  {"css*big_submenu*font",	"*Arial*medium*--12*"},
  {"css*big_menubar*font",	"*Arial*bold*--14*"},
  {"css*big_italic_menubar*font","*Arial*italic*--14*"},
  // following are def'd in smf_kit.cpp
  {"css*MenuBar*font", 		"*Arial*bold*--12*"},
  {"css*MenuItem*font", 	"*Arial*bold*--12*"},
  {"css*MenuBar*font", 		"*Arial*bold*--12*"},
  {"css*MenuItem*font", 	"*Arial*bold*--12*"},
  // this sets the scaling of the windows to 1.25 -- much closer to unix pdp sizing
  {"css*mswin_scale",		"1.25"},
#endif
  { NULL }
};
#endif // GUI_IV
*/
void css_cleanup(int err) {
  signal(err, SIG_DFL);
  if((err == SIGALRM) || (err == SIGUSR1) || (err == SIGUSR2)) {
    cerr << "css: received a signal: ";
    taMisc::Decode_Signal(err);
    cerr << "\n";
    taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) css_cleanup);
    return;
  }
  cerr << "css: exiting and cleaning up temp files from signal: ";
  taMisc::Decode_Signal(err);
  cerr << "\n";
#ifdef GUI_QT
  taiMisc::Cleanup(err);
#endif // GUI
  kill(getpid(), err);		// activate signal
}


int main(int argc, const char* argv[]) {
  cssMisc::PreInitialize(argc, argv); // creates QApplication and event loop -- these
  // must get created first, and are needed even if we don't open a gui

#ifdef GUI
  ta_Init_ta_misc();	// includes call to ta_Init_ta();
#else
  ta_Init_ta();
#endif // GUI
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) css_cleanup);

  cssMisc::Initialize(); // note: also deals with gui/nogui switches and logic

#ifdef GUI_QT
  taiMisc::Initialize(cssMisc::gui);
//  init_test();
//  cssiSession::in_session = true;
#endif
// TODO: should raise an error if NO_GUI and gui=true

  yydebug = 0;

  WinBase* root = new WinBase();
  tabMisc::root = (TAPtr)root;
  root->OpenNewWindow();
  taMisc::default_scope = &TA_WinBase;

  cssMisc::Top->StartupShell(cin, cout);
//TODO: the following line was never needed before... but types don't seem to be destroying now
// which was the mechanism for setting the flag...
  taMisc::not_constr = true;
  taMisc::types.RemoveAll();	// get rid of types before global dtor!
  return 0;
}

