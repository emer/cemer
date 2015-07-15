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

#include "ToolBar.h"
#include <iToolBar>
#include <iRect>

#include <taiMisc>

TA_BASEFUNS_CTORS_DEFN(ToolBar);


void ToolBar::Initialize() {
  lft = 0.0f;
  top = 0.0f;
  o = Horizontal;
  visible = false; // overrides base
}

void ToolBar::Copy_(const ToolBar& cp) {
  lft = cp.lft;
  top = cp.top;
  o = cp.o;
}

void ToolBar::Constr_impl(QWidget* gui_parent) {
  inherited::Constr_impl(gui_parent);
  widget()->setObjectName(name);
}

void ToolBar::GetWinState_impl() {
  inherited::GetWinState_impl();
  iRect r = widget()->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
  SetUserData("view_win_visible", widget()->isVisible());
  SigEmitUpdated();
}

void ToolBar::SetWinState_impl() {
  inherited::SetWinState_impl();
  //TODO: docked, etc.
  iToolBar* itb = widget(); //cache
  bool vis = GetUserDataDef("view_win_visible", true).toBool();
  itb->setVisible(vis);
  itb->setOrientation((Qt::Orientation)o);
  iRect r = itb->frameGeometry();
  r.x = (int)(lft * taiM->scrn_s.w);
  r.y = (int)(top * taiM->scrn_s.h);
  itb->move(r.topLeft());
  itb->resize(r.size());
}

void ToolBar::WidgetDeleting_impl() {
  inherited::WidgetDeleting_impl();
  visible = false;
}

