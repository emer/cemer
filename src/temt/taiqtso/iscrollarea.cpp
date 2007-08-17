// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "iscrollarea.h"

#include <QScrollBar>

iScrollArea::iScrollArea(QWidget* parent)
:inherited(parent)
{
  init();
}

void iScrollArea::init() {
}

QSize iScrollArea::sizeHint() const {
  QSize s(inherited::sizeHint());
  // if scrollbars are enabled but not shown, add their size anyway to avoid
  // the annoying slightly scrolled issue
  if (horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded) {
    QScrollBar* sb = horizontalScrollBar();
    if (!sb->isVisible())
      s.rheight() += sb->sizeHint().height() + 2;
  }
  if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) {
    QScrollBar* sb = verticalScrollBar();
    if (!sb->isVisible())
      s.rwidth() += sb->sizeHint().width() + 2;
  }
  return s;
}
