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

#ifndef iColorMatrixGrid_h
#define iColorMatrixGrid_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:

// declare all other types mentioned but not required to include:

class TA_API iColorMatrixGrid: QWidget {
  // a grid for visually depicting and/or editing grid data (usually 2d)
INHERITED(QWidget)
  Q_OBJECT
public:
  int		cellSize() const {return m_cellSize;} // h/w of each grid square; 0=auto
  void		setCellSize(int value);

  iColorMatrixGrid(QWidget* parent = NULL);

protected:
  int		m_cellSize;
  
  void 		paintEvent(QPaintEvent* event); // override
  
private:
  void		Init();
};

#endif // iColorMatrixGrid_h
