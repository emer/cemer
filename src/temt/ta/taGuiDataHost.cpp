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

#include "taGuiDataHost.h"
#include <taMisc>
#include <taGuiDialog>

taBase* taGuiDataHost::Base() const {
  return gui_owner;
}

TypeItem::ShowMembs taGuiDataHost::show() const {
  return taMisc::show_gui;
}

taGuiDataHost::taGuiDataHost(taGuiDialog* own, bool read_only_, bool modal_, QObject* parent)
  : taiDataHostBase(&TA_taGuiDialog, read_only_, modal_, parent) {
  gui_owner = own;
}

taGuiDataHost::~taGuiDataHost() {
}

void taGuiDataHost::Constr_Body() {
  if(gui_owner->widgets.size > 0) {
    taGuiWidget* fw = gui_owner->widgets.FastEl(0);
    fw->widget->setParent(mwidget);
    vblDialog->addWidget(fw->widget);
  }
}

void taGuiDataHost::GetImage(bool force) {
  ++updating;                   // prevents spurious changed flags from coming in
  gui_owner->GetImage();
  --updating;
}

void taGuiDataHost::GetValue() {
  gui_owner->GetValue();
  gui_owner->GetImage();
}

void taGuiDataHost::Ok_impl() {
  GetValue();
}
