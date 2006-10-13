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

#include "ilabel.h"

#include <QApplication>
#include <qpalette.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qbitmap.h>
#include <qtextstream.h>
#include <qstyle.h>


iLabel::iLabel(QWidget* parent)
: QLabel(parent)
{
  init();
}

iLabel::iLabel(const QString& text, QWidget* parent)
:inherited(text, parent)
{
  init();
}

iLabel::iLabel(int index_, const QString& text, QWidget* parent)
:inherited(text, parent)
{
  init();
  mindex = index_;
}

void iLabel::init() {
  mhighlight = false;
  mindex = -1;
}

void iLabel::contextMenuEvent(QContextMenuEvent* e) {
  emit contextMenuInvoked(this, e);
};

void iLabel::setHighlight(bool value){
  if (mhighlight == value) return;
  mhighlight = value;
  QFont fnt(font());
  QPalette pal(palette());
//NOTES:
// foregroundRole() is role used to render text
  if (value) {
    fnt.setBold(true);
    pal.setColor(backgroundRole(), Qt::yellow);
    setAutoFillBackground(true);
    pal.setColor(foregroundRole(), Qt::darkBlue); 
      //QApplication::palette().color(QPalette::Highlight)); // typically darkBlue
  } else {
    fnt.setBold(false);
    pal.setColor(backgroundRole(),  
      QApplication::palette(this).color(backgroundRole()));
    setAutoFillBackground(false);
    pal.setColor(foregroundRole(),  
      QApplication::palette(this).color(foregroundRole()));
  }
  setFont(fnt);
  setPalette(pal);
  update();
}

// NOTE: this is the Trolltech routine from QCheckbox, with the Highlight color setting added in the middle

/*void iLabel::paintEvent(QPaintEvent* pe)
{
  QLabel::paintEvent(pe);
  if (mhighlight) {
/.TODO    QPainter p;
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
    p.drawRect(r); 
  } else {
    //nothing
  }
}*/

