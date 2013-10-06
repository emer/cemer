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

#include "iT3PanelViewer.h"

#include <T3PanelViewer>
#include <iT3Panel>
#include <T3Panel>
#include <T3DataViewMain>
#include <iT3ViewspaceWidget>
#include <iTabBarBase>
#include <taiWidgetMenu>
#include <iMainWindowViewer>
#include <MainWindowViewer>
#include <iVec2i>

#include <taMisc>
#include <taiMisc>


#include <QVBoxLayout>
#include <QTimer>


iT3PanelViewer::iT3PanelViewer(T3PanelViewer* viewer_, QWidget* parent)
:inherited(viewer_, parent)
{
  Init();
}

iT3PanelViewer::~iT3PanelViewer() {
}

void iT3PanelViewer::Init() {
  last_idx = -1;
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setSpacing(0);  lay->setMargin(0);
  tw = new iTabWidget(this); //top, standard tabs
#if (QT_VERSION >= 0x040200)
  tw->setUsesScrollButtons(true);
  tw->setElideMode(Qt::ElideMiddle/*Qt::ElideNone*/); // fixed now..
  //  tw->setElideMode(Qt::ElideNone); // don't elide, because it does it even when enough room, and it is ugly and confusing
#endif
  lay->addWidget(tw);
  iTabBarBase* itb = tw->GetTabBar();

  connect(tw, SIGNAL(customContextMenuRequested2(const QPoint&, int)),
          this, SLOT(tw_customContextMenuRequested2(const QPoint&, int)) );
  connect(tw, SIGNAL(currentChanged(int)),
          this, SLOT(tw_currentChanged(int)) );

  connect(itb, SIGNAL(tabMoved(int, int)),
          this, SLOT(tw_tabMoved(int, int)) );

  // punt, and just connect a timer to focus first tab
  // if any ProcessEvents get called (should not!) may have to make non-zero time
  QTimer::singleShot(0, this, SLOT(FocusFirstTab()) );
}

