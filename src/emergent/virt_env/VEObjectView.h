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

#ifndef VEObjectView_h
#define VEObjectView_h 1

// parent includes:
#include <T3DataViewPar>
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class VEWorldView; // 
eTypeDef_Of(VEWorldView);
class TypeDef; // 
class VEObject; // 

eTypeDef_Of(VEObjectView);

class E_API VEObjectView: public T3DataViewPar {
  // view of one object: a group of bodies
INHERITED(T3DataViewPar)
friend class VEWorldView;
public:
  VEObject*		Object() const { return (VEObject*)data();}
  virtual void		SetObject(VEObject* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  void		BuildAll() override;
  
  bool			isVisible() const;
  bool		ignoreSigEmit() const override { return !isVisible(); }

  void 	SetDefaultName() override {} // leave it blank
  TA_BASEFUNS_NOCOPY(VEObjectView);
protected:
  void	Initialize();
  void	Destroy();

  void		Render_pre() override;
  void		Render_impl() override;
};

#endif // VEObjectView_h
