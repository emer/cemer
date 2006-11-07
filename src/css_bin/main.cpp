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

#include "ta_project.h"
#include "css_machine.h"

#ifdef TA_GUI
#endif // TA_GUI

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

int main(int argc, const char *argv[]) {
  cssMisc::prompt = "css> ";
  taMisc::use_gui = true;	// set opposite default from normal

  if(!taRootBase::Startup_Main(argc, argv, ta_Init_ta, &TA_taRootBase)) return 1;

  taMisc::Register_Cleanup((SIGNAL_PROC_FUN_TYPE) css_cleanup);
  if(taRootBase::Startup_Run())
    return 0;
  else
    return 2;
}
