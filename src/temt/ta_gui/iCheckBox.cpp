// Co2018ght 2013-22018 Regents of the University of Colorado,
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

#include "iCheckBox.h"

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
  if (mread_only == value)
    return;
  mread_only = value;
  setStyleSheet("color: black"); // prevent text from going grey when ro
  setEnabled(!value); // this does the right thing..
}

#endif
