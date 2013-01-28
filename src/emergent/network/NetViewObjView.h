// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef NetViewObjView_h
#define NetViewObjView_h 1

// parent includes:
#include "network_def.h"
#include <T3DataView>

// member includes:

// declare all other types mentioned but not required to include:
class NetViewObj; //
class NetView; //
TypeDef_Of(NetView);

TypeDef_Of(NetViewObjView);

class EMERGENT_API NetViewObjView: public T3DataView {
  // view of net view object
INHERITED(T3DataView)
public:
  NetViewObj*           Obj() const { return (NetViewObj*)data();}
  virtual void          SetObj(NetViewObj* ob);

  DATAVIEW_PARENT(NetView)

  void  SetDefaultName() {} // leave it blank
  TA_BASEFUNS_NOCOPY(NetViewObjView);
protected:
  void  Initialize();
  void  Destroy();

  override void         Render_pre();
  override void         Render_impl();
};

#endif // NetViewObjView_h
