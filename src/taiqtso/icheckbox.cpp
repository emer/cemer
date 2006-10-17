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

#include "icheckbox.h"
#ifndef QT_NO_CHECKBOX
#include <qpalette.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qbitmap.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qstyle.h>

#include <QStyleOptionButton>

iCheckBox::iCheckBox(QWidget* parent)
: inherited(parent)
{
  init();
}

iCheckBox::iCheckBox(const char* text, QWidget* parent)
: inherited(QString(text), parent)
{
  init();
}

iCheckBox::iCheckBox(bool value, QWidget* parent)
: inherited(parent)
{
  init();
  if (value) setChecked(value); // off is the default
}

void iCheckBox::init() {
  mread_only = false;
}

void iCheckBox::setReadOnly(bool value) {
  if (mread_only == value) return;
  mread_only = value;
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
  setEnabled(!value); // temp
}


#endif
