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
: QCheckBox(parent)
{
  init();
}

iCheckBox::iCheckBox(const char* text, QWidget* parent)
: QCheckBox(QString(text), parent)
{
  init();
}

void iCheckBox::init() {
  mhilight = false;
  mreadOnly = false;
}

bool iCheckBox::isReadOnly() {
  return mreadOnly;
}

void iCheckBox::setHilight(bool value){
  if (mhilight == value) return;
  if (isReadOnly() && value) return; // can't set hilight when ro
  mhilight = value;
  update();
}

void iCheckBox::setReadOnly(bool value) {
  if (mreadOnly == value) return;
  mreadOnly = value;
  if (value) {
    mhilight = false;
  } else {
    //nothing
  }
  setEnabled(!value); // temp
}

// NOTE: this is the Trolltech routine from QCheckbox, with the Hilight color setting added in the middle

void iCheckBox::paintEvent(QPaintEvent* pe)
{
  QCheckBox::paintEvent(pe);
  if (mhilight) {
/*TODO    QPainter p;
    QRect irect = style()->subElementRect(QStyle::SE_CheckBoxContents, QStyleOptionButton, this);
    p.setBrush(COLOR_BRIGHT_HILIGHT);
    p.setPen(COLOR_BRIGHT_HILIGHT);
//TODO: Qt4     p->setRasterOp(Qt::CopyROP);
    QRect r;
    r.setRect(irect.x(), irect.y() - 2, irect.width() + 2, 2);
    p.drawRect(r);
    r.setRect(irect.x() + irect.width() + 1, irect.y(), 2, irect.height() + 2);
    p.drawRect(r);
    r.setRect(irect.x() - 2, irect.y() + irect.height(), irect.width() + 2, 2);
    p.drawRect(r);
    r.setRect(irect.x() - 2, irect.y() - 2, 2, irect.height() + 2);
    p.drawRect(r); */
  } else {
    //nothing
  }
}

#endif
