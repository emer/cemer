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

// v3_compat.h -- version 3 compatability objects, 
//  only for converting v3.x files -- no other files should ref this,
//  since conversion routines should be in this file
// NOTE: symbols exported because they are used by value in the Project,
//  so require to be linked externally.

#ifndef V3_COMPAT_QTSO_H
#define V3_COMPAT_QTSO_H

#include "v3_compat.h"

#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"

#include "emergent_TA_type.h"

// forwards this file


class PDP_API ProcessDialog : public taiEditDataHost {
  // #IGNORE
  Q_OBJECT
public:
  ProcessDialog(void* base, TypeDef* tp, bool readonly = false)
    : taiEditDataHost(base, tp, readonly) { };
  ProcessDialog()				{ };
  ~ProcessDialog();

  virtual bool	CtrlPanel()	{ return false; }

public slots:
  void		Ok();		// override - stop the process when these are hit..
  void		Cancel(); // override
};

class PDP_API Process_RunDlg : public ProcessDialog {
  // #IGNORE
public:
  Process_RunDlg(void* base, TypeDef* tp, bool readonly = false)
    : ProcessDialog(base, tp, readonly) {};
  Process_RunDlg() {};
  ~Process_RunDlg();

  bool	CtrlPanel()	{ return true; }

  bool ShowMember(MemberDef* md);
};

class PDP_API taiProcess : public taiEdit {
  // adds control buttons to the edit window
public:
  taiProcess*	run_ie;

  int		BidForEdit(TypeDef* td);
  override int 	Edit(void* base, bool readonly = false, const iColor* bgclr = NULL); // unusual case, so we override

  void Initialize();
  void Destroy();
  TAQT_EDIT_INSTANCE(taiProcess, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class PDP_API taiProcessRunBox : public taiProcess {
  // just has the control buttons
public:
  int		BidForEdit(TypeDef*) 	{ return 0; }
  TAQT_EDIT_INSTANCE(taiProcessRunBox, taiProcess);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


#endif

