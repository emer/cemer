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

iTabView::iTabView(QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = NULL;
  Init();
}

iTabView::iTabView(iTabViewer* data_viewer_, QWidget* parent)
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

bool iTabView::ActivatePanel(taiDataLink* dl) {
  for (int i = 0; i < tbPanels->count(); ++i) {
    iDataPanel* panel = tbPanels->panel(i);
    if (!panel) continue;
    if (panel->link() == dl) {
      SetCurrentTab(i);
      return true;
    }
  }
  return false;
}

bool iTabView::AddPanel(iDataPanel* panel) {
  if (!panels.AddUnique(panel)) return false; // refs us on add
  wsPanels->addWidget(panel);
  if (panels.size == 1) wsPanels->setCurrentWidget(panel); // always show first
  iTabViewer* itv = tabViewerWin();
  if (itv) panel->OnWindowBind(itv);
  return true;
}

void iTabView::AddPanelNewTab(iDataPanel* panel, bool lock) {
  AddPanel(panel); //noop if already added
  if (lock) panel->Pin();
  int tab_idx = TabIndexOfPanel(panel);
  if(tab_idx < 0)
    tab_idx = tbPanels->addTab(panel);
  SetCurrentTab(tab_idx);
}

void iTabView::AddTab(int tab_idx) {
  iDataPanel* pan = NULL;
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
    tbPanels->removeTab(tab);
    panelSelected(tbPanels->currentIndex()); // needed to resync proper panel with tab
  } else { // last tab
    panelSelected(-1);
  }
}

void iTabView::Closing(CancelOp& cancel_op) {
  // close all panels
  for (int i = panels.size - 1; i >= 0; --i) {
    iDataPanel* panel = panels.FastEl(i);
    panel->Closing(cancel_op);
    if (cancel_op == CO_CANCEL) return; // can stop now

    RemoveDataPanel(panel); // note: removes from tabs, and deletes tabs
  }
}

iDataPanel* iTabView::curPanel() const {
  iDataPanel* rval = tbPanels->panel(tbPanels->currentIndex());
  return rval; // could be NULL if empty
}

void iTabView::DataPanelDestroying(iDataPanel* panel) {
  RemoveDataPanel(panel);//TODO
}

void iTabView::FillTabBarContextMenu(QMenu* contextMenu, int tab_idx) {
  iDataPanel* dp = tabPanel(tab_idx); // always safe, NULL if no tab
  // note: need to (re)parent the actions; not parented by adding to menu
  taiAction* act = new taiAction(tab_idx, "&Add Tab",  QKeySequence());
  act->connect(taiAction::int_act, this,  SLOT(AddTab(int)));
  act->setParent(contextMenu);
  contextMenu->addAction(act);
  // only add Close if on a tab
  if (tab_idx >= 0) {
    // always add for consistency, even if on an empty or locked guy
    act = new taiAction(tab_idx, "&Close Tab", QKeySequence());
    act->setParent(contextMenu);
    contextMenu->addAction(act);
    if (dp && dp->lockInPlace())
      act->setEnabled(false);
    else
      act->connect(taiAction::int_act, this,  SLOT(CloseTab(int)));
  }
  // pinning/unpinning only if not lockInPlace guy
  if (tab_idx < 0) return;
  if (!dp || dp->lockInPlace()) return;
  contextMenu->addSeparator();
  if (dp->pinned()) {
    act = new taiAction("&Unpin",  dp, SLOT(Unpin()), CTRL+Key_P );
  } else {
    act = new taiAction("&Pin in place",  dp, SLOT(Pin()), CTRL+Key_P );
  }//TODO
  act->setParent(contextMenu);
  contextMenu->addAction(act);
}

void iTabView::ShowLink(taiDataLink* link, bool not_in_cur) {
  iDataPanel* pan = GetDataPanel(link);
  ShowPanel(pan, not_in_cur);
}

//TODO
iDataPanel* iTabView::GetDataPanel(taiDataLink* link) {
  iDataPanel* rval;
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
    iDataPanel* pn = panel(i);
    if (pn) pn->GetWinState();
  }
}

void iTabView::SetWinState() {
//TODO: may want to get state of what panels are active
  for (int i = 0; i < panelCount(); ++i) {
    iDataPanel* pn = panel(i);
    if (pn) pn->SetWinState();
  }
}

