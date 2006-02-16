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

#ifdef TA_GUI
#include <QApplication>
#else
#include <QCoreApplication>
#endif
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

  iSession(QObject *parent = 0);
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

iSession::iSession(QObject *parent)
: QEventLoop(parent) //NOTE: QEventLoop handles the (disallowed) case of multiple creation
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
#ifdef TA_GUI
#ifdef TA_USE_INVENTOR
//use other version  SoQt::init((QWidget*)NULL); // creates a special Coin QApplication instance
  SoQt::init(argc, argv, prompt.chars()); // creates a special Coin QApplication instance
//note: must still set SoQtP.h:SoQtP::mainwidget to the main widget, before calling event loop
#else
  new QApplication(argc, (char**)argv); // accessed as qApp
#endif
#else
  new QCoreApplication(argc, (char**)argv); // accessed as qApp
#endif
//NOTE: in Qt3 we had to create the EventLoop prior to application,
//  but in Qt4, it complains; also, Application now creates its own event loop in exec()
  new iSession(); // aka QEventLoop -- accessed as iSession::instance()
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
    // keep processing events semi-indefinitely while more to do, but don't
    // let ourself get locked into a loop we can't quit from 
    // note that processing more events has higher priority than exiting,
    // so that things like defered deletes get done 
    int max_loops = 50;
    while ((max_loops-- > 0) && QCoreApplication::hasPendingEvents()) {
      ses->processEvents();
    }

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
//Qt3  if (ses->hasPendingEvents() && !ses->done() && cssiSession::in_session) {
  if (!ses->done() && cssiSession::in_session) {
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


