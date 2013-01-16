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

#include "iDataPanel.h"

#include <DataChangedReason>
#include <taiMisc>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QSCrollBar>


iDataPanel::iDataPanel(taiDataLink* dl_)
:QFrame(NULL)
{
  m_tabView = NULL; // set when added to tabview; remains NULL if in a panelset
  m_pinned = false;
  m_rendered = false;
  m_show_updt = false;
  m_update_on_show = true; // legacy default for most panels
  m_update_req = true;
  show_req = false;
  setFrameStyle(NoFrame | Plain);
  scr = new QScrollArea(this);
  scr->setWidgetResizable(true);
  m_saved_scroll_pos = 0;
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0);
  layOuter->setSpacing(2); //def
  layOuter->addWidget(scr, 1);

  if (dl_) //note: most classes always pass a link, but ex DocDataPanel doesn't
    dl_->AddDataClient(this); // sets our m_link variable
}

iDataPanel::~iDataPanel() {
  //note: m_tabView should be NULL during general destruction
  if (m_tabView)
    m_tabView->DataPanelDestroying(this);
}

QWidget* iDataPanel::centralWidget() const {
  return scr->widget();
}

void iDataPanel::customEvent(QEvent* ev_) {
  // we return early if we don't accept, otherwise fall through to accept
  bool focus = false;
  switch ((int)ev_->type()) {
  case CET_SHOW_PANEL_FOCUS:
    focus = true; // FALL THROUGH
  case CET_SHOW_PANEL: {
    if (show_req) {
      FrameShowing_Async(focus);
      show_req = false;
    }
  } break;
  default: inherited(ev_);
    return; // don't accept
  }
  ev_->accept();
}


void iDataPanel::DataChanged_impl(int dcr, void* op1, void* op2) {
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    if (tabView())
      tabView()->UpdateTabName(this); //in case changed
  }
}

void iDataPanel::FrameShowing(bool showing, bool focus) {
  // note: if caller sets focus on its showing, then this will cause
  // spurious tab focuses on restore window, change desktop, etc.
  if (tabView()) {
    tabView()->ShowTab(this, showing, focus);
  }
}

void iDataPanel::FrameShowing_Async(bool focus) {
  if (tabView()) {
    tabView()->ShowTab(this, true, focus);
  }
  show_req = false;
}

void iDataPanel::GetWinState() {
  GetWinState_impl();
}

void iDataPanel::SetWinState() {
  SetWinState_impl();
}

void iDataPanel::OnWindowBind(iTabViewer* itv) {
  OnWindowBind_impl(itv);
  if (m_rendered)
    SetWinState();
}

void iDataPanel::UpdatePanel() {
  if (!isVisible()) return;
  SaveScrollPos();
  UpdatePanel_impl();
  RestoreScrollPos();
}

void iDataPanel::UpdatePanel_impl() {
  m_update_req = false;
  if (tabView())
    tabView()->UpdateTabName(this); //in case changed
}

void iDataPanel::Render() {
  if (!m_rendered) {
    Render_impl();
    SetWinState();
    m_rendered = true;
    InitPanel();
    UpdatePanel();
  }
}

void iDataPanel::ResolveChanges(CancelOp& cancel_op) {
  ResolveChanges_impl(cancel_op);
}

void iDataPanel::setButtonsWidget(QWidget* widg) {
  widg->setParent(this);
  layOuter->addWidget(widg);
}

void iDataPanel::setCentralWidget(QWidget* widg) {
  scr->setWidget(widg);
  widg->show();
}

void iDataPanel::setUpdateOnShow(bool value) {
  if (m_update_on_show == value) return;
  m_update_on_show = value; // no action needed...
}

void iDataPanel::setPinned(bool value) {
  if (m_pinned == value) return;
  m_pinned = value; // no action needed... "pinned is just a state of mind"
  if (tabView())
    tabView()->UpdateTabNames(); //updates the icons
}

void iDataPanel::hideEvent(QHideEvent* ev) {
  // note: we only call the impl, because each guy gets it, so we don't
  // want sets to then invoke this twice
  CancelOp cancel_op = CO_PROCEED;
  if (m_rendered) ResolveChanges_impl(cancel_op);
  inherited::hideEvent(ev);
}

void iDataPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  // note: we only call the impl, because each guy gets it, so we don't
  // want sets to then invoke this twice
  if (m_rendered) {
    if (updateOnShow() || m_update_req) {
      m_show_updt = true;
      SaveScrollPos();
      UpdatePanel_impl();
      RestoreScrollPos();
      m_show_updt = false;
    }
  } else            Render();
}

iTabBar::TabIcon iDataPanel::tabIcon() const {
//note: locked will be conveyed by no icon, since the lock icon takes up so much room
  if (lockInPlace())
    return iTabBar::TI_NONE; //iTabBar::TI_LOCKED;
  else {
    if (pinned()) return iTabBar::TI_PINNED;
    else return iTabBar::TI_UNPINNED;
  }
}

String iDataPanel::TabText() const {
/*Qt3  ISelectable* ci = tabViewerWin()->curItem();
  if (ci) {
    return ci->view_name();
  } else return _nilString; */
  if (m_link)
    return link()->GetDisplayName();
  else return _nilString;
}

void iDataPanel::SaveScrollPos() {
  m_saved_scroll_pos = ScrollBarV()->value();
}

void iDataPanel::RestoreScrollPos() {
  ScrollTo(m_saved_scroll_pos);
}

QScrollBar* iDataPanel::ScrollBarV() {
  return scr->verticalScrollBar();
}

void iDataPanel::ScrollTo(int scr_pos) {
  taiMisc::ScrollTo_SA(scr, scr_pos);
}

void iDataPanel::CenterOn(QWidget* widg) {
  taiMisc::CenterOn_SA(scr, scr->widget(), widg);
}

void iDataPanel::KeepInView(QWidget* widg) {
  taiMisc::KeepInView_SA(scr, scr->widget(), widg);
}

bool iDataPanel::PosInView(int scr_pos) {
  return taiMisc::PosInView_SA(scr, scr_pos);
}

QPoint iDataPanel::MapToPanel(QWidget* widg, const QPoint& pt) {
  return taiMisc::MapToArea_SA(scr, scr->widget(), widg, pt);
}

int iDataPanel::MapToPanelV(QWidget* widg, int pt_y) {
  return taiMisc::MapToAreaV_SA(scr, scr->widget(), widg, pt_y);
}

