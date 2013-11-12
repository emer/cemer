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

#ifndef VEStatic_Group_h
#define VEStatic_Group_h 1

// parent includes:
#include <VEStatic>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(VEStatic_Group);

class TA_API VEStatic_Group : public taGroup<VEStatic> {
  // ##CAT_VirtEnv a group of virtual environment static elements
INHERITED(taGroup<VEStatic>)
public:
  virtual void  Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void  DestroyODE();   // #CAT_ODE destroy ODE objs for these items

  virtual void  UpdateCurToRels();
  // #CAT_ODE go through all the statics in this space that have rel_static and relative set, and compute current position and rotation from relative offsets compared to the rel_body current values
  virtual void  SaveCurAsPrv();
  // #IGNORE go through all the statis in this space and save current vals as prv_* -- needed for UpdateCurToRels

  virtual void  Translate(float dx, float dy, float dz);
  // #BUTTON #DYN1 #CAT_ODE move object given distance (can select multiple and operate on all at once)
  virtual void  Scale(float sx, float sy=0.0f, float sz=0.0f);
  // #BUTTON #DYN1 #CAT_ODE scale size of object -- if sy or sz is 0, then sx is used for that dimension (can select multiple and operate on all at once)
  virtual void  RotateAxis(float x_ax, float y_ax, float z_ax, float rot);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void  RotateEuler(float euler_x, float euler_y, float euler_z);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values
  virtual void  SnapPosToGrid(float grid_size=0.05f);
  // #BUTTON #DYN1 #CAT_ODE snap the position of static bodies to grid of given size
  virtual void  CopyColorFrom(VEStatic* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other object


  TA_BASEFUNS_NOCOPY(VEStatic_Group);
private:
  void  Initialize()            { SetBaseType(&TA_VEStatic); }
  void  Destroy()               { };
};

#endif // VEStatic_Group_h
