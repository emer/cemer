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

#include "iPanelOfDocEdit.h"
#include <taDoc>
#include <taiEditorOfDoc>

#include <SigLinkSignal>
#include <taMisc>



iPanelOfDocEdit::iPanelOfDocEdit(taiSigLink* dl_)
:inherited(dl_)
{
  taDoc* doc_ = doc();
  de = NULL;
  if (doc_) {
    de = new taiEditorOfDoc(doc_, doc_->GetTypeDef());
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = doc_->GetEditColorInherit(ok);
      if (ok) de->setBgColor(bgcol);
    }
    de->ConstrEditControl();
    setCentralWidget(de->widget()); //sets parent
    setButtonsWidget(de->widButtons);
  }
}

iPanelOfDocEdit::~iPanelOfDocEdit() {
  if (de) {
    delete de;
    de = NULL;
  }
}

QWidget* iPanelOfDocEdit::firstTabFocusWidget() {
  if(!de) return NULL;
  return de->firstTabFocusWidget();
}

bool iPanelOfDocEdit::ignoreSigEmit() const {
  return false; // don't ignore -- we do SmartButComplicatedIgnore(TM)
//  return !isVisible();
}

void iPanelOfDocEdit::SigEmit_impl(int sls, void* op1_, void* op2_) {
  if (sls <= SLS_ITEM_UPDATED_ND) {
    this->m_update_req = true; // so we update next time we show, if hidden
  }
  inherited::SigEmit_impl(sls, op1_, op2_);
}

bool iPanelOfDocEdit::HasChanged() {
  if (de) return de->HasChanged();
  else return false;
}

void iPanelOfDocEdit::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iPanelOfDocEdit::UpdatePanel_impl() {
  inherited::UpdatePanel_impl(); // clears reg flag and updates tab
  if (de) de->ReShow_Async();
}

void iPanelOfDocEdit::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (de && de->HasChanged()) {
    de->Apply();
  }
}

