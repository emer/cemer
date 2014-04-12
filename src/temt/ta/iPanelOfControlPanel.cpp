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

#include "iPanelOfControlPanel.h"
#include <taiEditorOfControlPanelFull>
#include <taiEditorOfControlPanelFast>
#include <iPanelSet>

#include <taMisc>
#include <taiMisc>


iPanelOfControlPanel::iPanelOfControlPanel(taiSigLink* dl_)
:inherited(dl_)
{
  ControlPanel* se_ = sele();
  se = NULL;
  if (se_) {
    /* switch (taMisc::select_edit_style) {
    case taMisc::ES_ALL_CONTROLS:
      se = new taiEditorOfControlPanelFull(se_, se_->GetTypeDef());
      break;
    case taMisc::ES_ACTIVE_CONTROL:
      se = new taiEditorOfControlPanelFast(se_, se_->GetTypeDef());
      break;
    }
    */
    se = new taiEditorOfControlPanelFull(se_, se_->GetTypeDef());
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
  }
}

iPanelOfControlPanel::~iPanelOfControlPanel() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iPanelOfControlPanel::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iPanelOfControlPanel::HasChanged() {
  return se->HasChanged();
}

bool iPanelOfControlPanel::ignoreSigEmit() const {
  return !isVisible();
}

void iPanelOfControlPanel::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iPanelOfControlPanel::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iPanelOfControlPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}

void iPanelOfControlPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  m_dps->UpdateMethodButtons();
}
