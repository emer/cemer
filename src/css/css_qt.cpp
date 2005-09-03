/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/


// These are some routines to handle simultaneous use of css and interviews

#include "css_qt.h"
#include "css_qtdialog.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "ta_qt.h"

#include <qapplication.h>
#include <qeventloop.h>

#ifdef TA_USE_INVENTOR
  #include <Inventor/Qt/SoQt.h>
#endif

#include <stdlib.h>

extern "C" {
  extern int readline_waitproc(void);
  extern int (*rl_event_hook)(void);	// this points to the waitproc if running Qt
  extern int rl_done;			// set this to kill readline..
}

int readline_waitproc() {
  return cssiSession::Run();
}

//////////////////////////////////
// 	iSession		//
//////////////////////////////////

class iSession: public QEventLoop {
public:
  static iSession* m_instance;
  static iSession* instance();

  bool m_done;

  iSession(QObject *parent = 0, const char *name = 0);
  ~iSession();

  bool processEvents() {return QEventLoop::processEvents(QEventLoop::AllEvents);}

  override void exit(int retcode = 0); // -- called by Qt if it gets a Quit message
  virtual void quit();
  virtual bool done() const {return m_done;}
};


/*
Note that this object simply replicates the semantics and naming of the Iv Session
object -- this was done to make porting the css/ta code much easier, since the
event loop processing has many layers.
*/

iSession* iSession::m_instance = NULL;

iSession* iSession::instance() {
  return m_instance;
}

iSession::iSession(QObject *parent, const char *name)
: QEventLoop(parent, name) //NOTE: QEventLoop handles the (disallowed) case of multiple creation
{
  m_done = false;
  m_instance = this;
}

iSession::~iSession() {
  m_instance = NULL;
}

void iSession::exit(int retcode) {
  // override -- called by Qt if it gets a Quit message
  if (!m_done) {
    m_done = true;
    QEventLoop::exit(retcode); // sets the exit flag, for compatability
    //following is what QEventLoop does when it gets a Quit message in the proper event loop (that we don't run)
    // note however that we don't need the aboutToQuit() signal, and Coin/Inventor does not use it either
//    ((iApplication*)qApp)->emit_aboutToQuit();
  if (cssMisc::cur_top)
    cssMisc::cur_top->ExitShell(); //forces exit from shell
    QApplication::sendPostedEvents( 0, QEvent::DeferredDelete );
  }
}

void iSession::quit() {
  exit(0);
}


//////////////////////////////////
// 	cssMisc		//
//////////////////////////////////

void cssMisc::PreInitialize(int argc_, char** argv_) {
  cssMisc::argc = argc_;
  cssMisc::argv = argv_; // cast away constness -- we still treat it as const
  new iSession(); // aka QEventLoop -- accessed as iSession::instance()
#ifdef TA_USE_INVENTOR
//use other version  SoQt::init((QWidget*)NULL); // creates a special Coin QApplication instance
  SoQt::init(argc, argv, prompt.chars()); // creates a special Coin QApplication instance
//note: must still set SoQtP.h:SoQtP::mainwidget to the main widget, before calling event loop
#else
  new QApplication(argc, (char**)argv); // accessed as qApp
#endif
}

bool cssiSession::block_stdin = false;
bool cssiSession::done_busy = false;
bool cssiSession::in_session = false;
bool cssiSession::quitting = false;
int (*cssiSession::WaitProc)() = NULL; // user sets this to a work process


void cssiSession::Init() {
  rl_event_hook = readline_waitproc; // set the hook to our "waitproc"
  rl_done = false;


// TODO: // the link function first arg spec's the file desc. index
  // and 0 == stdin.  Thus, this is handling stdin.
//  Dispatcher::instance().link(0, Dispatcher::ReadMask, instance);
//  Dispatcher::instance().link(0, Dispatcher::ExceptMask, instance);
}

void cssiSession::Stop() {
  in_session = false;
}

void cssiSession::Quit() {
  in_session = false;
  iSession::instance()->quit();  //TODO: verify works
}

//TODO: what is the meaning of the return value??? (used in return to readline callback
int cssiSession::Run() {
//  stdin_event = false;
  in_session = true;

  int post_wait_proc = false;

  iSession* ses = iSession::instance();

  if (//!ses->done() &&
  in_session && !rl_done) {
    ses->processEvents();

    if (//!ses->done() &&
    in_session && !rl_done) {
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

  in_session = false; 	// make sure its false
  return 0;
}

// returns if it was stopped or not..

int cssiSession::RunPending() {
//obs  stdin_event = false;
  in_session = true;

  iSession* ses = iSession::instance();

  /* Note: call to hasPendingEvents() may not be relevant for Qt, since it processes
    all pending events in the call to processEvents() -- the Iv version used to make
    a call to process a single event, where it then made sense */
  if (ses->hasPendingEvents() && !ses->done() && cssiSession::in_session) {
    ses->processEvents();
  }
  return (ses->done() || !(cssiSession::in_session));
}

/* not used???
int cssiSession::inputReady(int) {
  stdin_event = true;	// set the flag
  if(!block_stdin)
    Stop();		// break from whatever loop might be running..
  return 0;
}

int cssiSession::outputReady(int) {
  stdin_event = true;	// set the flag
  if(!block_stdin)
    Stop();		// break from whatever loop might be running..
  return 0;
}

// this is apparently if some problem occurs, not sure how to deal with it
// at this point..
int cssiSession::exceptionRaised(int) {
  stdin_event = true;
  Stop();
  taMisc::Error("exception raised on stdin in cssiSession");
  return 0;
} */


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
  for (i=0;i<10;i++)
    RunPending();
  for (i=taiMisc::css_active_edits.size-1; i>=0; i--) {
    cssiEditDialog* dlg = (cssiEditDialog*)taiMisc::css_active_edits.FastEl(i);
    if (dlg->state == taiDataHost::ACTIVE)
      dlg->Raise();
  }
  for (i=0;i<10;i++)
    RunPending();
}


