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

#ifndef T3ColorBar_h
#define T3ColorBar_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:
#include <ColorScale>

// declare all other types mentioned but not required to include:



TypeDef_Of(T3ColorBar);

class TA_API T3ColorBar : public T3NodeLeaf {
  // 3D color bar -- the bar is oriented horizontally be default (use a transform if needed) the 0,0 is at the centre of the rectangle
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3ColorBar);
#endif // def __MAKETA__
public:
  static void		initClass();

  float			height;
  float			width;

  ColorScaleRef		scale;

  virtual int 		blocks();
  override void		clear();

  virtual void		SetColorScale(ColorScale* c);
  void			SetDimensions(float wd, float ht);
  T3ColorBar(ColorScale* c = NULL, T3DataView* dataView_ = NULL);

protected:
  SoTriangleStripSet*	bars_; // one rectangle per subbar

  void			render();
  ~T3ColorBar();
};


#endif // T3ColorBar_h
