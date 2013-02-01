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

#include "EditDataPanel.h"
#include <taiEditorWidgetsOfClass>

#include <taiMisc>


EditDataPanel::EditDataPanel(taiEditorWidgetsOfClass* owner_, taiSigLink* dl_)
:inherited(dl_)
{
  owner = owner_;
  bgcol = NULL;
}

EditDataPanel::~EditDataPanel() {
  if (owner) {
    owner->WidgetDeleting(); // removes our ref
    owner->deleteLater(); // no one else does this!
    owner = NULL;
  }
}

void EditDataPanel::Closing(CancelOp& cancel_op) {
  //note: a higher level routine may already have resolved changes
  // if so, the ResolveChanges call is superfluous
  if (!owner) return;
  bool discarded = false;
  owner->ResolveChanges(cancel_op, &discarded);
  if (cancel_op == CO_CANCEL) {
    return;
  } else if (!discarded) {
    owner->state = taiEditorWidgetsMain::ACCEPTED;
    return;
  }
  // discarded, or didn't have any changes
  owner->state = taiEditorWidgetsMain::CANCELED;
}

void EditDataPanel::UpdatePanel_impl() {
  if (owner) owner->Refresh();
}

const iColor EditDataPanel::GetTabColor(bool selected, bool& ok) const {
  if (owner) {
    ok = true;
    return (selected) ? owner->bg_color : owner->bg_color_dark;
  } else
    return inherited::GetTabColor(selected, ok);
}

bool EditDataPanel::HasChanged_impl() {
  if (owner) return owner->HasChanged();
  else       return false;
}

String EditDataPanel::panel_type() const {
  static String str("Properties");
  return str;
}

void EditDataPanel::UpdatePanel() {
  //   iDataPanel::UpdatePanel_impl(); // for tab stuff -- skip our direct inherit!
  // actually, our refresh has built-in vis testing, so don't do visibility check which
  // the parent guy does!  this allows edit guy to update buttons even if it is not
  // itself visible..
  UpdatePanel_impl();
}

void EditDataPanel::Render_impl() {
  inherited::Render_impl();
  taiEditorWidgetsOfClass* edh = editDataHost();
  if (edh->state >= taiEditorWidgetsMain::CONSTRUCTED) return;

  edh->ConstrDeferred();
  setCentralWidget(edh->widget());
  setButtonsWidget(edh->widButtons);
  taiMisc::active_edits.Add(edh); // add to the list of active edit dialogs
  edh->state = taiEditorWidgetsMain::ACTIVE;
  //prob need to do a refresh!!!
}

void EditDataPanel::ResolveChanges_impl(CancelOp& cancel_op) {
  if (!owner) return;
  owner->ResolveChanges(cancel_op);
}

QWidget* EditDataPanel::firstTabFocusWidget() {
  return editDataHost()->firstTabFocusWidget();
}

void EditDataPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  taiEditorWidgetsOfClass* edh = editDataHost();
  if(edh && edh->state >= taiEditorWidgetsMain::CONSTRUCTED)  {
    edh->GetButtonImage();              // update buttons whenver we show!
  }
}

