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

#include "ColorMatrixGrid.h"
#include <QPainter>


ColorMatrixGrid::ColorMatrixGrid(QWidget* parent)
:inherited(parent)
{
  Init();
}

void ColorMatrixGrid::Init() {
  m_cellSize = 3;
}

void ColorMatrixGrid::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
//TEMP
  painter.setPen(Qt::blue);
  painter.setFont(QFont("Arial", 10));
  painter.drawText(rect(), Qt::AlignCenter, "(Colorgrid TBD)");
//</TEMP>
}

void ColorMatrixGrid::setCellSize(int value) {
  if (value < 0) value = 0;
  if (m_cellSize == value) return;
  m_cellSize = value;
  update();
}

