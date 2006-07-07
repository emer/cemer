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
#include "ta_css.h"

#ifdef TA_GUI
#include "ta_qt.h"
#include "ta_TA_type.h" //NOTE: this will be an issue building Qt phase1 BA
#include "css_qt.h"
#include "ta_qtdata.h" // for taiObjChooser
#include "ta_qtviewer.h"
#include "ta_qtbrowse.h"
#include <QApplication>
#endif // TA_GUI

#ifdef TA_USE_INVENTOR
  #include <Inventor/Qt/SoQt.h>
#endif

#include <QCoreApplication>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

//#include <malloc.h>

extern int yydebug;

#ifdef GUI_IV
static ivOptionDesc options[] = {
    { NULL }
};

static ivPropertyData app_defs[] = {
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
#ifdef TA_GUI
//TODO:  taqtMisc::Cleanup(err);
#endif // GUI_IV
  kill(getpid(), err);		// activate signal
}

int main(int argc, char *argv[]) {
/* following creates QApplication and event loop -- these
   must get created first, and are needed even if we don't open a gui
   Set cssMisc::gui, according to command line switch and compilation mode (TA_GUI/NO_GUI)
*/
  cssMisc::PreInitialize(argc, argv);

  ta_Init_ta();
  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) css_cleanup);

  int rval = cssMisc::Initialize();
  if (rval != 0) return rval;

#ifdef TA_GUI
  cssMisc::gui = false;		// default for css is always false!
#else
  cssMisc::gui = false;
#endif // TA_GUI

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      String tmp = argv[i];
      if (tmp == "-gui")
	cssMisc::gui = true;
      else if (tmp == "-nogui")
	cssMisc::gui = false;
    }
  }

// TODO: should raise an error if NO_GUI and gui=true
#ifdef TA_GUI
  // todo: needs to be done later -- this code is from pdpbase.cpp
  if(cssMisc::gui && (taMisc::dmem_proc == 0)) {
    taiM_ = taiMisc::New(cssMisc::gui);
    taiMC_ = taiM_;
//     taiM->icon_bitmap = new QBitmap(pdp_bitmap_width,
//     	pdp_bitmap_height, pdp_bitmap_bits);
//    qApp->setWindowIcon(QIcon(*(taiM->icon_bitmap)));
/*    root->WinInit();
    root->OpenNewWindow();
    taiMisc::SetMainWindow(root->window); */
    //NOTE: we root the browser at the projects, to minimize unnecessary browsing levels
    // root.colorspecs is aliased in root->colorspecs, as a HIDDEN linked variable
    //    DataBrowser* db = DataBrowser::New(&taMisc::types, NULL, taMisc::types.GetTypeDef(), true);
    //    db->InitLinks(); // no one else to do it!
    //    db->ViewWindow();
//    bw->browser->setRoot(root, root->GetTypeDef());
//    taiMisc::SetMainWindow(db->browser_win());
  }
#else  // NO TA_GUI
  if (gui) {
    cerr << "Cannot specify 'gui' flag when not compiled for gui.\n ";
    return 1;
  }
  taiMC_ = taiMiscCore::New();
#endif  // TA_GUI
  yydebug = 0;

  cssMisc::TopShell->StartupShellInit(cin, cout);

#ifdef TA_GUI
  if(cssMisc::gui) {
    cssMisc::TopShell->Shell_Gui_Console("css");
    qApp->exec();
  }
  else
#endif
    {
      cssMisc::TopShell->Shell_NoGui_Rl("css");
    }

  // after this point, we have dropped out of the main event loop and are quitting!
#ifdef TA_GUI
#ifdef TA_USE_INVENTOR
  SoQt::done();
#endif
#endif

  //note: new 4.0 behavior is for root deletion to be the app end, so following is probably redundant
  if (tabMisc::root) {
    delete tabMisc::root;
    tabMisc::root = NULL;
  }
#ifdef TA_GUI
  taiMisc::RunPending(); // do Qt defered deletes, if any
#endif
  taMisc::types.RemoveAll();	// get rid of types before global dtor!
  return 0;
}




