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

#include <QApplication>
#include <QPalette>

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
  mmin_char_width = 0;
}

void iLineEdit::setMinCharWidth(int num) {
  if (num > 128) num = 128;
  else if (num < 0) num = 0;
  if (mmin_char_width == num) return;
  mmin_char_width = num;
  if (mmin_char_width == 0) {
    setMinimumWidth(0);
  } else {
    QFontMetrics fm(font());
    QString s; s.reserve(num);
    // just use numbers, which are probably of about average width
    for (int i = 0; i < num; ++i) {
      s.append(QChar('0' + (i % 10)));
    }
    setMinimumWidth(fm.size(Qt::TextSingleLine, s).width());
  }
}

void iLineEdit::setReadOnly(bool value) {
  if (isReadOnly() == value) return;
  inherited::setReadOnly(value);
  QPalette pal(palette());
  if (value) {
//    setFocusPolicy(ClickFocus);
    pal.setColor(backgroundRole(), COLOR_RO_BACKGROUND);
  } else {
//    setFocusPolicy(StrongFocus);
    pal.setColor(backgroundRole(), 
      QApplication::palette(this).color(QPalette::Base));
  }
  setPalette(pal);
  update();
}

