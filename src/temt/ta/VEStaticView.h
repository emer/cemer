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

#ifndef VEStaticView_h
#define VEStaticView_h 1

// parent includes:
#include <T3DataView>

// member includes:

// declare all other types mentioned but not required to include:
class VEWorldView; // 
class TypeDef; // 
class VEStatic; // 


class TA_API VEStaticView: public T3DataView {
  // view of one static environment element
INHERITED(T3DataView)
friend class VEWorldView;
public:
  VEStatic*		Static() const { return (VEStatic*)data();}
  virtual void		SetStatic(VEStatic* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  virtual void		SetDraggerPos();
  // set dragger position, based on shape

  bool			isVisible() const;
  override bool		ignoreDataChanged() const { return !isVisible(); }

  void 	SetDefaultName() {} // leave it blank
  TA_BASEFUNS_NOCOPY(VEStaticView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

#endif // VEStaticView_h