void iTabView::ShowTab(iDataPanel* panel, bool show, bool focus) {
// this is for ctrl panel frames that go visible, to show their ctrl panel tabs
// note that we are assuming for simplicity that we can focus the default or 0th tab
// when removing a tab for a visible ctrl guy
  if (show) {
    // may be there already, prob most recent...
    int tb = -1;
    for (int i = tabCount() - 1; i >= 0; --i) {
      iDataPanel* tpan = tabPanel(i);
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
      iDataPanel* tpan = tabPanel(i);
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

iDataPanel* iTabView::panel(int pan_idx) {
  return panels.PosSafeEl(pan_idx);
}

int iTabView::panelCount() const {
  return panels.size;
}

void iTabView::panelSelected(int idx) {
  iDataPanel* panel = NULL;
  if (idx >= 0) panel = tbPanels->panel(idx);
  ++(tabViewerWin()->tab_changing);
  if (panel) {
    wsPanels->setCurrentWidget(panel);
  } else {
    wsPanels->setCurrentIndex(-1);
  }
  if (m_viewer_win)
    m_viewer_win->TabView_Selected(this);
  --(tabViewerWin()->tab_changing);
}

void iTabView::OnWindowBind(iTabViewer* itv) {
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pan = tbPanels->panel(i);
    if (!pan) continue; // can happen!!!
    pan->OnWindowBind(itv);
  }
}

void iTabView::Refresh() {
  for (int i = panels.size - 1; i >= 0; --i) {
    iDataPanel* panel = panels.FastEl(i);
    panel->UpdatePanel();
  }
  UpdateTabNames();
}

void iTabView::RemoveDataPanel(iDataPanel* panel) {
  // we guard for destructing case by clearing panels, so don't detect it
  if (panels.RemoveEl(panel)) { // Remove unrefs us in panel
    wsPanels->removeWidget(panel); // superfluous, but safe, if panel is destroying
    // remove any associated tabs, except leave last non-locked tab (will get deleted anyway if we are
    // destructing)
    for (int i = tbPanels->count() - 1; i >= 0; --i) {
      iDataPanel* dp = tbPanels->panel(i);
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
    iDataPanel* panel = panels.FastEl(i);
    panel->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return; // can stop now
  }
}

bool iTabView::SetCurrentTab(int tab_idx) {
  if (tab_idx < 0 || tab_idx >= tbPanels->count()) return false;
  iDataPanel* pan = tabPanel(tab_idx);
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

void iTabView::ShowPanel(iDataPanel* panel, bool not_in_cur) {
  if (!panel) return;
  //note: panel has typically been added already, but we double check

  iDataPanel* cur_pn = curPanel(); //note: can be null

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
    iDataPanel* pn = tbPanels->panel(i);
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

iDataPanel* iTabView::tabPanel(int tab_idx) {
  int ct = tabCount();
  if ((tab_idx < 0) || (tab_idx >= ct)) return NULL;
  return tbPanels->panel(tab_idx);
}

int iTabView::TabIndexOfPanel(iDataPanel* panel) const {
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


void iTabView::UpdateTabNames() { // called by a datalink when a tab name might have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pan = tbPanels->panel(i);
    if (pan == NULL) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
    tbPanels->setTabToolTip(i, pan->TabText()); // esp for when elided
    tbPanels->setTabIcon(i,  pan->tabIcon());
  }
}

void iTabView::UpdateTabName(iDataPanel* pan) { // called by a panel when its tab name may have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pani = tbPanels->panel(i);
    if (pan != pani) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
    tbPanels->setTabToolTip(i, pan->TabText()); // esp for when elided
    tbPanels->setTabIcon(i,  pan->tabIcon());
  }
}

void iTabView::keyPressEvent(QKeyEvent* e) {
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    if(e->key() == Qt::Key_P) {
      iDataPanel* dp = curPanel();
      if(dp) {
        if(dp->pinned())
          dp->Unpin();
        else
          dp->Pin();
      }
      e->accept();
      return;
    }
//     else if(e->key() == Qt::Key_T) {
//       AddTab(tbPanels->currentIndex());
//       e->accept();
//       return;
//     }
    // I don't think it is useful to have a command for this -- too obscure and dangerous
//     else if(e->key() == Qt::Key_D) {
//       CloseTab(tbPanels->currentIndex());
//       e->accept();
//       return;
//     }
  }
  QWidget::keyPressEvent(e);
}


