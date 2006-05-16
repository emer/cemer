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



// These are some routines to handle simultaneous use of css and interviews
#include "css_qt.h"

#include "css_qtdialog.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "ta_qt.h"

void cssiSession::CancelProgEdits(cssProgSpace* prsp) {
  int i;
  for (i=taiMisc::css_active_edits.size-1; i>=0; i--) {
    cssiEditDialog* dlg = (cssiEditDialog*)taiMisc::css_active_edits.FastEl(i);
    if ((dlg->state == taiDataHost::ACTIVE) && ((dlg->top == prsp) || (dlg->top == NULL)))
      dlg->Cancel();
  }
  taiMisc::PurgeDialogs();
}

void cssiSession::CancelClassEdits(cssClassType* cltyp) {
  int i;
  for (i=taiMisc::css_active_edits.size-1; i>=0; i--) {
    cssiEditDialog* dlg = (cssiEditDialog*)taiMisc::css_active_edits.FastEl(i);
    if ((dlg->state == taiDataHost::ACTIVE) && (dlg->obj->type_def == cltyp))
      dlg->Cancel();
  }
  taiMisc::PurgeDialogs();
}

void cssiSession::CancelObjEdits(cssClassInst* clobj) {
  int i;
  for (i=taiMisc::css_active_edits.size-1; i>=0; i--) {
    cssiEditDialog* dlg = (cssiEditDialog*)taiMisc::css_active_edits.FastEl(i);
    if ((dlg->state == taiDataHost::ACTIVE) && (dlg->obj == clobj))
      dlg->Cancel();
  }
  taiMisc::PurgeDialogs();
}

void cssiSession::RaiseObjEdits() {
  int i;
  taiMiscCore::RunPending();
  for (i=taiMisc::css_active_edits.size-1; i>=0; i--) {
    cssiEditDialog* dlg = (cssiEditDialog*)taiMisc::css_active_edits.FastEl(i);
    if (dlg->state == taiDataHost::ACTIVE)
      dlg->Raise();
  }
  taiMiscCore::RunPending();
}

/*
#include "css_qtdialog.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "ta_qt.h"

#ifdef TA_GUI
#include <QApplication>
#endif

#include <QCoreApplication>
#include <QEvent>

#ifdef TA_USE_INVENTOR
  #include <Inventor/Qt/SoQt.h>
#endif

#include <stdlib.h>

extern "C" {
  extern int readline_waitproc(void);
  extern int (*rl_event_hook)(void);	// this points to the waitproc if running Qt
  extern int rl_done;			// set this to kill readline..
}



//////////////////////////////////
// 	cssMisc		//
//////////////////////////////////

bool cssiSession::block_stdin = false;
bool cssiSession::done_busy = false;
bool cssiSession::in_session = false;
bool cssiSession::quitting = false;
int (*cssiSession::WaitProc)() = NULL; // user sets this to a work process

int cssiSession::readline_waitproc() {
  return Run();
}

int cssiSession::RunPending() {
  in_session = true;

  if (!QCoreApplication::closingDown() && in_session && !quitting) {
    QCoreApplication::processEvents();
  }
  return (QCoreApplication::closingDown() || !in_session || !quitting);
}


void cssiSession::Init() {
  //note: init of waitproc was done in cssMisc::Preinitialize
}

void cssiSession::Stop() {
  in_session = false;
}

void cssiSession::Quit() {
  in_session = false;
  quitting = true;
  qApp->quit();  //TODO: verify works
}

//TODO: what is the meaning of the return value??? (used in return to readline callback
int cssiSession::Run() {
//  stdin_event = false;
  in_session = true;

  int post_wait_proc = false;

  if (//!ses->done() &&
  in_session && !rl_done) {
    // this processes events until there are no more to process
    QCoreApplication::processEvents();

    if (in_session && !rl_done && !quitting) {
      int rval = false;
      if (WaitProc != NULL) 	// then its safe to do some work
	rval = (*WaitProc)();
      if (!rval && post_wait_proc) {
	// do special things that can only be done post wait proc..
	post_wait_proc = false;
      }
      else
	post_wait_proc = rval;
      if (!rval && done_busy) {
	taiMisc::DoneBusy_impl();
	done_busy = false;
      }

      // loops down into a next shell..
      // this is typically called from tascript.cc when user requests interactive editing
      if (cssMisc::next_shell != NULL) {
	cssProgSpace* nxt_shl = cssMisc::next_shell;
	cssMisc::next_shell = NULL;
	nxt_shl->CtrlShell(*(nxt_shl->fin), *(nxt_shl->fout), nxt_shl->name);
      }
    }
  }
  if (quitting && cssMisc::cur_top) {
    cssMisc::cur_top->ExitShell(); //forces exit from shell
    QCoreApplication::sendPostedEvents( 0, QEvent::DeferredDelete );
  }

  in_session = false; 	// make sure its false
  return 0;
}

// returns if it was stopped or not..


*/
