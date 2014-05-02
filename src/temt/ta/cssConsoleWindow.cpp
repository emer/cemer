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

#include "cssConsoleWindow.h"
#include <css_qtconsole.h>
#include <QHBoxLayout>
#include <QToolBar>
#include <iAction>
#include <QApplication>
#include <tabMisc>
#include <taRootBase>
#include <taProject>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <taiMisc>
#include <iRect>
#include <QIcon>


cssConsoleWindow::cssConsoleWindow(QWidget* parent)
  : inherited(parent) {

  lock_to_proj = true;
  in_self_resize = false;

  css_con = QcssConsole::getInstance(NULL, cssMisc::TopShell);

  QWidget* bod = new QWidget(this);
  setCentralWidget(bod);
  QHBoxLayout* hb = new QHBoxLayout(bod);
  hb->setMargin(0); hb->setSpacing(0);
  QToolBar* tb = new QToolBar(bod);
  tb->setOrientation(Qt::Vertical);
  tb->setMovable(false);
  tb->setFloatable(false);
  tb->setIconSize(QSize(16,16));

  hb->addWidget(tb);

  hb->addWidget(css_con);

  unpinned = new QIcon(":/images/tab_unpushed.png");
  pinned = new QIcon(":/images/tab_locked.png");

  pin_act = new iAction("&pin", QKeySequence(), "pin");
  pin_act->setIcon(*pinned);

  connect(pin_act, SIGNAL(Action()), this, SLOT(PinAction()));

  tb->addAction(pin_act);
  
  setWindowTitle("css Console");
}

cssConsoleWindow::~cssConsoleWindow() {
  delete pinned;
  delete unpinned;
  delete pin_act;
}

void cssConsoleWindow::UpdateFmLock() {
  if(lock_to_proj) 
    pin_act->setIcon(*pinned);
  else
    pin_act->setIcon(*unpinned);

  if(lock_to_proj) {
    taProject* proj = tabMisc::root->projects.DefaultEl();
    if(proj) {
      MainWindowViewer* mwv = proj->GetDefaultProjectBrowser();
      if(mwv && mwv->widget()) {
        mwv->widget()->AlignCssConsole();
      }
    }
  }
  else {
    LoadGeom();
  }
}

void cssConsoleWindow::SaveGeom() {
  iRect r = frameGeometry(); //note: same as size() for widgets
  // convert from screen coords to relative (note, allowed to be >1.0)
  // adjust for scrn geom, esp for evil mac
  float lft = (float)(r.left() - taiM->scrn_geom.left()) /
    (float)(taiM->scrn_s.w); // all of these convert from screen coords
  float top = (float)(r.top() - taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h);
  float wd = (float)r.width() / (float)(taiM->scrn_s.w);
  float ht = (float)r.height() / (float)(taiM->scrn_s.h);
  
  tabMisc::root->console_size.SetXY(wd, ht);
  tabMisc::root->console_pos.SetXY(lft, top);
}

void cssConsoleWindow::LoadGeom() {
  if(tabMisc::root->console_size == 0.0f) return;

  float lft = tabMisc::root->console_pos.x;
  float top = tabMisc::root->console_pos.y;
  float wd = tabMisc::root->console_size.x;
  float ht = tabMisc::root->console_size.y;

  iSize s((int)(wd * taiM->scrn_s.w), (int)(ht * taiM->scrn_s.h));
  s.h -= taiM->frame_s.h;
  s.w -= taiM->frame_s.w;
  iRect tr = iRect(
    (int)((lft * taiM->scrn_s.w)) + taiM->scrn_geom.left(),
    (int)((top * taiM->scrn_s.h)) + taiM->scrn_geom.top(),
    s.w,
    s.h
  );

  // note: resize/move combo recommended by Qt
  in_self_resize = true;
  resize(tr.w, tr.h);
  move(tr.x, tr.y);
  in_self_resize = false;
}

void cssConsoleWindow::PinAction() {
  lock_to_proj = !lock_to_proj;
  UpdateFmLock();
}

void cssConsoleWindow::LockedNewGeom(int left, int top, int width, int height) {
  if(!lock_to_proj) return;
  in_self_resize = true;
  resize(width, height);
  move(left, top);
  in_self_resize = false;
  css_con->gotoEnd();
}

void cssConsoleWindow::resizeEvent(QResizeEvent* e) {
  inherited::resizeEvent(e);
  if(in_self_resize) return;

  taProject* proj = tabMisc::root->projects.DefaultEl();
  if(!proj) return;             // only functions if there is a project

  if(!lock_to_proj) {
    SaveGeom();
  }
  else {
    lock_to_proj = false;
    UpdateFmLock();
  }
}

void cssConsoleWindow::moveEvent(QMoveEvent* e) {
  inherited::moveEvent(e);
  if(in_self_resize) return;
  taProject* proj = tabMisc::root->projects.DefaultEl();
  if(!proj) return;             // only functions if there is a project

  if(!lock_to_proj) {
    SaveGeom();
  }
  else {
    lock_to_proj = false;
    UpdateFmLock();
  }
}
