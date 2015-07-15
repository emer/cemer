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

#ifndef iVColorBar_h
#define iVColorBar_h 1

// parent includes:
#include <iColorBar>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API iVColorBar : public iColorBar { // #IGNORE Vertical ColorBlocks Bar
#ifndef __MAKETA__
typedef iColorBar inherited;
#endif
  Q_OBJECT
public:
  iVColorBar(ColorScale* c, QWidget* parent = NULL);
protected:
  void 		paintEvent(QPaintEvent* ev) override;
};


#endif // iVColorBar_h
