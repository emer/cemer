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

#ifndef T3GraphBar_h
#define T3GraphBar_h 1

// parent includes:
#include <T3NodeLeaf>

// member includes:
#include <iVec3f>

// declare all other types mentioned but not required to include:
#ifdef TA_QT3D

#include <T3Cube>
class iColor;

class TA_API T3GraphBar : public T3NodeLeaf {
  // a graph bar for bar graphs -- data is GraphColView
  Q_OBJECT
  INHERITED(T3NodeLeaf)
public:
  bool                  z_on;   // is z axis on or not?
  T3Cube*       cube;
    
  virtual void          SetBar(iVec3f& pos, iVec3f& size, const iColor& color);

  T3GraphBar(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL, bool z_on = false);
  ~T3GraphBar();
};

#else // TA_QT3D

class T3Color;

taTypeDef_Of(T3GraphBar);

class TA_API T3GraphBar : public T3NodeLeaf {
  // a graph bar for bar graphs -- data is GraphColView
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  TA_SO_NODE_HEADER(T3GraphBar);
#endif // def __MAKETA__
public:

  static void		initClass();

  virtual void          SetBar(iVec3f& pos, iVec3f& size, const T3Color& color);

  T3GraphBar(T3DataView* dataView_ = NULL);

protected:
  ~T3GraphBar();
};

#endif // TA_QT3D

#endif // T3GraphBar_h
