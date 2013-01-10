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

#include "iViewPanelFrame.h"
#include <taMisc>

int iViewPanelFrame::show() const {
  return taMisc::show_gui;
}

iViewPanelFrame::iViewPanelFrame(taDataView* dv_)
:inherited((taiDataLink*)dv_->GetDataLink()) //NOTE: link not created yet during loads
{
  m_dps = NULL;
  vp_flags = 0;
  read_only = false;
  m_dv = dv_;
  updating = 0;
  m_modified = false;
  apply_req = false;
  taDataLink* dl = dv_->GetDataLink();
  dl->AddDataClient(this);
  btnApply = NULL;
  btnRevert = NULL;
  btnCopyFrom = NULL;
  warn_clobber = false;
}

iViewPanelFrame::~iViewPanelFrame() {
  if (m_dps) {// if in a panelset, and it hasn't null'ed us
    m_dps->PanelDestroying(this); // removes tab
  }
}

void iViewPanelFrame::Apply() {
  if (warn_clobber) {
    int chs = taMisc::Choice("Warning: this object has changed since you started editing -- if you apply now, you will overwrite those changes -- what do you want to do?",
                             "Apply", "Revert", "Cancel");
    if(chs == 1) {
      Revert();
      return;
    }
    if(chs == 2)
      return;
  }
  GetValue();
  GetImage();
  InternalSetModified(false); // superfulous??
}

void iViewPanelFrame::Apply_Async() {
  if (apply_req) return; // already waiting
  QEvent* ev = new QEvent((QEvent::Type)CET_APPLY);
  apply_req = true;
  QCoreApplication::postEvent(this, ev);
}

void iViewPanelFrame::ClosePanel() {
  CancelOp cancel_op = CO_NOT_CANCELLABLE;
  Closing(cancel_op); // forced, ignore cancel

  emit destroyed(this); // signals tab view we are gone
/*  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this); */
  if (m_dps) {// if in a panelset, we let panelset destroy us
    m_dps->PanelDestroying(this); // removes tab
    if (tabView()) // effectively unlink from system
      tabView()->DataPanelDestroying(this);
  } else {
    deleteLater(); // per Qt specs, defer deletions to avoid issues
  }
}

void iViewPanelFrame::Changed() {
  if (updating > 0) return;
  InternalSetModified(true);
}

const iColor iViewPanelFrame::colorOfCurRow() const {
  return this->palette().color(QPalette::Active, QPalette::Background);
}

void iViewPanelFrame::customEvent(QEvent* ev_) {
  // we return early if we don't accept, otherwise fall through to accept
  switch ((int)ev_->type()) {
  case CET_APPLY: {
    if (apply_req) {
      Apply();
      apply_req = false;
    }
  } break;
  default: inherited::customEvent(ev_);
    return; // don't accept
  }
  ev_->accept();
}

void iViewPanelFrame::DataLinkDestroying(taDataLink*) {
  m_dv = NULL;
  ClosePanel();
}

void iViewPanelFrame::GetValue() {
  ++updating;
  InternalSetModified(false); // do it first, so signals/updates etc. see us nonmodified
  GetValue_impl();
  --updating;
}

void iViewPanelFrame::InitPanel() {
  if (!m_dv) return;
  if (updating) return;
  ++updating;
  InitPanel_impl();
  if (isVisible()) { // no update when hidden!
    UpdatePanel_impl();
  }
  --updating;
}

void iViewPanelFrame::InternalSetModified(bool value) {
  m_modified = value;
  if (!value) warn_clobber = false;
  UpdateButtons();
}

void iViewPanelFrame::MakeButtons(QBoxLayout* lay, QWidget* par) {
  if (vp_flags & VP_USE_BTNS) return;
  if (!lay) return; // bug
  if (!par) par = this;

  QHBoxLayout* layButtons = NULL;
  if (lay)
    layButtons = new QHBoxLayout();
  else
    layButtons = new QHBoxLayout(par);
  layButtons->setMargin(0);
  layButtons->setSpacing(0);
  btnCopyFrom = new HiLightButton("&Copy From", par);
  layButtons->addWidget(btnCopyFrom);
  //  layButtons->addSpacing(4);

  layButtons->addStretch();
  btnApply = new HiLightButton("&Apply", par);
  layButtons->addWidget(btnApply);
  layButtons->addSpacing(4);

  btnRevert = new HiLightButton("&Revert", par);
  layButtons->addWidget(btnRevert);
  layButtons->addSpacing(4);
  if (lay)
    lay->addLayout(layButtons);

  connect(btnApply, SIGNAL(clicked()), this, SLOT(Apply()) );
  connect(btnRevert, SIGNAL(clicked()), this, SLOT(Revert()) );
  connect(btnCopyFrom, SIGNAL(clicked()), this, SLOT(CopyFrom()) );

  vp_flags |= VP_USE_BTNS;
  InternalSetModified(false);
}

void iViewPanelFrame::ResolveChanges_impl(CancelOp& cancel_op) {
  // called by root on closing, dialog on closing, etc. etc., when hiding
  if (HasChanged()) {
    Apply();
  }
}

void iViewPanelFrame::Revert() {
  GetImage();
  InternalSetModified(false);
}

void iViewPanelFrame::CopyFrom() {
  CopyFrom_impl();
  UpdatePanel();
  if(Base() && Base()->InheritsFrom(&TA_taDataView)) {
    taDataView* dvm = (taDataView*)Base();
    dvm->Render();
  }
}

void iViewPanelFrame::UpdatePanel() {
  if (!m_dv) return;
  if (updating) return;
  if (!isVisible()) return; // no update when hidden!
  ++updating;
  SaveScrollPos();
  UpdatePanel_impl();
  RestoreScrollPos();
  --updating;
}

String iViewPanelFrame::TabText() const {
  if (m_dv) return m_dv->GetLabel();
  else      return inherited::TabText();
}

iTabViewer* iViewPanelFrame::tabViewerWin() const {
  return (tabView()) ? tabView()->tabViewerWin() : NULL;
}

void iViewPanelFrame::UpdateButtons() {
  if (!(vp_flags & VP_USE_BTNS)) return;
  if (Root() && m_modified) {
    btnApply->setEnabled(true);
    btnRevert->setEnabled(true);
  } else {
    btnApply->setEnabled(false);
    btnRevert->setEnabled(false);
  }
}

void iViewPanelFrame::keyPressEvent(QKeyEvent* e) {
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
    Apply();                    // do it!
  }
  if(e->key() == Qt::Key_Escape) {
    Revert();                   // do it!
  }
}

