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

#include "iToolBar.h"
#include <ToolBar>
#include <iApplicationToolBar>
#include <iAction>
#include <iMainWindowViewer>
#include <taiWidgetMenu>

#include <taMisc>



IDataViewWidget* ToolBar::ConstrWidget_impl(QWidget* gui_parent) {
  if (name == "Application")
    return new iApplicationToolBar(this, gui_parent); // usually parented later
  else
    return new iToolBar(this, gui_parent); // usually parented later
}

iToolBar::iToolBar(ToolBar* viewer_, QWidget* parent)
  : inherited(parent), IDataViewWidget(viewer_)
{
  Init();
}

iToolBar::~iToolBar()
{
}

void iToolBar::Init() {
  setWindowTitle(viewer()->GetName());
}

void iToolBar::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iToolBar::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iToolBar::Showing(bool showing) {
  iMainWindowViewer* dv = viewerWindow();
  if (!dv) return;
  iAction* me = dv->toolBarMenu->FindActionByData((void*)this);
  if (!me) return;
  if(showing && taMisc::viewer_options & taMisc::VO_NO_TOOLBAR) {
    me->setChecked(false); //note: triggers event
    hide();
    return;
  }
  if (showing == me->isChecked()) return;
  me->setChecked(showing); //note: triggers event
}

