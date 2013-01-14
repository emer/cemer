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

#ifndef ColorPad_h
#define ColorPad_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#include <iColor>

// declare all other types mentioned but not required to include:
class ColorScaleBar;

class TA_API ColorPad : public QWidget { // #IGNORE color pads for palletes
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  enum BlockFill {		// fill types for grid blocks
    COLOR,			// color indicates value
    AREA,			// area indicates value
    LINEAR 			// linear size of square side indicates value
  };

  ColorScaleBar* sb;
  float 	padval;
  float 	oldpadval;
  BlockFill 	fill_type;
  iColor fg;			// #IGNORE foreground color;
  iColor bg;			// #IGNORE background color;
  iColor tc;			// #IGNORE text color;

  virtual void		SetFillType(BlockFill b);
  virtual void		Set(float val);
  virtual void		Reset();
  virtual void		ReFill();
  virtual float		GetVal();
  virtual void		GetColors();
  virtual void		Toggle();
  ColorPad(ColorScaleBar* tsb, BlockFill s = COLOR, QWidget* parent = NULL);
  ~ColorPad();
};


#endif // ColorPad_h
