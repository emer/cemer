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

#include "DockViewer.h"
#include <iDockViewer>

void DockViewer::Initialize() {
  dock_flags = DV_NONE;
  dock_area = Qt::BottomDockWidgetArea;
}

IDataViewWidget* DockViewer::ConstrWidget_impl(QWidget* gui_parent) {
//TODO: maybe we don't even need a generic one, but it does enable us to
// make a purely taBase guy that doesn't need its own special gui guy
  return new iDockViewer(this, gui_parent);
}

void DockViewer::GetWinState_impl() {
  inherited::GetWinState_impl();
  QWidget* wid = widget(); //cache
  bool vis = wid->isVisible();
  SetUserData("view_visible", vis);
}

void DockViewer::SetWinState_impl() {
  inherited::SetWinState_impl();
  QWidget* wid = widget(); //cache
  bool vis = GetUserDataDef("view_visible", true).toBool();
  wid->setVisible(vis);
}

