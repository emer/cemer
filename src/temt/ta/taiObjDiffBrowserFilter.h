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

#ifndef taiObjDiffBrowserFilter_h
#define taiObjDiffBrowserFilter_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <iDialog>
#endif

// member includes:
#include <taString>


// declare all other types mentioned but not required to include:


class TA_API taiObjDiffBrowserFilter : iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS filter dialog for obj diff browser
INHERITED(iDialog)
  Q_OBJECT
public:

  static taiObjDiffBrowserFilter* New(int font_type, QWidget* par_window_ = NULL);

  // state to set action to:
  bool          action_on;

  // what types of changes to include
  bool          add;
  bool          del;
  bool          chg;

  // string value checking
  bool          nm_not;
  String        nm_contains;

  bool          val_not;
  String        val_contains;

  QVBoxLayout*          layOuter;
  QCheckBox*              chkActionOn;

  QCheckBox*              chkAdd;
  QCheckBox*              chkDel;
  QCheckBox*              chkChg;

  QCheckBox*              chkNmNot;
  iLineEdit*              editNm;
  QCheckBox*              chkValNot;
  iLineEdit*              editVal;

  QPushButton*            btnOk;
  QPushButton*            btnCancel;

  virtual bool          Browse();
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel -- examine the member vals for results
  virtual void  Constr();

  taiObjDiffBrowserFilter(QWidget* par_window_);
  ~taiObjDiffBrowserFilter();

protected slots:
  override void         accept();
  override void         reject();
};

#endif // taiObjDiffBrowserFilter_h
