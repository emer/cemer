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

#include "iTabView.h"
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <iPanelBase>
#include <iTreeView>
#include <iTreeViewItem>
#include <taiMisc>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QMenu>
#include <QKeyEvent>
#include <QItemSelectionModel>


iTabView::iTabView(QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = NULL;
  Init();
}

iTabView::iTabView(iPanelViewer* data_viewer_, QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = data_viewer_;
  Init();
}


iTabView::~iTabView()
{
  panels.Reset(); // removes the refs, so we don't get callbacks
  if (m_viewer_win)
    m_viewer_win->TabView_Destroying(this);
}


void iTabView::Init() {
  panels.m_tabView = this;
  layDetail = new QVBoxLayout(this);
  // no margins/spacing needed
  layDetail->setMargin(0);
  layDetail->setSpacing(0);
  tbPanels = new iTabBar(this);
#if (QT_VERSION >= 0x040200)
  tbPanels->setUsesScrollButtons(true);
  tbPanels->setElideMode(Qt::ElideMiddle/*Qt::ElideNone*/);
#endif
  layDetail->addWidget(tbPanels);
  wsPanels = new QStackedWidget(this);
  layDetail->addWidget(wsPanels);
  // add a dummy data panel with id=0 to show blank (except dialogs)
  MainWindowViewer* mwv = tabViewerWin()->mainWindowViewer();
  if (!mwv->isDialog()) {
    tbPanels->addTab("");
    connect(tbPanels, SIGNAL(currentChanged(int)),
        this, SLOT(panelSelected(int)) );
  }
}

void iTabView::Activated(bool val) {
  // change our appearance when inactivated
  QFont f(tbPanels->font());
  f.setItalic(!val);
  tbPanels->setFont(f);
}

bool iTabView::ActivatePanel(taiSigLink* dl) {
  for (int i = 0; i < tbPanels->count(); ++i) {
    iPanelBase* panel = tbPanels->panel(i);
    if (!panel) continue;
    if (panel->link() == dl) {
      SetCurrentTab(i);
      return true;
    }
  }
  return false;
}

bool iTabView::AddPanel(iPanelBase* panel) {
  if (!panels.AddUnique(panel))
    return false; // refs us on add
    wsPanels->addWidget(panel);
  if (panels.size == 1) wsPanels->setCurrentWidget(panel); // always show first
  iPanelViewer* itv = tabViewerWin();
  if (itv) panel->OnWindowBind(itv);
  return true;
}

void iTabView::AddPanelNewTab(iPanelBase* panel, bool lock) {
  AddPanel(panel); //noop if already added
  if (lock) panel->Pin();
  int tab_idx = TabIndexOfPanel(panel);
  if(tab_idx < 0)
    tab_idx = tbPanels->addTab(panel);
  SetCurrentTab(tab_idx);
}

void iTabView::AddTab(int tab_idx) {
  iPanelBase* pan = NULL;
  if (tab_idx >= 0)
    pan = tbPanels->panel(tab_idx);
  // "AddTab" on a view guy just makes a blank tab
  // if (pan && pan->lockInPlace())
  //   pan = NULL;
  int new_tab_idx = tbPanels->addTab(pan);
  SetCurrentTab(new_tab_idx);
}

void iTabView::CloseTab(int tab) {
  if (tab < 0) return; // huh?
  // don't allow closing last tab for a modified panel
  if (tbPanels->count() > 1) {
    iPanelBase* panel_base = tbPanels->panel(tbPanels->currentIndex());
    panel_base->Unpin();
    tbPanels->removeTab(tab);
    panelSelected(tbPanels->currentIndex()); // needed to resync proper panel with tab
  } else { // last tab
    panelSelected(-1);
  }
}

void iTabView::Closing(CancelOp& cancel_op) {
  // close all panels
  for (int i = panels.size - 1; i >= 0; --i) {
    iPanelBase* panel = panels.FastEl(i);
    panel->Closing(cancel_op);
    if (cancel_op == CO_CANCEL) return; // can stop now

    RemoveDataPanel(panel); // note: removes from tabs, and deletes tabs
  }
}

