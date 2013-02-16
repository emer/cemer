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

#ifndef VEObject_Group_h
#define VEObject_Group_h 1

// parent includes:
#include <VEObject>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class VEBody; // 


taTypeDef_Of(VEObject_Group);

class TA_API VEObject_Group : public taGroup<VEObject> {
  // ##CAT_VirtEnv a group of virtual environment objects
INHERITED(taGroup<VEObject>)
public:
  virtual void  Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void Step_pre();
  // #EXPERT #CAT_ODE do computations prior to taking a step of the ode physics solver -- called automatically at the start of Step() to allow special objects (e.g., VEArm) to update prior to stepping
  virtual void  CurToODE();
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call
  virtual void  CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing
  virtual void  DestroyODE();
  // #CAT_ODE destroy ODE objs for these items

  virtual void  CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init or SetValsToODE
  virtual void  Translate(float dx, float dy, float dz, bool init);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos
  virtual void  Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void  RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void  RotateEuler(float euler_x, float euler_y, float euler_z, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot
  virtual void  SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of bodies to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void  CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  TA_BASEFUNS_NOCOPY(VEObject_Group);
private:
  void  Initialize()            { SetBaseType(&TA_VEObject); }
  void  Destroy()               { };
};

#endif // VEObject_Group_h
