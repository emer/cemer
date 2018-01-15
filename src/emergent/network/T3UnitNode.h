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

#ifndef T3UnitNode_h
#define T3UnitNode_h 1

// parent includes:
#include "network_def.h"
#include <T3NodeLeaf>

// member includes:

// declare all other types mentioned but not required to include:
class iColor; //
class NetView; //

#ifdef TA_QT3D

class E_API T3UnitNode: public T3NodeLeaf {
  Q_OBJECT
  INHERITED(T3NodeLeaf)
public:
  static float  base_height; // #HIDDEN #NO_SAVE #DEF_0.1 height when not active or empty
  static float  max_height; // #HIDDEN #NO_SAVE #DEF_0.8 height when fully active

  float         spacing;                      // unit spacing
  float         disp_scale;                   // overall scaling
  
  void          setDefaultCaptionTransform() override;
  //  sets text justif and
  void          setAppearance(NetView* nv, float act, const iColor& color, float max_z);
  // act is -1:1; max_z is net->max_size.z
  virtual void  setPicked(bool value);

  T3UnitNode(Qt3DNode* par = NULL, T3DataView* dataView_ = NULL,
             float max_x = 1.0f, float max_y = 1.0f,
             float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);
  ~T3UnitNode();

  virtual void  setAppearance_impl(NetView* nv, float act, const iColor& color, float max_z,
                                   bool act_invalid);
  // act is -1:1; max_z is net->max_size.z; trans is transparency; act_invalid true for nan/inf -- act has been set to 0.0f
};

#else // TA_QT3D

class E_API T3UnitNode: public T3NodeLeaf { //
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_ABSTRACT_HEADER(T3UnitNode);
#endif
friend class T3UnitGroupNode;
public:
  static float  base_height; // #HIDDEN #NO_SAVE #DEF_0.1 height when not active or empty
  static float  max_height; // #HIDDEN #NO_SAVE #DEF_0.8 height when fully active

  static void   initClass();

  void          setAppearance(NetView* nv, float act, const iColor& color, float max_z);
  // act is -1:1; max_z is net->max_size.z
  virtual void  setPicked(bool value);
  T3UnitNode(T3DataView* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
             float max_z = 1.0f, float un_spc = .01f, float disp_sc = 1.0f);

protected:
  float                 spacing;                      // unit spacing
  float                 disp_scale;                   // overall scaling
  void                  setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  virtual void  setAppearance_impl(NetView* nv, float act, const iColor& color,
                                   float max_z, bool act_invalid);
  // act is -1:1; max_z is net->max_size.z; trans is transparency; act_invalid true for nan/inf -- act has been set to 0.0f
  ~T3UnitNode();
};

#endif // TA_QT3D

#endif // T3UnitNode_h
