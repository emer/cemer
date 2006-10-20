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
#include "css_console.h"
#include "ta_css.h"

#ifdef TA_GUI
#include "ta_qt.h"
#include "ta_TA_type.h" //NOTE: this will be an issue building Qt phase1 BA
#include "css_qt.h"
#include "ta_qtdata.h" // for taiObjChooser
#include "ta_qtviewer.h"
// #include "ta_qtbrowse.h"
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
  cssMisc::gui = true; // false;		// default for css is always false! (except when debugging!)
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

#ifdef TA_GUI
  if(cssMisc::gui && (taMisc::dmem_proc == 0)) {
    taiM_ = taiMisc::New(cssMisc::gui);
    taiMC_ = taiM_;
//   QApplication a( argc, argv );

    //Create and show the main window
    QMainWindow* mw = new QMainWindow(0, "Application window");
    mw->setMinimumSize(640, 720);
    // todo: I don't see any way to set a preferred size different from min size..
//     mw->setBaseSize(640, 720);
    QcssConsole* console = QcssConsole::getInstance(mw, cssMisc::TopShell);
    mw->setFocusProxy((QWidget*)console);
    mw->setCentralWidget((QWidget*)console);
    qApp->setMainWidget(mw);	// todo: deprecated version
    mw->show();
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_Qt_Console);
  }
  
#else  // NO TA_GUI
  if(cssMisc::gui) {
    cerr << "Cannot specify 'gui' flag when not compiled for gui.\n ";
    return 1;
  }
  taiMC_ = taiMiscCore::New();
#endif  // TA_GUI
  if(!cssMisc::gui || (taMisc::dmem_proc > 0)) {
    cssMisc::TopShell->StartupShellInit(cin, cout, cssCmdShell::CT_NoGui_Rl);
  }

  yydebug = 0;

#ifdef TA_GUI
  if(cssMisc::gui && (taMisc::dmem_proc == 0)) {
    cssMisc::TopShell->Shell_Qt_Console("css> ");
    qApp->exec();
  }
  else
#endif
    {
      cssMisc::TopShell->Shell_NoGui_Rl("css> ");
    }

  // after this point, we have dropped out of the main event loop and are quitting!
#ifdef TA_GUI
#ifdef TA_USE_INVENTOR
  SoQt::done();
#endif
#endif

  //note: new 4.0 behavior is for root deletion to be the app end, so following is probably redundant
//   if (tabMisc::root) {
//     delete tabMisc::root;
//     tabMisc::root = NULL;
//   }
#ifdef TA_GUI
  taiMisc::RunPending(); // do Qt defered deletes, if any
#endif
  taMisc::types.RemoveAll();	// get rid of types before global dtor!
  return 0;
}