iPanelBase* iTabView::curPanel() const {
  iPanelBase* rval = tbPanels->panel(tbPanels->currentIndex());
  return rval; // could be NULL if empty
}

void iTabView::DataPanelDestroying(iPanelBase* panel) {
  RemoveDataPanel(panel);//TODO
}

void iTabView::FillTabBarContextMenu(QMenu* contextMenu, int tab_idx) {
  iPanelBase* dp = tabPanel(tab_idx); // always safe, NULL if no tab
  // note: need to (re)parent the actions; not parented by adding to menu
  iAction* act = NULL;
  // only add Close if on a tab
  if (tab_idx >= 0) {
    // always add for consistency, even if on an empty or locked guy
    act = new iAction(tab_idx, "&Close Tab", QKeySequence());
    act->setParent(contextMenu);
    contextMenu->addAction(act);
    if (dp && dp->lockInPlace())
      act->setEnabled(false);
    else
      act->connect(iAction::int_act, this,  SLOT(CloseTab(int)));
  }
  // pinning/unpinning only if not lockInPlace guy
  if (tab_idx < 0) return;
  if (!dp || dp->lockInPlace()) return;
  contextMenu->addSeparator();
  if (dp->pinned()) {
    act = new iAction("&Unpin",  dp, SLOT(Unpin()), Qt::CTRL+Qt::Key_P );
  } else {
    act = new iAction("&Pin in place",  dp, SLOT(Pin()), Qt::CTRL+Qt::Key_P );
  }//TODO
  act->setParent(contextMenu);
  contextMenu->addAction(act);
  
  // open the panel as a window (same as opening from context menu with "edit dialog"
  act = new iAction("&Open In Window",  dp, SLOT(OpenInWindow()), 0);
  act->setParent(contextMenu);
  contextMenu->addAction(act);
}

void iTabView::ShowLink(taiSigLink* link, bool not_in_cur) {
  iPanelBase* pan = GetDataPanel(link);
  ShowPanel(pan, not_in_cur);
}

//TODO
iPanelBase* iTabView::GetDataPanel(taiSigLink* link) {
  iPanelBase* rval;
  for (int i = 0; i < panels.size; ++i) {
    rval = panels.FastEl(i);
    if (rval->link() == link) return rval;
  }

  rval = link->CreateDataPanel();
  if (rval != NULL) {
    AddPanel(rval);
    //note: we don't do a show() here because it automatically raises the panel
    //    rval->show(); //needed!
  }
  return rval;
}

void iTabView::GetWinState() {
  //TODO: may want to save state of what panels are active
  for (int i = 0; i < panelCount(); ++i) {
    iPanelBase* pn = panel(i);
    if (pn) pn->GetWinState();
  }
}

void iTabView::SetWinState() {
  //TODO: may want to get state of what panels are active
  for (int i = 0; i < panelCount(); ++i) {
    iPanelBase* pn = panel(i);
    if (pn) pn->SetWinState();
  }
}

void iTabView::ShowTab(iPanelBase* panel, bool show, bool focus) {
  // this is for ctrl panel frames that go visible, to show their ctrl panel tabs
  // note that we are assuming for simplicity that we can focus the default or 0th tab
  // when removing a tab for a visible ctrl guy
  if (show) {
    // may be there already, prob most recent...
    int tb = -1;
    for (int i = tabCount() - 1; i >= 0; --i) {
      iPanelBase* tpan = tabPanel(i);
      if (tpan == panel) {
        tb = i;
        break;
      }
    }
    if (tb < 0) {
      tb = tbPanels->addTab(panel);
    }
    // focus it
    if (focus)
      SetCurrentTab(tb);
  } else {
    for (int i = tabCount() - 1; i >= 0; --i) {
      iPanelBase* tpan = tabPanel(i);
      if (tpan == panel) {
        tbPanels->removeTab(i);
        if (tpan->isVisible()) {
          SetCurrentTab(0); // should be the non-ctrl default edit guy...
        }
        return;
      }
    }
  }
}

