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

#include "iTabBarBase.h"

#include <taiMisc>

#include <QKeyEvent>


iTabBarBase::iTabBarBase(QWidget* parent_) : inherited(parent_) {
}

void iTabBarBase::selectNextTab() {
  int dx = 1;
  for (int index = currentIndex() + dx; index >= 0 && index < count(); index += dx) {
    if (isTabEnabled(index)) {
      setCurrentIndex(index);
      break;
    }
  }
}

void iTabBarBase::selectPrevTab() {
  int dx = -1;
  for (int index = currentIndex() + dx; index >= 0 && index < count(); index += dx) {
    if (isTabEnabled(index)) {
      setCurrentIndex(index);
      break;
    }
  }
}

void iTabBarBase::keyPressEvent(QKeyEvent* key_event) {
  taiMisc::BoundAction action = taiMisc::GetActionFromKeyEvent(taiMisc::PROJECTWINDOW_CONTEXT, key_event);
  switch (action) {
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_LEFT: // switch tab
      selectPrevTab();
      key_event->accept();
      return;
    case taiMisc::PROJECTWINDOW_SHIFT_TAB_RIGHT: // switch tab
      selectNextTab();
      key_event->accept();
      return;
    default:
      inherited::keyPressEvent(key_event);
  }
}

void iTabBarBase::mousePressEvent(QMouseEvent* e) {
  setFocus();
  return inherited::mousePressEvent(e);
}

