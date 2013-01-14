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

#ifndef ColorBar_h
#define ColorBar_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#include <ColorScale>

// declare all other types mentioned but not required to include:

class TA_API ColorBar : public QWidget { // #IGNORE Basic properties of a bar
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  ColorScaleRef		scale;
  virtual int 		blocks();

  virtual void		SetColorScale(ColorScale* c);
  ColorBar(ColorScale* c, QWidget* parent = NULL);
  ~ColorBar();
};



#endif // ColorBar_h