iPanelBase* iTabView::panel(int pan_idx) {
  return panels.PosSafeEl(pan_idx);
}

int iTabView::panelCount() const {
  return panels.size;
}

void iTabView::panelSelected(int idx) {
  iPanelBase* panel = NULL;
  if (idx >= 0) {
    panel = tbPanels->panel(idx);
  }
  // select the tree item that goes with the panel
  if (panel) {
    if (tbPanels->TabWasSelected()) {  // only do this selection if the user clicked on the tab
      tbPanels->ClearTabWasSelected();
      taiSigLink* sig_link = panel->link();
      if (sig_link && viewerWindow()) {
        iTreeView* tree_view = viewerWindow()->GetMainTreeView();
        if (tree_view) {
          iTreeViewItem* item = tree_view->AssertItem(sig_link);
          // if T3 panel item will be null
          if (item) {
            if (sig_link->taData()->InheritsFrom(&TA_taDataView)) {
              tree_view->clearSelection();
            }
            else {
              tree_view->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
            }
          }
          tree_view->update();
        }
      }
    }
    wsPanels->setCurrentWidget(panel);
  }
  else {
    wsPanels->setCurrentIndex(-1);
  }
  ++(tabViewerWin()->tab_changing);
  if (m_viewer_win) {
    m_viewer_win->TabView_Selected(this);
  }
  --(tabViewerWin()->tab_changing);
}

void iTabView::OnWindowBind(iPanelViewer* itv) {
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iPanelBase* pan = tbPanels->panel(i);
    if (!pan) continue; // can happen!!!
    pan->OnWindowBind(itv);
  }
}

void iTabView::Refresh() {
  for (int i = panels.size - 1; i >= 0; --i) {
    iPanelBase* panel = panels.FastEl(i);
    panel->UpdatePanel();
  }
  UpdateTabNames();
}

void iTabView::RemoveDataPanel(iPanelBase* panel) {
  // we guard for destructing case by clearing panels, so don't detect it
  if (panels.RemoveEl(panel)) { // Remove unrefs us in panel
    wsPanels->removeWidget(panel); // superfluous, but safe, if panel is destroying
    // remove any associated tabs, except leave last non-locked tab (will get deleted anyway if we are
    // destructing)
    for (int i = tbPanels->count() - 1; i >= 0; --i) {
      iPanelBase* dp = tbPanels->panel(i);
      if (dp == panel) {
        if ((i > 0) || (tbPanels->count() > 1)) {
          tbPanels->removeTab(i);
          // next tab focuses, but doesn't send us an event, so...
          // activate next, unless we removed last
          int fi = i;
          if (fi >= tbPanels->count())
            --fi;
          SetCurrentTab(fi);
        } else {
          tbPanels->SetPanel(0, NULL); // no panel
        }
      }
    }
  }
}

void iTabView::ResolveChanges(CancelOp& cancel_op) {
  for (int i = panels.size - 1; i >= 0; --i) {
    iPanelBase* panel = panels.FastEl(i);
    panel->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return; // can stop now
  }
}

bool iTabView::SetCurrentTab(int tab_idx) {
  if (tab_idx < 0 || tab_idx >= tbPanels->count()) return false;
  iPanelBase* pan = tabPanel(tab_idx);
  if (!pan) return false;
  ++(tabViewerWin()->tab_changing);
  tbPanels->setCurrentIndex(tab_idx);
  wsPanels->setCurrentWidget(pan);
  pan->show();                  // make it visible for sure!
  --(tabViewerWin()->tab_changing);
  return true;
}

bool iTabView::SetCurrentTabName(const String& tab_nm) {
  int tab_idx = TabIndexByName(tab_nm);
  if(tab_idx >= 0) {
    return SetCurrentTab(tab_idx);
  }
  return false;
}

