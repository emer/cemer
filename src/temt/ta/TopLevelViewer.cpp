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

#include "TopLevelViewer.h"
#include <taMisc>
#include <taiMisc>
#include <taProject>
#include <tabMisc>
#include <taRootBase>
#include <iRect>

#include <QWidget>
#include <QCoreApplication>
#include <QMainWindow>

TA_BASEFUNS_CTORS_DEFN(TopLevelViewer);


void TopLevelViewer::Initialize() {
}

void TopLevelViewer::InitLinks() {
  inherited::InitLinks();
  if (taMisc::is_loading && isTopLevel() && openOnLoad())
      taiMisc::unopened_windows.LinkUnique(this);
}

void TopLevelViewer::CutLinks() {
  inherited::CutLinks();
}


void TopLevelViewer::Copy_(const TopLevelViewer& cp) {
  if (!GetName().empty())
    SetName(GetName() + "_copy");
}

void TopLevelViewer::GetWinState_impl() {
  QWidget* widget = this->widget(); // cache

  iRect r = widget->frameGeometry(); //note: same as size() for widgets
  // convert from screen coords to relative (note, allowed to be >1.0)
  // adjust for scrn geom, esp for evil mac
  float lft = (float)(r.left() - taiM->scrn_geom.left()) /
    (float)(taiM->scrn_s.w); // all of these convert from screen coords
  float top = (float)(r.top() - taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h);
  float wd = (float)r.width() / (float)(taiM->scrn_s.w);
  float ht = (float)r.height() / (float)(taiM->scrn_s.h);
  bool iconified = widget->isMinimized();
  // save in UserData
  SetUserData("view_win_lft", lft);
  SetUserData("view_win_top", top);
  SetUserData("view_win_wd", wd);
  SetUserData("view_win_ht", ht);
  SetUserData("view_win_iconified", iconified);
}

void TopLevelViewer::SetWinState_impl() {
  float lft = GetUserDataDef("view_win_lft", 0.0f).toFloat();
  float top = GetUserDataDef("view_win_top", 0.0f).toFloat();
  float wd = GetUserDataDef("view_win_wd", 1.0f).toFloat();
  float ht = GetUserDataDef("view_win_ht", 1.0f).toFloat();
  bool iconified = GetUserDataDef("view_win_iconified", false).toBool();
  // adjust ht in case we are using the console win
  if (taMisc::console_win) {
    if (ht > 0.8f) ht = 0.8f;
  }

/*
  // make sure to limit on mac, due to wacky menubar and dock
  //TODO: maybe this should be used for all? ex. windows or kde taskbar etc.
#ifdef TA_OS_MAC
  if (taiM) { // guard for instance creation before sys init
    lft = MAX(lft, (taiM->scrn_s.w > 0) ?
      (float)(taiM->scrn_geom.left()) / (float)(taiM->scrn_s.w) : 0.0f);
    top = MAX(top, (taiM->scrn_s.h > 0) ?
      (float)(taiM->scrn_geom.top()) / (float)(taiM->scrn_s.h) : 0.0f);
  }
//TODO: prob should limit wd and ht too, because of dock, and inability to size if grip is offscreen
#endif */

  QWidget* widget = this->widget(); // cache

  // convert to pixels
  // we use the screen geom offsets, mostly for evil mac
  iSize s((int)(wd * taiM->scrn_s.w), (int)(ht * taiM->scrn_s.h));
  FrameSizeToSize(s); // only does anything for wins(int)(ht * taiM->scrn_s.h)
  iRect tr = iRect(
    (int)((lft * taiM->scrn_s.w)) + taiM->scrn_geom.left(),
    (int)((top * taiM->scrn_s.h)) + taiM->scrn_geom.top(),
    s.w,
    s.h
  );

  //note: resize/move combo recommended by Qt
  widget->resize(tr.w, tr.h);
  widget->move(tr.x, tr.y);
  if (iconified)
    widget->showMinimized();
  else
    widget->show();

  SetWinName();
}

void TopLevelViewer::DeIconify() {
  if (!isMapped() || !isTopLevel()) return;
  if (widget()->isMinimized()) {
    widget()->showNormal();
  }
  else {
    widget()->raise();
  }
}

bool TopLevelViewer::deleteOnWinClose() const {
//NOTE: this behavior can be changed, but is the current default:
  // if we are in a project, then persist, else transient
  taProject* proj = GET_MY_OWNER(taProject);
  return (!proj);
}

void TopLevelViewer::Iconify() {
  if (!isMapped() || !isTopLevel()) return;
  if (!widget()->isMinimized())
    widget()->showMinimized();
}


void TopLevelViewer::SetWinName() {
//   if (!isMapped()) return;
  MakeWinName_impl();
  if(widget())
    widget()->setWindowTitle(win_name);
//     widget()->setCaption(win_name);
}

void TopLevelViewer::ViewWindow() {
  if (!taMisc::gui_active) return;
  if (dvwidget()) {
    DeIconify();
  }
  else {
    // if not owned yet, put us in the global guy
    if (!GetOwner() && tabMisc::root)
      tabMisc::root->viewers_tmp.Add(this); // does InitLinks
    Constr(); // NO parent
//     Constr(QApplication::activeWindow()); // parent to current active
    Render();
  }
  Show();
}

void TopLevelViewer::WindowClosing(CancelOp& cancel_op) {
  ResolveChanges(cancel_op); // note, may have been done earlier
  if (cancel_op == CO_CANCEL) return;

  // root win is special, since closing it forces shutdown
  if (isRoot()) {
    switch (taMisc::quitting) {
    case taMisc::QF_RUNNING: {
      // to avoid bug we send a msg to main obj, then cancel here
      QEvent* ev = new QEvent((QEvent::Type)taiMiscCore::CE_QUIT);
      QCoreApplication::postEvent(taiM, ev);
      // taiMiscCore::Quit(CO_NOT_CANCELLABLE); // no going back now
      cancel_op = CO_CANCEL;
      return;
    }
    case taMisc::QF_USER_QUIT:
      // ok, upgrade to non-cancellable
      taMisc::quitting = taMisc::QF_FORCE_QUIT;
    default: break; // force-quit
    }
  }
}

