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

#include "ilineedit.h"

#include <qapplication.h>


iLineEdit::iLineEdit(QWidget* parent)
: QLineEdit(parent)
{
  init();
}

iLineEdit::iLineEdit(const char* text, QWidget* parent)
: QLineEdit(QString(text), parent)
{
  init();
}

void iLineEdit::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  emit focusChanged(true);
}

void iLineEdit::focusOutEvent(QFocusEvent* ev) {
  inherited::focusOutEvent(ev);
  emit focusChanged(false);
}

void iLineEdit::init() {
  mhilight = false;
}

void iLineEdit::setHilight(bool value){
  if (mhilight == value) return;
  if (isReadOnly() && value) return; // can't set hilight when ro
  mhilight = value;
  //NOTE: following doesn't really work, and is just temporary until a proper solution is found
  QColor bg;
  if (value) {
    bg = COLOR_HILIGHT;
  } else {
    bg = (QApplication::palette().active().base()); // current text widget background
  }
  setBackgroundColor(bg);
  update();
}

void iLineEdit::setReadOnly(bool value) {
  if (isReadOnly() == value) return;
  QLineEdit::setReadOnly(value);
  if (value) {
    mhilight = false;
//    setFocusPolicy(ClickFocus);
    setPaletteBackgroundColor(QApplication::palette().active().button());
  } else {
//    setFocusPolicy(StrongFocus);
    setPaletteBackgroundColor(QApplication::palette().active().base());
  }
}

