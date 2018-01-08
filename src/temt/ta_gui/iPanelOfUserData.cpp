// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "iPanelOfUserData.h"
#include <taiEditorOfUserData>
#include <UserDataItem_List>

#include <taMisc>
#include <taiMisc>



iPanelOfUserData::iPanelOfUserData(taiSigLink* dl_)
:inherited(dl_)
{
  UserDataItem_List* se_ = udil();
  se = NULL;
  if (se_) {
    se = new taiEditorOfUserData(se_, se_->GetTypeDef());
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
  }
}

iPanelOfUserData::~iPanelOfUserData() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iPanelOfUserData::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iPanelOfUserData::HasChanged() {
  return se->HasChanged();
}

bool iPanelOfUserData::ignoreSigEmit() const {
  return !isVisible();
}

void iPanelOfUserData::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iPanelOfUserData::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iPanelOfUserData::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}

