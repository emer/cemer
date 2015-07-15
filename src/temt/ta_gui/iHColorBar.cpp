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

#include "iHColorBar.h"
#include <QPainter>


iHColorBar::iHColorBar(ColorScale* c, QWidget* parent)
:inherited(c, parent)
{};

void iHColorBar::paintEvent(QPaintEvent* ev) {
  if (!scale) return; // some kind of zombie happening...
  QRect r = rect();
  int w_tot = r.width();
  if (w_tot == 0) return;  // prob should never happen

  int b = blocks(); // cache
  int x = r.x(); //  s/b 0
  int h = r.height(); // always exact
  int y = r.y(); // always exact
  QPainter paint(this);
  if (w_tot <= b ) { // less or same # strips as blocks -- subsample
    for (int j = 0; j < w_tot ; ++j) { // one strip per pixel
      int i = (j * (b - 1)) / w_tot ;
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
      paint.fillRect(x, y, 1, h, (QColor)col); // note: QColor converted to QBrush by Qt
      x += 1;
    }
  } else { // more space than blocks -- oversample
    float w = ((float)w_tot) / b; // ideal exact value per strip
    for (int i = 0; i < b; ++i) { // one strip per color value
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
      int wi = (int)(((i + 1) * w) - (float)x);

      paint.fillRect(x, y, wi, h, (QColor)col); // note: QColor converted to QBrush by Qt
      x += wi;
    }
  }
}


