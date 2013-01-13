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

#include "iT3DataViewer.h"

#include <T3DataViewer>
#include <iT3DataViewFrame>
#include <T3DataViewFrame>
#include <iTabBarBase>


iT3DataViewer::iT3DataViewer(T3DataViewer* viewer_, QWidget* parent)
:inherited(viewer_, parent)
{
  Init();
}

iT3DataViewer::~iT3DataViewer() {
}

void iT3DataViewer::Init() {
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

void iT3DataViewer::AddT3DataViewFrame(iT3DataViewFrame* idvf, int idx) {
  T3DataViewFrame* dvf = idvf->viewer();
  String tab_label = dvf->GetName();
  if (idx < 0)
    idx = tw->addTab(idvf, tab_label);
  else
    tw->insertTab(idx, idvf, tab_label);
  tw->setTabToolTip(idx, tab_label);
  idvf->t3vs->Connect_SelectableHostNotifySignal(this,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  tw->setCurrentIndex(idx); // not selected automatically
}

void iT3DataViewer::AddFrame() {
// T3DataViewFrame* fr  =
 viewer()->NewT3DataViewFrame();
}

void iT3DataViewer::DeleteFrame(int tab_idx) {
  T3DataViewFrame* fr = viewFrame(tab_idx);
  if (!fr) return;
  fr->Close();
  //NOTE: do not place any code here -- we are deleted!
}

void iT3DataViewer::FrameProperties(int tab_idx) {
  T3DataViewFrame* fr = viewFrame(tab_idx);
  if (!fr) return;
  fr->EditDialog(true);
}

void iT3DataViewer::FillContextMenu_impl(taiMenu* menu, int tab_idx) {
  taiAction*
  act = menu->AddItem("&Add Frame", taiAction::action,
    this, SLOT(AddFrame()),_nilVariant);

  if (tab_idx >= 0) {
    act = menu->AddItem("&Delete Frame", taiAction::int_act,
      this, SLOT(DeleteFrame(int)), tab_idx);

    menu->AddSep();
    // should always be at bottom:
    act = menu->AddItem("Frame &Properties...", taiAction::int_act,
      this, SLOT(FrameProperties(int)), tab_idx);

  }
}

void iT3DataViewer::FocusFirstTab() {
  if (tw->count() > 0) {
    if (tw->currentIndex() == 0) {
      tw_currentChanged(0);
    } else {
      tw->setCurrentIndex(0);
    }
  }

}

void iT3DataViewer::tw_customContextMenuRequested2(const QPoint& pos, int tab_idx) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  FillContextMenu_impl(menu, tab_idx);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iT3DataViewer::tw_currentChanged(int tab_idx) {
//note: the backwards order below fulfills two competing requirements:
// 1) have a hide/show insures we don't get multiple tabs
// 2) but show/hide order prevents panel tab switching away
// TODO: known bug: when you delete a frame, it switches from CtrlPanel tab
// different logic required when tab numbers are the same!
  iT3DataViewFrame* idvf;
  if(tab_idx == last_idx) {
    idvf = iViewFrame(tab_idx);
    idvf->Showing(true);
  }
  else {
    idvf = iViewFrame(tab_idx);
    if (idvf) { // should exist
      idvf->Showing(true);
      idvf = iViewFrame(last_idx);
      if (idvf) {
        idvf->Showing(false);
      }
    }
  }
  last_idx = tab_idx;
}

iT3DataViewFrame* iT3DataViewer::iViewFrame(int idx) const {
  iT3DataViewFrame* rval = NULL;
  if ((idx >= 0) && (idx < tw->count())) {
    rval = qobject_cast<iT3DataViewFrame*>(tw->widget(idx));
  }
  return rval;
}

T3DataViewFrame* iT3DataViewer::viewFrame(int idx) const {
  iT3DataViewFrame* idvf = iViewFrame(idx);
  if (idvf) return idvf->viewer();
  else return NULL;
}


void iT3DataViewer::Refresh_impl() {
  for (int i = 0; i < viewer()->frames.size; ++i) {
    T3DataViewFrame* dvf = viewer()->frames.FastEl(i);
    iT3DataViewFrame* idvf = dvf->widget();
    if (!idvf) continue;
    idvf->Refresh();
  }
  UpdateTabNames();
  inherited::Refresh_impl(); // prob nothing
}

void iT3DataViewer::UpdateTabNames() {
  for (int i = 0; i < viewer()->frames.size; ++i) {
    T3DataViewFrame* dvf = viewer()->frames.FastEl(i);
    iT3DataViewFrame* idvf = dvf->widget();
    if (!idvf) continue;
    int idx = tw->indexOf(idvf);
    if (idx >= 0) {
      tw->setTabText(idx, dvf->GetName());
      tw->setTabToolTip(idx, dvf->GetName());
    }
  }
}

int iT3DataViewer::TabIndexByName(const String& nm) const {
  for (int i = 0; i < tw->count(); ++i) {
    if(tw->tabText(i) == nm) return i;
  }
  return -1;
}

bool iT3DataViewer::SetCurrentTab(int tab_idx) {
  if(tab_idx < 0 || tab_idx >= tw->count()) return false;
  tw->setCurrentIndex(tab_idx);
  return true;
}

bool iT3DataViewer::SetCurrentTabName(const String& tab_nm) {
  int tab_idx = TabIndexByName(tab_nm);
  if(tab_idx >= 0) {
    return SetCurrentTab(tab_idx);
  }
  return false;
}

void iT3DataViewer::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  MainWindowViewer* mwv = viewer()->mainWindowViewer();
  if(mwv && mwv->widget()) {
    mwv->widget()->FocusIsRightViewer();
  }
}

void iT3DataViewer::tw_tabMoved(int fm, int to) {
  viewer()->frames.MoveIdx(fm, to);
}

