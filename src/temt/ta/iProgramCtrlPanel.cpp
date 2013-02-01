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

#include "iProgramCtrlPanel.h"
#include <taiEditorOfProgramCtrl>
#include <Program>
#include <iDataPanelSet>

#include <taMisc>
#include <taiMisc>



iProgramCtrlPanel::iProgramCtrlPanel(taiSigLink* dl_)
:inherited(dl_)
{
  Program* prog_ = prog();
  pc = NULL;
  if (prog_) {
    pc = new taiEditorOfProgramCtrl(prog_);
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = prog_->GetEditColorInherit(ok);
      if (ok) pc->setBgColor(bgcol);
    }
    pc->ConstrEditControl();
    setCentralWidget(pc->widget()); //sets parent
    setButtonsWidget(pc->widButtons);
  }
}

iProgramCtrlPanel::~iProgramCtrlPanel() {
  if (pc) {
    delete pc;
    pc = NULL;
  }
}

void iProgramCtrlPanel::SigEmit_impl(int sls, void* op1_, void* op2_) {
  inherited::SigEmit_impl(sls, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
  // not in this case!
}

bool iProgramCtrlPanel::HasChanged_impl() {
  if (pc) return pc->HasChanged();
  else return false;
}

void iProgramCtrlPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iProgramCtrlPanel::UpdatePanel_impl() {
  if (pc) pc->ReShow_Async();
}

void iProgramCtrlPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (pc && pc->HasChanged()) {
    pc->Apply();
  }
}

void iProgramCtrlPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  m_dps->UpdateMethodButtons();
}
