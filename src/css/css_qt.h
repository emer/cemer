// Copyright (C) 1995-2005 Regents of the University of Colorado,
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

// css_qt.h: Qt-css interface

#ifndef CSS_QT_H
#define CSS_QT_H

#include "ta_stdef.h"

class cssProgSpace;
class cssClassType;
class cssClassInst;

class cssiSession  { // this handles events for css/qt
public:
  static bool	block_stdin;	// block the processing of stdin input
  static bool	done_busy;	// true if done being busy after all events proc'd
  static bool	in_session;	// if we are in this session or not
  static bool	quitting; // #IGNORE set when user chooses Quit command in gui or shell

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

#endif // CSS_QT_H
