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

#include "VColorBar.h"
#include <QPainter>


VColorBar::VColorBar(ColorScale* c, QWidget* parent)
:inherited(c, parent)
{}


void VColorBar::paintEvent(QPaintEvent* ev) {
  if (!scale) return; // some kind of zombie happening...
  QRect r = rect();
  int h_tot = r.height();
  if (h_tot == 0) return;  // prob should never happen

  int b = blocks(); // cache
  int x = r.x(); // always exact
  int w = r.width(); // always exact
  int y = r.y(); // s/b 0
  QPainter paint(this);
  if (h_tot <= b ) { // less or same # strips as blocks -- subsample
    for (int j = h_tot - 1; j >= 0; --j) { // one strip per pixel
      int i = (j * (b - 1)) / h_tot ;
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
      paint.fillRect(x, y, w, 1, (QColor)col); // note: QColor converted to QBrush by Qt
      y += 1;
    }
  } else { // more space than blocks -- oversample
    float h = ((float)h_tot) / b; // ideal exact value per strip
    for (int i = b - 1; i >= 0; --i) { // one strip per color value
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
//      int hi = (int)(((((b - 1) - i) + 1) * h) - (float)y);
      int hi = (int)(((b - i) * h) - (float)y);

      paint.fillRect(x, y, w, hi, (QColor)col); // note: QColor converted to QBrush by Qt
      y += hi;
    }
  }
}

