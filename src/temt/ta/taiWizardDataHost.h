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

#ifndef taiWizardDataHost_h
#define taiWizardDataHost_h 1

// parent includes:
#include <taiDataHost_impl>

// member includes:
#include <MembSet>

// declare all other types mentioned but not required to include:
class taWizard; //
class QTabWidget; //

class TA_API taiWizardDataHost : public taiDataHost_impl {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit host for Wizards -- uses tab pages
  Q_OBJECT
INHERITED(taiDataHost_impl)
public:
  MembSet_List          membs; // one set per page

  QTabWidget*           tabs;

  Member_List&          memb_el(int i) // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}

//  override void               GetImage();
//  override void               GetValue();
  override void         Constr_Buttons();//

//  override bool       eventFilter(QObject *obj, QEvent *event);
  // event filter to trigger apply button on Ctrl+Return

  taiWizardDataHost(taWizard* base, TypeDef* typ_ = NULL,
    bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  ~taiWizardDataHost();

protected:
  String_PArray         page_names; // stores names, or blank if none
  override void         Constr_Strings();
  override void         Constr_Box();
  override void         Constr_Data_Labels();
  override void         Constr_Data_Labels_impl(int& idx, Member_List* ms,
     taiDataList* dl);
  override void         Constr_RegNotifies();
//  override void       DoConstr_Dialog(iHostDialog*& dlg);
  override void         Enum_Members(); // called by Constr_impl to fill memb_el[]
  override void         ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
  override void         Ok_impl();
};

#endif // taiWizardDataHost_h
