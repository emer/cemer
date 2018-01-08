// Copyright 2013-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef T3UnitNode_Block_h
#define T3UnitNode_Block_h 1

// parent includes:
#include <T3UnitNode>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef TA_QT3D

class E_API T3UnitNode_Block: public T3UnitNode {
  Q_OBJECT
  INHERITED(T3UnitNode)
public:
  T3UnitNode_Block(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL,
                   float max_x = 1.0f, float max_y = 1.0f,
		   float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);
  ~T3UnitNode_Block();

  void	 setAppearance_impl(NetView* nv, float act, const iColor& color,
                            float max_z, bool act_invalid) override;
// private:
//   SoCube*		shape_; //#IGNORE
};

#else // TA_QT3D

class SoCube; //

class E_API T3UnitNode_Block: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  TA_SO_NODE_HEADER(T3UnitNode_Block);
#endif
friend class T3UnitGroupNode;
public:
  static void	initClass();

  T3UnitNode_Block(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		   float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
  void	 setAppearance_impl(NetView* nv, float act, const iColor& color,
                            float max_z, bool act_invalid) override;
  ~T3UnitNode_Block();
private:
  SoCube*		shape_; //#IGNORE
};

#endif // TA_QT3D

#endif // T3UnitNode_Block_h