void iTabView::ShowPanel(iPanelBase* panel, bool not_in_cur) {
  if (!panel) return;
  //note: panel has typically been added already, but we double check

  iPanelBase* cur_pn = curPanel(); //note: can be null

  // first, see if we have a tab for guy already -- don't create more than 1 per guy
  if (ActivatePanel(panel->link())) return;

  // always create a new tab for lockinplace guys
  if (panel->lockInPlace()) {
    AddPanelNewTab(panel);
    return;
  }

  // ok, so we'll either replace cur panel, swap one out, or make a new

  // replace curr if allowed and it is not locked, pinned, or dirty+autocommit
  if (!not_in_cur && cur_pn && (!cur_pn->lockInPlace() && !cur_pn->pinned() &&
      (!cur_pn->dirty() || autoCommit())))
  {
    bool proceed = true;
    if (cur_pn->dirty()) { // must be autocommit
      CancelOp cancel_op = CO_PROCEED;
      cur_pn->ResolveChanges(cancel_op);
      proceed = (cancel_op == CO_PROCEED);
    }
    if (proceed) {
      tbPanels->SetPanel(tbPanels->currentIndex(), panel);
      wsPanels->setCurrentWidget(panel);
      return;
    }
  }

  // try switching to another eligible panel
  for (int i = 0; i < tbPanels->count(); ++i) {
    iPanelBase* pn = tbPanels->panel(i);
    if (pn) {
      if (pn == cur_pn) continue;
      if (pn->lockInPlace() || (pn->dirty() && !autoCommit()) || pn->pinned()) continue;
    }
    bool proceed = true;
    if (pn && pn->dirty()) { // must be autocommit
      CancelOp cancel_op = CO_PROCEED;
      pn->ResolveChanges(cancel_op);
      proceed = (cancel_op == CO_PROCEED);
    }
    if (proceed) {
      tbPanels->SetPanel(i, panel);
      SetCurrentTab(i);
      return;
    }
  }

  // no eligible one, so make new
  AddPanelNewTab(panel);
}

int iTabView::tabCount() const {
  return tbPanels->count();
}

iPanelBase* iTabView::tabPanel(int tab_idx) {
  int ct = tabCount();
  if ((tab_idx < 0) || (tab_idx >= ct)) return NULL;
  return tbPanels->panel(tab_idx);
}

int iTabView::TabIndexOfPanel(iPanelBase* panel) const {
  for (int i = 0; i < tbPanels->count(); ++i) {
    if (tbPanels->panel(i) == panel) return i;
  }
  return -1;
}

int iTabView::TabIndexByName(const String& nm) const {
  for (int i = 0; i < tbPanels->count(); ++i) {
    if (tbPanels->panel(i)->TabText() == nm) return i;
  }
  return -1;
}


void iTabView::UpdateTabNames() { // called by a siglink when a tab name might have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iPanelBase* pan = tbPanels->panel(i);
    if (pan == NULL) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
    tbPanels->setTabToolTip(i, pan->TabText()); // esp for when elided
    tbPanels->setTabIcon(i,  pan->tabIcon());
  }
}

void iTabView::UpdateTabName(iPanelBase* pan) { // called by a panel when its tab name may have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iPanelBase* pani = tbPanels->panel(i);
    if (pan != pani) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
    tbPanels->setTabToolTip(i, pan->TabText()); // esp for when elided
    tbPanels->setTabIcon(i,  pan->tabIcon());
  }
}

void iTabView::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::PROJECTWINDOW_CONTEXT, key_event);
  switch (action) {
    case taiMisc::PROJECTWINDOW_TOGGLE_PANEL_PIN: // move left between regions
    case taiMisc::PROJECTWINDOW_TOGGLE_PANEL_PIN_II: // move left between regions
    {
      iPanelBase* dp = curPanel();
      if(dp) {
        if(dp->pinned())
          dp->Unpin();
        else
          dp->Pin();
      }
      key_event->accept();
      return;
    }
    default:
      QWidget::keyPressEvent(key_event);
  }
}
