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

#include "taGuiEditor.h"
#include <taMisc>
#include <taGuiDialog>

#include <QVBoxLayout>

taBase* taGuiEditor::Base() const {
  return gui_owner;
}

TypeItem::ShowMembs taGuiEditor::show() const {
  return taMisc::show_gui;
}

taGuiEditor::taGuiEditor(taGuiDialog* own, bool read_only_, bool modal_, QObject* parent)
  : taiEditor(&TA_taGuiDialog, read_only_, modal_, parent) {
  gui_owner = own;
}

taGuiEditor::~taGuiEditor() {
}

void taGuiEditor::Constr_Body() {
  if(gui_owner->widgets.size > 0) {
    taGuiWidget* fw = gui_owner->widgets.FastEl(0);
    fw->widget->setParent(mwidget);
    vblDialog->addWidget(fw->widget);
  }
}

void taGuiEditor::GetImage(bool force) {
  ++updating;                   // prevents spurious changed flags from coming in
  gui_owner->GetImage();
  --updating;
}

void taGuiEditor::GetValue() {
  gui_owner->GetValue();
  gui_owner->GetImage();
}

void taGuiEditor::Ok_impl() {
  GetValue();
}
