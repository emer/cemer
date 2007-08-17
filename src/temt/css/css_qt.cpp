// Copyright, 1995-2005, Regents of the University of Colorado,
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

