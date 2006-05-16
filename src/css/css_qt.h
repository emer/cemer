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


// css_qt.h: Qt-css interface

#ifndef CSS_QT_H
#define CSS_QT_H

#include "ta_stdef.h"
#include "css_def.h"

// externals
class cssProgSpace;
class cssClassType;
class cssClassInst;

class CSS_API cssiSession  { // this handles events for css/qt
public:
  static void	CancelProgEdits(cssProgSpace* prsp);
  // cancel any active edit dialogs for given prog space
  static void	CancelClassEdits(cssClassType* cltyp);
  // cancel any active edit dialogs for given class type
  static void	CancelObjEdits(cssClassInst* clobj);
  // cancel any active edit dialogs for given class object
  static void	RaiseObjEdits();
  // bring all object edit dialogs to the front (e.g., after loading)

private:
  cssiSession();
  ~cssiSession();
};

/*obs
class cssProgSpace;
class cssClassType;
class cssClassInst;

class CSS_API cssiSession  { // this handles events for css/qt
public:
  static bool	block_stdin;	// block the processing of stdin input
  static bool	done_busy;	// true if done being busy after all events proc'd
  static bool	in_session;	// if we are in this session or not
  static bool	quitting; // #IGNORE set when user chooses Quit command in gui or shell

  static int	readline_waitproc(); // note: nongui version in cssMisc
  static void	CancelProgEdits(cssProgSpace* prsp);
  // cancel any active edit dialogs for given prog space
  static void	CancelClassEdits(cssClassType* cltyp);
  // cancel any active edit dialogs for given class type
  static void	CancelObjEdits(cssClassInst* clobj);
  // cancel any active edit dialogs for given class object
  static void	RaiseObjEdits();
  // bring all object edit dialogs to the front (e.g., after loading)

  static void 	Init();		// call this to initialize the handler
  static void 	Stop();		// tell the processing loop to stop
  static void	Quit();		// tell the Qt session to quit

  static int	Run();		// process events, etc.
  static int	RunPending();	// run any pending events (don't wait..)

  static int	(*WaitProc)();
  // set this to a work process for idle time processing.  return true if something
  // happend that might create new window system events..

private:
  cssiSession() {}
  ~cssiSession() {}
};
*/
#endif // CSS_QT_H
