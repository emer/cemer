// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "iStripeWidget.h"

#include <QPainter>

iStripeWidget::iStripeWidget(QWidget* parent)
:QWidget(parent), mhiLightColor(0x80, 0x80, 0x80)
{
  mstripeHeight = 25;
  mtopMargin = 0;
  mbottomMargin = 0;
}

iStripeWidget::~iStripeWidget()
{
}

void iStripeWidget::clearLater() {
  while (children().count() > 0) {
    QObject* obj = children().last();
    delete obj;
  }
}

QSize iStripeWidget::minimumSizeHint() const {
  // get superclass's hint, and override the height
  QSize rval = QWidget::minimumSizeHint();
  return rval.expandedTo(QSize(rval.width(), 
    mtopMargin + (2 * mstripeHeight) + mbottomMargin));
}

void iStripeWidget::setColors(const QColor& hilight, const QColor& bg) {
  mhiLightColor = hilight; // cached for rebuild
  QPalette palette;
  palette.setColor(this->backgroundRole(), bg);
  this->setPalette(palette);
  update();
}

void iStripeWidget::setHiLightColor(const QColor& val) {
  mhiLightColor = val;
  update();
}

void iStripeWidget::setStripeHeight(int val) {
  if (val == mstripeHeight) return;
  if (val < 1) return; // must be +ve
  mstripeHeight = val;
  update();
}

void iStripeWidget::setBottomMargin(int val) {
  if (val == mbottomMargin) return;
  if (val < 0) return; // must be +ve
  mbottomMargin = val;
  update();
}

void iStripeWidget::setTopMargin(int val) {
  if (val == mtopMargin) return;
  if (val < 0) return; // must be +ve
  mtopMargin = val;
  update();
}

void iStripeWidget::paintEvent(QPaintEvent* pev)
{
  QWidget::paintEvent(pev);
  if (height() <= mtopMargin) return;
  QPainter p(this);
  int num_stripes = (height() - mtopMargin) / mstripeHeight;
  if ((mstripeHeight * num_stripes) > (height() - mtopMargin)) num_stripes++; // one fraction of a stripe -- should be clipped by painting

  p.setPen(mhiLightColor);
  p.setBrush(mhiLightColor);

  // hilight every second stripe
  for (int i = 1; i < num_stripes; i+= 2 ) {
    p.drawRect(0, (i * mstripeHeight) + mtopMargin, width(), mstripeHeight); // draw hilighted rect
  }
}

