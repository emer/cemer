// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "iLabel.h"

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

#include <IWidgetHost>
#include <iColor>

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
  mlighten = false;
  mindex = -1;
  host = NULL;
}

void iLabel::contextMenuEvent(QContextMenuEvent* e) {
  emit contextMenuInvoked(this, e);
};

void iLabel::setHighlight(bool value){
  if (mhighlight == value) return;
  mhighlight = value;
  updateBgColor();
}

void iLabel::setLighten(bool value){
  if (mlighten == value) return;
  mlighten = value;
  updateBgColor();
}

void iLabel::updateBgColor() {
  QFont fnt(font());
  QPalette pal(palette());
  if (mhighlight) {
    fnt.setBold(true);
    if(mlighten) {
      pal.setColor(backgroundRole(), QColor(Qt::yellow).darker(120)); // yeah actually darker
    }
    else {
      pal.setColor(backgroundRole(), Qt::yellow);
    }
    setAutoFillBackground(true);
    // pal.setColor(foregroundRole(), Qt::darkBlue); 
  }
  else {
    fnt.setBold(false);
    QColor bgclr;
    if(host) {
      bgclr = host->backgroundColor();
    }
    else {
      bgclr = Qt::cyan;
    }
    if(mlighten) {
      pal.setColor(backgroundRole(), bgclr.lighter(120));
      setAutoFillBackground(true);
    }
    else {
      pal.setColor(backgroundRole(), bgclr);
      setAutoFillBackground(false);
    }
    // pal.setColor(foregroundRole(),  
    //              QApplication::palette(this).color(foregroundRole()));
  }
  setFont(fnt);
  setPalette(pal);
  update();
}

void iLabel::setUserData(const QVariant& value) {
  muser_data = value;
}

