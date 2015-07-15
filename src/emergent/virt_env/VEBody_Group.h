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

#ifndef VEBody_Group_h
#define VEBody_Group_h 1

// parent includes:
#include <VEBody>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(VEBody_Group);

class TA_API VEBody_Group : public taGroup<VEBody> {
  // ##CAT_VirtEnv a group of virtual environment bodies
INHERITED(taGroup<VEBody>)
public:
  virtual void  Init();
  // #CAT_ODE #BUTTON initialize all ODE params and set to init_ settings

  virtual void Step_pre();
  // #EXPERT #CAT_ODE do computations prior to taking a step of the ode physics solver -- called automatically at the start of Step() to allow special objects (e.g., VEArm) to update prior to stepping
  virtual void  CurToODE();
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call

  virtual void  CurFromODE(bool updt_disp = false);     // get the updated values from ODE after computing
  virtual void  DestroyODE();   // #CAT_ODE destroy ODE objs for these items

  virtual void  CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init
  virtual void  UpdateInitToRels();
  // #CAT_ODE go through all the bodies in this object that have rel_body and init_rel set, and compute initial position and rotation from relative offsets compared to the rel_body init values
  virtual void  UpdateCurToRels();
  // #CAT_ODE go through all the bodies in this object that have rel_body and init_rel set, and compute current position and rotation from relative offsets compared to the rel_body current values
  virtual void  SaveCurAsPrv();
  // #IGNORE go through all the bodies in this object and save current vals as prv_* -- needed for UpdateCurToRels
 
 virtual void  Translate(float dx, float dy, float dz, bool init);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos
  virtual void  Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void  RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  virtual void  RotateEuler(float euler_x, float euler_y, float euler_z, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot (in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  virtual void  SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of bodies to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void  CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  TA_BASEFUNS_NOCOPY(VEBody_Group);
private:
  void  Initialize()            { SetBaseType(&TA_VEBody); }
  void  Destroy()               { };
};

#endif // VEBody_Group_h
