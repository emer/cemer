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

#include "iUserDataPanel.h"
#include <iUserDataDataHost>
#include <UserDataItem_List>

#include <taMisc>
#include <taiMisc>



iUserDataPanel::iUserDataPanel(taiSigLink* dl_)
:inherited(dl_)
{
  UserDataItem_List* se_ = udil();
  se = NULL;
  if (se_) {
    switch (taMisc::select_edit_style) { // NOTE: the two below look identical to me...
    case taMisc::ES_ALL_CONTROLS:
      se = new iUserDataDataHost(se_, se_->GetTypeDef());
      break;
    case taMisc::ES_ACTIVE_CONTROL:
      se = new iUserDataDataHost(se_, se_->GetTypeDef());
      break;
    }
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
  }
}

iUserDataPanel::~iUserDataPanel() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iUserDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iUserDataPanel::HasChanged() {
  return se->HasChanged();
}

bool iUserDataPanel::ignoreDataChanged() const {
  return !isVisible();
}

void iUserDataPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iUserDataPanel::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iUserDataPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}

