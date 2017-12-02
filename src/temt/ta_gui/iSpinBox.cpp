// Copyright 2017, Regents of the University of Colorado,
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

#include "iSpinBox.h"

#include <iLineEdit>
#include <qevent.h>

#include <limits.h>

#include <taMisc>


using namespace Qt;

iSpinBox::iSpinBox(QWidget* parent)
: QSpinBox(parent)
{
  init();
}

void iSpinBox::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  // emit focusChanged(true);
}

void iSpinBox::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
  // emit focusChanged(false);
}

void iSpinBox::wheelEvent(QWheelEvent * event) {
  event->ignore();		// don't process this event!
  return;
}

void iSpinBox::init() {
  updating = 0;
  setMaximum(INT_MAX); // 99 is a whacked-out maximum
  setLineEdit(new iLineEdit); // takes ownership
  connect(lineEdit(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()) );
  setCharWidth(taMisc::edit_width_int);
}

bool iSpinBox::hasSelectedText() {
  return lineEdit()->hasSelectedText();
}

void iSpinBox::cut() {
  lineEdit()->cut();
}

void iSpinBox::copy() {
  lineEdit()->copy();
}

void iSpinBox::paste() {
  lineEdit()->paste();
}

void iSpinBox::del() {
  lineEdit()->del();
}

void iSpinBox::setReadOnly(bool value) {
  if (isReadOnly() == value) return;
  
  // we need to set ro first, so our strongly typed guy sees it
  iLineEdit* le = qobject_cast<iLineEdit*>(lineEdit()); // should succeed
  if (le) {
    le->setReadOnly(value);
  }
  inherited::setReadOnly(value);
}

void iSpinBox::setMinCharWidth(int num) {
  ((iLineEdit*)lineEdit())->setMinCharWidth(num);
  int wid = lineEdit()->minimumWidth();
  setMinimumWidth(wid + 20);     // spin box..
  wid = lineEdit()->maximumWidth();
  setMaximumWidth(wid + 20);     // spin box..
}

void iSpinBox::setCharWidth(int num) {
  ((iLineEdit*)lineEdit())->setCharWidth(num);
  int wid = lineEdit()->minimumWidth();
  setMinimumWidth(wid + 20);     // spin box..
  wid = lineEdit()->maximumWidth();
  setMaximumWidth(wid + 20);     // spin box..
}
