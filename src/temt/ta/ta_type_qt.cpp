// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

// Copyright (C) 1995-2007 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// ta_type qt implementations

#include "ta_type.h"


#include "ta_qt.h"
#include "ta_qttype.h"
//#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "css_qt.h"
#include "ta_program.h"

#include "css_machine.h"	// for setting error code in taMisc::Error

void taMisc::Error(const char* a, const char* b, const char* c, const char* d,
  const char* e, const char* f, const char* g, const char* h, const char* i)
{
  ++err_cnt;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  //TODO: should log errors on nodes > 0!!!
  if(taMisc::dmem_proc > 0) return;
#endif
  taMisc::last_err_msg = SuperCat(a, b, c, d, e, f, g, h, i);
  String fmsg = "***ERROR: " + taMisc::last_err_msg;
  taMisc::LogEvent(fmsg);
#if !defined(NO_TA_BASE)
  if(taMisc::ErrorCancelCheck()) {
    return;			// cancel!
  }
#endif
  // we always output to console
  if (beep_on_error) cerr << '\a'; // BEL character
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top) {
    taMisc::last_err_msg += String("\n") + cssMisc::GetSourceLoc(NULL);
  }
#endif
  cerr << fmsg << endl;
  FlushConsole();
#if !defined(NO_TA_BASE) 
  if(cssMisc::cur_top) {
    if(cssMisc::cur_top->own_program) {
      bool running = cssMisc::cur_top->state & cssProg::State_Run;
      cssMisc::cur_top->own_program->taError(cssMisc::GetSourceLn(NULL), running,
					      taMisc::last_err_msg);
    }
    cssMisc::cur_top->run_stat = cssEl::ExecError; // tell css that we've got an error
    cssMisc::cur_top->exec_err_msg = taMisc::last_err_msg;
  }
  if (taMisc::gui_active) {
    bool cancel = taiChoiceDialog::ErrorDialog(NULL, taMisc::last_err_msg);
    taMisc::ErrorCancelSet(cancel);
  }
#endif
}

int taMisc::Choice(const char* text, const char* a, const char* b, const char* c,
  const char* d, const char* e, const char* f, const char* g, const char* h, const char* i)
{
  int m=-1;
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if(taMisc::dmem_proc > 0) return -1;
#endif
#if !defined(NO_TA_BASE)
  if (taMisc::gui_active) {
    String delimiter = taiChoiceDialog::delimiter;
    int   chn = 0;
    String chstr = delimiter;
    if (a) { chstr += String(a) + delimiter; chn++; }
    if (b) { chstr += String(b) + delimiter; chn++; }
    if (c) { chstr += String(c) + delimiter; chn++; }
    if (d) { chstr += String(d) + delimiter; chn++; }
    if (e) { chstr += String(e) + delimiter; chn++; }
    if (f) { chstr += String(f) + delimiter; chn++; }
    if (g) { chstr += String(g) + delimiter; chn++; }
    if (h) { chstr += String(h) + delimiter; chn++; }
    if (i) { chstr += String(i) + delimiter; chn++; }
    m = taiChoiceDialog::ChoiceDialog(NULL, text, chstr);
  } else
#endif
  {
    int   chn = 0;
    String chstr = text;
    chstr += "\n";
    if (a) { chstr += String("0: ") + a + "\n"; chn++; }
    if (b) { chstr += String("1: ") + b + "\n"; chn++; }
    if (c) { chstr += String("2: ") + c + "\n"; chn++; }
    if (d) { chstr += String("3: ") + d + "\n"; chn++; }
    if (e) { chstr += String("4: ") + e + "\n"; chn++; }
    if (f) { chstr += String("5: ") + f + "\n"; chn++; }
    if (g) { chstr += String("6: ") + g + "\n"; chn++; }
    if (h) { chstr += String("7: ") + h + "\n"; chn++; }
    if (i) { chstr += String("8: ") + i + "\n"; chn++; }

    int   choiceval = -1;
    while((choiceval < 0) ||  (choiceval > chn) ) {
      cout << chstr;
      String choice;
      cin >> choice;
      choiceval = atoi(choice);
    }
    m = choiceval;
  }
  return m;
}

void taMisc::Confirm(const char* a, const char* b, const char* c,
  const char* d, const char* e, const char* f, const char* g,
  const char* h, const char* i)
{
#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  if (taMisc::dmem_proc > 0) return;
#endif
  String msg = SuperCat(a, b, c, d, e, f, g, h, i);
  taMisc::LogEvent("***CONFIRM: " + msg);
#if !defined(NO_TA_BASE)
  if (taMisc::gui_active) {
    taiChoiceDialog::ConfirmDialog(NULL, msg);
  } else
#endif
  {
    cout << msg << "\n";
    FlushConsole();
  }
}
