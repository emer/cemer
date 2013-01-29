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

#include "iTabBar.h"
#include <iTabView>
#include <iDataPanel>

#include <QIcon>
#include <QMenu>
#include <QContextMenuEvent>


QIcon* iTabBar::tab_icon[iTabBar::TI_LOCKED + 1];

void iTabBar::InitClass() {
  // load pushpin icons
  tab_icon[TI_UNPINNED] = new QIcon(":/images/tab_unpushed.png");
  tab_icon[TI_PINNED] = new QIcon(":/images/tab_pushed.png");
  tab_icon[TI_LOCKED] = new QIcon(":/images/tab_locked.png");
}

iTabBar::iTabBar(iTabView* parent_)
:inherited((QWidget*)parent_)
{
  defPalette = palette();
}

iTabBar::~iTabBar() {
}

int iTabBar::addTab(iDataPanel* pan) {
  // if it is a lock guy, add to end, otherwise insert before lock guys
  int idx = 0;
  if (pan && pan->lockInPlace()) {
    idx = inherited::addTab("");
  }
  else {
    while (idx < count()) {
      iDataPanel* tpan = panel(idx);
      if (tpan && tpan->lockInPlace()) break; // found 1st guy
      ++idx;
    }
    idx = inherited::insertTab(idx, ""); // idx result is normally the same
  }
  setTabData(idx, QVariant((ta_intptr_t)0)); // set to valid null value
  SetPanel(idx, pan);
  return idx;
}

void iTabBar::contextMenuEvent(QContextMenuEvent * e) {
  // find the tab being clicked, or -1 if none
  int idx = count() - 1;
  while (idx >= 0) {
    if (tabRect(idx).contains(e->pos())) break;
    --idx;
  }
  // put up the context menu
  QMenu* menu = new QMenu(this);
  tabView()->FillTabBarContextMenu(menu, idx);
  menu->exec(QCursor::pos());
  delete menu;
}

void iTabBar::mousePressEvent(QMouseEvent* e) {
  inherited::mousePressEvent(e);
  if (tabView()->m_viewer_win)
    tabView()->m_viewer_win->TabView_Selected(tabView());
}

bool iTabBar::focusNextPrevChild(bool next) {
  if(!next) return inherited::focusNextPrevChild(next);

  int idx = currentIndex();
  iDataPanel* tpan = panel(idx);
  if(!tpan) return inherited::focusNextPrevChild(next);
  QWidget* nxt = tpan->firstTabFocusWidget();
  if(!nxt) return inherited::focusNextPrevChild(next);
  nxt->setFocus();
  return true;
}

iDataPanel* iTabBar::panel(int idx) {
  QVariant data(tabData(idx)); // returns NULL variant if out of range
  if (data.isNull() || !data.isValid()) return NULL;
  ta_intptr_t rval = data.value<ta_intptr_t>(); //NOTE: if probs in msvc, use the qvariant_cast thingy
  return (iDataPanel*)rval;

}

void iTabBar::setTabIcon(int idx, TabIcon ti) {
  QIcon* ico = NULL;
  if (ti != TI_NONE)
    ico = tab_icon[ti];
  if (ico)
    inherited::setTabIcon(idx, *ico);
  else
    inherited::setTabIcon(idx, QIcon());
}

void iTabBar::SetPanel(int idx, iDataPanel* value, bool force) {
  QVariant data(tabData(idx)); // returns NULL variant if out of range
  iDataPanel* m_panel = NULL;
  if (!data.isNull() && data.isValid())
    m_panel = (iDataPanel*)data.value<ta_intptr_t>(); //NOTE: if probs in msvc, use the qvariant_cast thingy

  if ((m_panel == value) && !force) goto set_cur;
  m_panel = value;
  if (m_panel) {
    setTabText(idx, m_panel->TabText());
    setTabToolTip(idx, m_panel->TabText());
    setTabIcon(idx, m_panel->tabIcon());
//    m_panel->mtab_cnt++;
    // make sure we show latest data (helps in case there are "holes" in data updating)
    m_panel->UpdatePanel();
  } else {
    setTabText(idx, "");
    setTabToolTip(idx, "");
    setTabIcon(idx, TI_NONE); // no icon
  }
  data = (ta_intptr_t)m_panel;
  setTabData(idx, data);

set_cur:
;/*don't autoset  if (currentIndex() != idx)
    setCurrentIndex(idx); */
}

