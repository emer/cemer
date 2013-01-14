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

#ifndef VColorBar_h
#define VColorBar_h 1

// parent includes:
#include <ColorBar>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API VColorBar : public ColorBar { // #IGNORE Vertical ColorBlocks Bar
#ifndef __MAKETA__
typedef ColorBar inherited;
#endif
  Q_OBJECT
public:
  VColorBar(ColorScale* c, QWidget* parent = NULL);
protected:
  void 		paintEvent(QPaintEvent* ev); // override
};


#endif // VColorBar_h
