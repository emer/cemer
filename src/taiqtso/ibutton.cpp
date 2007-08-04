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

#include "ibutton.h"

#include <QColor>
#include <QColorDialog>
#include <QPainter>

iMenuButton::iMenuButton(QWidget* parent)
:inherited(parent)
{
  init();
}

iMenuButton::iMenuButton(const QString& text_, QWidget* parent)
:inherited(parent)
{
  init();
  setText(text_);
}


void iMenuButton::init() {
  setPopupMode(InstantPopup); //note: Menu mode looks/behaves awful
  setArrowType(Qt::DownArrow);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

//////////////////////////////////
//  iColorButton		//
//////////////////////////////////

iColorButton::iColorButton(QWidget* parent)
:inherited(parent)
{
  init();
}

iColorButton::iColorButton(int r, int g, int b, int a, QWidget* parent)
:inherited(parent)
{
  init(r, g, b, a);
}


void iColorButton::init(int r, int g, int b, int a) {
  m_use_alpha = true; // default
  m_col.setRgb(r, g, b, a);
  setPopupMode(InstantPopup); //note: Menu mode looks/behaves awful
  connect(this, SIGNAL(clicked()), this, SLOT(this_clicked()));
}

void iColorButton::paintEvent(QPaintEvent* pe)
{
  inherited::paintEvent(pe);
  const int in = 4; // inset
  QPainter p(this);
  p.setBrush(m_col); // can set a brush with a color
  p.drawRect(in, in, width() - (2 * in), height() - (2 * in));
}

void iColorButton::setColor(const QColor& val) {
  m_col = val;
  update();
}

void iColorButton::this_clicked() {
  if (useAlpha()) {
    QRgb rgb = m_col.rgba();
    bool ok = false;
    rgb = QColorDialog::getRgba(rgb, &ok);
    if (!ok) return;
    m_col.setRgba(rgb);
  } else {
    QColor col(m_col);
    col = QColorDialog::getColor(col);
    if (!col.isValid()) return;
    // note: don't actually clobber alpha
    col.setAlpha(m_col.alpha()); // easiest way
    m_col = col;
  }
  update();
  emit colorChanged();
}

