// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#include "ispinbox.h"

#include "qlineedit.h"

#include <qapplication.h>
#include <qevent.h>
#include <qrangecontrol.h>

iSpinBox::iSpinBox(QWidget* parent)
: QSpinBox(parent)
{
  init();
}

void iSpinBox::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  emit focusChanged(true);
}

void iSpinBox::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
  emit focusChanged(false);
}

void iSpinBox::init() {
  mhilight = false;
  mreadOnly = false;
  updating = 0;
  connect(editor(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()) );

}

bool iSpinBox::hasSelectedText() {
  return editor()->hasSelectedText();
}

void iSpinBox::cut() {
  editor()->cut();
}

void iSpinBox::copy() {
  editor()->copy();
}

void iSpinBox::paste() {
  editor()->paste();
}

void iSpinBox::del() {
  editor()->del();
}

void iSpinBox::setHilight(bool value){
  if (mhilight == value) return;
  if (mreadOnly && value) return;  //hilighting ignored if read-only
  mhilight = value;
  if (value) {
    setPaletteBackgroundColor(COLOR_HILIGHT);
  } else {
    setPaletteBackgroundColor(QApplication::palette().active().base());
  }
  update();
}

void iSpinBox::setReadOnly(bool value) {
  if (mreadOnly == value) return;
  editor()->setReadOnly(value);
  if (value) {
    mhilight = false;
    setFocusPolicy(ClickFocus);
    setPaletteBackgroundColor(QApplication::palette().active().button());
  } else {
    setFocusPolicy(StrongFocus);
    setPaletteBackgroundColor(QApplication::palette().active().base());
  }
  // note: we have to dynamically disable the up/down controls in the paint event, because of how qspinbox is implemented
  mreadOnly = value;
}

void iSpinBox::setPaletteBackgroundColor(const QColor& color) {
   // pushes through to lineEdit
  QSpinBox::setPaletteBackgroundColor(color);
  editor()->setBackgroundColor(color);
}

void iSpinBox::stepUp() {
   if (mreadOnly) return;
   QSpinBox::stepUp();
}

void iSpinBox::stepDown(){
   if (mreadOnly) return;
   QSpinBox::stepDown();
}

void iSpinBox::updateDisplay() {
  // this is a huge implementation-dependent hack to disable the up/down for readOnly
  // if this stops working, it is not critical (controls will be enabled, but won't do anything)
  if (mreadOnly) {
    if (updating > 0) return; // needed because our actions cause reentrance
    ++updating;
    // following tricks updateDisplay() into disabling both up/down levers
    int old_max = maxValue();
    int old_min = minValue();
    bool old_wrap = wrapping();
    setMaxValue(value());
    setMinValue(value());
    setWrapping(false);
    QSpinBox::updateDisplay();
    // restore saved values
    setRange(old_min, old_max);
    setWrapping(old_wrap);
    --updating;
  } else {
    QSpinBox::updateDisplay();
  }
}
