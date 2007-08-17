// Copyright, 1995-2005, Regents of the University of Colorado,
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


#include "icombobox.h"

#include <qcheckbox.h>

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


iComboBox::iComboBox(QWidget* parent)
: QComboBox(parent)
{
  defPalette = palette();
  init();
}

void iComboBox::init() {
  mhilight = false;
}

void iComboBox::setHilight(bool value){
  if (mhilight == value) return;
  mhilight = value;
  if (value) {
    QPalette pal = QPalette(defPalette);
    pal.setColor(QPalette::Active, QPalette::Button, COLOR_HILIGHT);
    pal.setColor(QPalette::Inactive, QPalette::Button, COLOR_HILIGHT);

    ((iComboBox*)this)->setPalette(pal);
  } else {
    ((iComboBox*)this)->setPalette(defPalette);
  }
  update();
}

/*void iComboBox::paintEvent(QPaintEvent* pe) { // override
  if (mhilight) {
    QPalette pal = QPalette(defPalette);
    pal.setColor(QPalette::Active, QColorGroup::Button, COLOR_HILIGHT);
    pal.setColor(QPalette::Inactive, QColorGroup::Button, COLOR_HILIGHT);

    ((iComboBox*)this)->setPalette(pal);
    QComboBox::paintEvent(pe);
    ((iComboBox*)this)->setPalette(defPalette);
  } else {
    QComboBox::paintEvent(pe);
  }
} */


