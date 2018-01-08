// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef VEBodyView_h
#define VEBodyView_h 1

// parent includes:
#include <T3DataView>
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class VEWorldView; // 
eTypeDef_Of(VEWorldView);
class TypeDef; // 
class VEBody; // 


eTypeDef_Of(VEBodyView);

class E_API VEBodyView: public T3DataView {
  // view of one body
INHERITED(T3DataView)
friend class VEWorldView;
public:
  VEBody*		Body() const { return (VEBody*)data();}
  virtual void		SetBody(VEBody* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  virtual void		SetDraggerPos();
  // set dragger position, based on shape

  bool			isVisible() const;
  bool		ignoreSigEmit() const override { return !isVisible(); }

  void 	SetDefaultName() override {} // leave it blank
  TA_BASEFUNS_NOCOPY(VEBodyView);

protected:
  void	Initialize();
  void	Destroy();

  void		Render_pre() override;
  void		Render_impl() override;
};

#endif // VEBodyView_h