void iT3PanelViewer::AddT3Panel(iT3Panel* ipanl, int idx) {
  T3Panel* panl = ipanl->viewer();
  String tab_label = panl->GetName();
  if (idx < 0)
    idx = tw->addTab(ipanl, tab_label);
  else
    tw->insertTab(idx, ipanl, tab_label);
  tw->setTabToolTip(idx, tab_label);
  if(ipanl->t3vs) {
    ipanl->t3vs->Connect_SelectableHostNotifySignal(this,
      SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  }
  tw->setCurrentIndex(idx); // not selected automatically
}

void iT3PanelViewer::AddPanel() {
// T3Panel* fr  =
 viewer()->NewT3Panel();
}

void iT3PanelViewer::DeletePanel(int tab_idx) {
  T3Panel* fr = viewPanel(tab_idx);
  if (!fr) return;
  fr->Close();
  //NOTE: do not place any code here -- we are deleted!
}

void iT3PanelViewer::PanelProperties(int tab_idx) {
  T3Panel* fr = viewPanel(tab_idx);
  if (!fr) return;
  fr->EditDialog(false);        // non-modal so it can open other dialogs..
}

void iT3PanelViewer::FirstViewProperties(int tab_idx) {
  T3Panel* fr = viewPanel(tab_idx);
  if (!fr) return;
  T3DataViewMain* dvm = fr->FirstChild();
  if(!dvm) return;
  dvm->EditDialog(false);
}

void iT3PanelViewer::FillContextMenu_impl(taiWidgetMenu* menu, int tab_idx) {
   menu->AddItem("&Add Panel", iAction::action,
                 this, SLOT(AddPanel()),_nilVariant);

  if (tab_idx >= 0) {
     menu->AddItem("&Delete Panel", iAction::int_act,
                   this, SLOT(DeletePanel(int)), tab_idx);

    menu->AddSep();
    // should always be at bottom:
    menu->AddItem("Panel &Properties...", iAction::int_act,
                  this, SLOT(PanelProperties(int)), tab_idx);

    menu->AddItem("&1st View Properties...", iAction::int_act,
                  this, SLOT(FirstViewProperties(int)), tab_idx);

  }
}

void iT3PanelViewer::FocusFirstTab() {
  if (tw->count() > 0) {
    if (tw->currentIndex() == 0) {
      tw_currentChanged(0);
    } else {
      tw->setCurrentIndex(0);
    }
  }

}

void iT3PanelViewer::tw_customContextMenuRequested2(const QPoint& pos, int tab_idx) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  FillContextMenu_impl(menu, tab_idx);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iT3PanelViewer::tw_currentChanged(int tab_idx) {
//note: the backwards order below fulfills two competing requirements:
// 1) have a hide/show insures we don't get multiple tabs
// 2) but show/hide order prevents panel tab switching away
// TODO: known bug: when you delete a panel, it switches from CtrlPanel tab
// different logic required when tab numbers are the same!
  iT3Panel* ipanl;
  if(tab_idx == last_idx) {
    ipanl = iViewPanel(tab_idx);
    ipanl->Showing(true);
  }
  else {
    ipanl = iViewPanel(tab_idx);
    if (ipanl) { // should exist
      ipanl->Showing(true);
      ipanl = iViewPanel(last_idx);
      if (ipanl) {
        ipanl->Showing(false);
      }
    }
  }
  last_idx = tab_idx;
}

iT3Panel* iT3PanelViewer::iViewPanel(int idx) const {
  iT3Panel* rval = NULL;
  if ((idx >= 0) && (idx < tw->count())) {
    rval = qobject_cast<iT3Panel*>(tw->widget(idx));
  }
  return rval;
}

T3Panel* iT3PanelViewer::viewPanel(int idx) const {
  iT3Panel* ipanl = iViewPanel(idx);
  if (ipanl) return ipanl->viewer();
  else return NULL;
}


void iT3PanelViewer::Refresh_impl() {
  for (int i = 0; i < viewer()->panels.size; ++i) {
    T3Panel* panl = viewer()->panels.FastEl(i);
    iT3Panel* ipanl = panl->widget();
    if (!ipanl) continue;
    ipanl->Refresh();
  }
  UpdateTabNames();
  inherited::Refresh_impl(); // prob nothing
}

void iT3PanelViewer::UpdateTabNames() {
  for (int i = 0; i < viewer()->panels.size; ++i) {
    T3Panel* panl = viewer()->panels.FastEl(i);
    iT3Panel* ipanl = panl->widget();
    if (!ipanl) continue;
    int idx = tw->indexOf(ipanl);
    if (idx >= 0) {
      tw->setTabText(idx, panl->GetName());
      tw->setTabToolTip(idx, panl->GetName());
    }
  }
}

int iT3PanelViewer::TabIndexByName(const String& nm) const {
  for (int i = 0; i < tw->count(); ++i) {
    if(tw->tabText(i) == nm) return i;
  }
  return -1;
}

bool iT3PanelViewer::SetCurrentTab(int tab_idx) {
  if(tab_idx < 0 || tab_idx >= tw->count()) return false;
  tw->setCurrentIndex(tab_idx);
  return true;
}

bool iT3PanelViewer::SetCurrentTabName(const String& tab_nm) {
  int tab_idx = TabIndexByName(tab_nm);
  if(tab_idx >= 0) {
    return SetCurrentTab(tab_idx);
  }
  return false;
}

void iT3PanelViewer::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  MainWindowViewer* mwv = viewer()->mainWindowViewer();
  if(mwv && mwv->widget()) {
    mwv->widget()->FocusIsRightViewer();
  }
}

void iT3PanelViewer::tw_tabMoved(int fm, int to) {
  viewer()->panels.MoveIdx(fm, to);
}

