// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef taiMiscCore_h
#define taiMiscCore_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#include <QEvent>
#else
class QEvent; //
#endif

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class QTimer; //

// note: because this defines CancelOp, it must be TA parsed -- otherwise it would not be

enum CancelOp { // ops for passing cancel status and instructions, typically for Window closing
  CO_PROCEED,           // tells caller to proceed with operation (typical default)
  CO_CANCEL,            // client can set this to tell caller to cancel the operation
  CO_NOT_CANCELLABLE    // preset, to tell client that operation will go ahead unconditionally
};

taTypeDef_Of(taiMiscCore);

class TA_API taiMiscCore: public QObject {
  // ##NO_TOKENS ##NO_INSTANCE object for Qt Core event processing, etc. taiMisc inherits; taiM is always instance
INHERITED(QObject)
  Q_OBJECT
friend class TypeDef; // for the post_load guy
public:
#ifndef __MAKETA__
  enum CustomEvents {
    CE_QUIT             = QEvent::User + 1 // sent from RootWin::CloseEvent to avoid issues
  };
#endif
  static int            rl_callback(); // a readline-compatible callback -- calls event loop

  static taiMiscCore*   New(QObject* parent = NULL);
  // #IGNORE either call this or call taiMisc::New

  static void           WaitProc(); // the core idle loop process

  static int            ProcessEvents();
  // run any pending qt events that might need processed
  static int            RunPending();
  // check to see if any events are pending, and run if true -- MUCH faster than processevents, but also likely to miss some events along the way.

  static void           Quit(CancelOp cancel_op = CO_NOT_CANCELLABLE);
   // call to quit, invokes Quit_impl on instance first
  static void           OnQuitting(CancelOp& cancel_op); // call this when a quit situation is detected -- does all the save logic

  const String          classname(); // 3.x compatability, basically the app name

  int                   Exec(); // enter the event loop, either QCoreApplication or QApplication
  virtual void          Busy_(bool busy) {} // impl for gui in taiMisc
  virtual void          CheckConfigResult_(bool ok);
    // this is the nogui version; taiMisc does the gui version

  taiMiscCore(QObject* parent = NULL);
  ~taiMiscCore();

public slots:
  void                  PostUpdateAfter(); // run the dumpMisc::PostUpdateAfter; called asynchronously via a timer

protected slots:
  void                  app_aboutToQuit();
  virtual void          timer_timeout(); // called when timer times out, for waitproc processing

protected:
  QTimer*               timer; // for idle processing
  override void         customEvent(QEvent* ev);
  virtual void          Init(bool gui = false); // NOTE: called from static New
  virtual int           Exec_impl();
  virtual void          OnQuitting_impl(CancelOp& cancel_op); // allow to cancel
  virtual void          Quit_impl(CancelOp cancel_op); // non-gui guy does nothing
};

extern TA_API taiMiscCore* taiMC_; // note: use taiM macro instead
#ifdef TA_GUI
# define taiM taiM_
#else
# define taiM taiMC_
#endif

#endif // taiMiscCore_h
