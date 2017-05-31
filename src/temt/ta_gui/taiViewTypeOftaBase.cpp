// Copyright 2016, Regents of the University of Colorado,
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

#include "taiViewTypeOftaBase.h"
#include <taiEditorOfClass>
#include <iPanelOfEditor>
#include <iPanelSet>
#include <taiEdit>

int taiViewTypeOftaBase::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taBase))
    return (inherited::BidForView(td) +1);
  return 0;
}

taiSigLink* taiViewTypeOftaBase::CreateSigLink_impl(taBase* data_) {
  return NULL; //no taBase implementation for now
}

iPanelBase* taiViewTypeOftaBase::CreateDataPanel(taiSigLink* dl_) {
  iPanelBase* rval = inherited::CreateDataPanel(dl_);
  // if more than one panel, then move the edit menu and methods to outside panelset
  if (m_dps) {
    taiEditorOfClass* edh = edit_panel->EditorOfClass();
    // move the menu -- note: QMenu on linux/win, QToolBar on mac
    m_dps->SetMenu(edh->menu->GetRep());
    m_dps->SetMethodBox(edh->frmMethButtons, edh);
    edh->frmMethButtons->setVisible(edh->showMethButtons());
  }
  return rval;
}

void taiViewTypeOftaBase::CreateDataPanel_impl(taiSigLink* dl)
{
  //NOTE: taiViewTypeOfList calls this directly to get the property panel --
  // if this method is changed substantially, make sure to check taiViewTypeOfList, and change
  // if necessary
  TypeDef* td = dl->GetDataTypeDef();
  taiEdit* taie = td->ie;
  //TODO: need to determine read_only
  //note: we cache this panel for the menu/method box fixup step
  // note: prob don't need this color nonsense, could just use defs, which does the same thing
  bool ok;
  const iColor bg = GetEditColorInherit(dl, ok);
  if (ok)
    edit_panel = taie->EditNewPanel(dl, dl->data(), false, bg);
  else
    edit_panel = taie->EditNewPanel(dl, dl->data(), false);
  DataPanelCreated(edit_panel);
}

taiSigLink* taiViewTypeOftaBase::GetSigLink(void* data_, TypeDef* el_typ) {
  //NOTE: replaced in taiViewTypeOftaOBase with an optimized version
  taBase* data = (taBase*)data_;
  if(data->isDestroying()) return NULL;
  taSigLink* dl = data->sig_link();
  if (dl) return (taiSigLink*)dl;
  else return CreateSigLink_impl(data);
}

const iColor taiViewTypeOftaBase::GetEditColorInherit(taiSigLink* dl, bool& ok) const {
  return ((taBase*)dl->data())->GetEditColorInherit(ok);
}
