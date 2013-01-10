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

#include "iDataPanelSetBase.h"

iDataPanelSetBase::iDataPanelSetBase(taiDataLink* link_)
:inherited(link_)
{
  cur_panel_id = -1;
  widg = new QWidget();
  layDetail = new QVBoxLayout(widg);
  // don't introduce any new margin
  layDetail->setMargin(0);
  layDetail->setSpacing(0);
  wsSubPanels = new QStackedWidget(widg); // add to lay by desc

  setCentralWidget(widg);
}

iDataPanelSetBase::~iDataPanelSetBase() {
  for (int i = panels.size - 1; i >= 0 ; --i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->setTabView(NULL);
    pn->ClearDataPanelSet();
  }
}

void iDataPanelSetBase::Closing(CancelOp& cancel_op) {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->Closing(cancel_op);
    if (cancel_op == CO_CANCEL) return;
  }
}

void iDataPanelSetBase::ClosePanel() {
  for (int i = panels.size - 1; i >= 0 ; --i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->ClosePanel();
  }
  if (tabView()) // effectively unlink from system
    tabView()->DataPanelDestroying(this);
  deleteLater();
}

void iDataPanelSetBase::DataLinkDestroying(taDataLink* dl) {
  ClosePanel();
} // nothing for us; subpanels handle

void iDataPanelSetBase::UpdatePanel() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->UpdatePanel();
  }
}

const iColor iDataPanelSetBase::GetTabColor(bool selected, bool& ok) const {
  iDataPanel* pn = curPanel();
  if (pn) return pn->GetTabColor(selected, ok);
  else    return inherited::GetTabColor(selected, ok);
}

iDataPanel* iDataPanelSetBase::GetDataPanelOfType(TypeDef* typ, int& idx) {
  while ((idx >= 0) && (idx < panels.size)) {
    iDataPanel* rval = panels.FastEl(idx);
    idx++; // before returning val
    if (rval->GetTypeDef()->InheritsFrom(typ))
      return rval;
  }
  return NULL;
}

void iDataPanelSetBase::GetWinState() {
  GetWinState_impl();
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn) pn->GetWinState();
  }
}

void iDataPanelSetBase::SetWinState() {
  SetWinState_impl();
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn) pn->SetWinState();
  }
}

bool iDataPanelSetBase::HasChanged() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn->HasChanged_impl()) return true;
  }
  return false;
}

void iDataPanelSetBase::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->OnWindowBind(itv);
  }
}

void iDataPanelSetBase::removeChild(QObject* obj) {
  panels.RemoveEl_(obj); // harmless if not a panel
  if (obj)
    obj->setParent(0);
}

void iDataPanelSetBase::ResolveChanges(CancelOp& cancel_op) {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return;
  }
  inherited::ResolveChanges(cancel_op); // calls our own impl
}

void iDataPanelSetBase::setCurrentPanelId(int id) {
  if (cur_panel_id == id) return;
  cur_panel_id = id; // set this early, to prevent unexpected calls from the signal
  setCurrentPanelId_impl(id);
}

void iDataPanelSetBase::setTabView(iTabView* value) {
  inherited::setTabView(value);
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->setTabView(value);
  }
}

