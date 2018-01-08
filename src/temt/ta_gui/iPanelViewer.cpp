// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "iPanelViewer.h"
#include <iTabView>
#include <iFrameViewer>
#include <PanelViewer>
#include <FrameViewer>
#include <iMainWindowViewer>
#include <ISelectableHost>
#include <MainWindowViewer>
#include <Program>
#include <ProgEl>
#include <QVBoxLayout>



iPanelViewer::iPanelViewer(PanelViewer* viewer_, QWidget* parent)
: inherited(viewer_, parent)
{
  Init();
}

iPanelViewer::~iPanelViewer()
{
  //  delete m_tabViews;
  //  m_tabViews = 0;
  
  delete m_curTabView;
  m_curTabView = 0;
}

void iPanelViewer::Init() {
  //  m_tabViews = new  iTabView_PtrList();
  cur_item = NULL;
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setMargin(0);  lay->setSpacing(0);
  m_curTabView = new iTabView(this, this);
  lay->addWidget(m_curTabView);
  //  m_tabViews->Add(rval);
  //nn  m_curTabView->show();
}

void iPanelViewer::AddPanel(iPanelBase* panel) {
  tabView()->AddPanel(panel);
}

void iPanelViewer::AddPanelNewTab(iPanelBase* panel, bool lock) {
  tabView()->AddPanelNewTab(panel, lock);
}

void iPanelViewer::AddTab() {
  tabView()->AddTab();
}

void iPanelViewer::CloseTab() {
  tabView()->CloseTab();
}

void iPanelViewer::Closing(CancelOp& cancel_op) {
  tabView()->Closing(cancel_op);
}

void iPanelViewer::Constr_post() {
  /*  for (int i = 0; i < m_tabViews->size; ++i) {
   iTabView* itv = m_tabViews->FastEl(i);
   itv->OnWindowBind(this);
   }*/
  if (m_curTabView) {
    m_curTabView->OnWindowBind(this);
  }
}

void iPanelViewer::GetWinState_impl() {
  /*for (int i = 0; i < m_tabViews->size; ++i) {
   iTabView* itv = m_tabViews->FastEl(i);
   itv->GetWinState();
   }*/
  if (m_curTabView) {
    m_curTabView->GetWinState();
  }
}

void iPanelViewer::SetWinState_impl() {
  /*for (int i = 0; i < m_tabViews->size; ++i) {
   iTabView* itv = m_tabViews->FastEl(i);
   itv->SetWinState();
   }*/
  if (m_curTabView) {
    m_curTabView->SetWinState();
  }
}

void iPanelViewer::Refresh_impl() {
  /* for (int i = 0; i < m_tabViews->size; ++i) {
   iTabView* itv = m_tabViews->FastEl(i);
   itv->Refresh();
   }*/
  if (m_curTabView) {
    m_curTabView->Refresh();
  }
}

void iPanelViewer::ResolveChanges_impl(CancelOp& cancel_op) {
  /*for (int i = 0; i < m_tabViews->size; ++i) {
   iTabView* itv = m_tabViews->FastEl(i);
   if (itv) itv->ResolveChanges(cancel_op);
   if (cancel_op == CO_CANCEL) break;
   }
   }*/
  if (m_curTabView) {
    m_curTabView->ResolveChanges(cancel_op);
  }
}

void iPanelViewer::SelectionChanged_impl(ISelectableHost* src_host) {
  if (tab_changing) return;
  //TODO: should actually check if old panel=new panel, since theoretically, two different
  // gui items can have the same siglink (i.e., underlying data)
  iPanelBase* new_pn = NULL;
  cur_item = src_host->curItem(); // note: could be NULL
  if (!cur_item) goto end; // no selected item, so no change
  if (m_curTabView) {
    taiSigLink* link = cur_item->effLink();
/*
    if (link->taData()->GetTypeDef()->DerivesFrom(&TA_ProgEl)) {
      ProgEl* prgEl = (ProgEl*)link->taData();
      prgEl->EditProgramEl();
    }
    else if (link) {
 */
    if (link) {
      new_pn = m_curTabView->GetDataPanel(link);
      m_curTabView->ShowPanel(new_pn);
    }
  }
end:
  inherited::SelectionChanged_impl(src_host); // prob does nothing
}

void iPanelViewer::TabView_Destroying(iTabView* tv) {
  if (m_curTabView != tv) return;
  m_curTabView = NULL;
  /* int idx = m_tabViews->FindEl(tv);
   m_tabViews->RemoveIdx(idx);
   if (m_curTabView != tv) return;
   // focus next, if any, or prev, if any
   if (idx >= m_tabViews->size) --idx;
   TabView_Selected(m_tabViews->PosSafeEl(idx)); // NULL if no more*/
}

void iPanelViewer::TabView_Selected(iTabView* tv) {
  if (m_curTabView == tv) return;
  if (m_curTabView) {
    m_curTabView->Activated(false);
  }
  m_curTabView = tv;
  if (m_curTabView) {
    m_curTabView->Activated(true);
  }
}

void iPanelViewer::ShowLink(taiSigLink* link, bool not_in_cur) {
  tabView()->ShowLink(link, not_in_cur);
}

void iPanelViewer::ShowPanel(iPanelBase* panel) {
  tabView()->ShowPanel(panel);
}

void iPanelViewer::UpdateTabNames() { // called by a siglink when a tab name might have changed
  tabView()->UpdateTabNames();
}

iTabBarBase* iPanelViewer::tabBar() {
  if(!tabView()) return NULL;
  return tabView()->tabBar();
}

void iPanelViewer::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  MainWindowViewer* mwv = mainWindowViewer();
  if(mwv && mwv->widget()) {
    mwv->widget()->FocusIsMiddlePanel();
  }
}


