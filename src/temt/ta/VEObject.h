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

#ifndef VEObject_h
#define VEObject_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <VEBody_Group>
#include <VEJoint_Group>
#include <MinMaxInt>

// declare all other types mentioned but not required to include:
class VEWorld; // 
class VEBody; // 


taTypeDef_Of(VEObject);

class TA_API VEObject : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_veobj a virtual environment object, which contains interconnected bodies and their joints, and represents a sub-space of objects
INHERITED(taNBase)
public:
  enum SpaceType {
    SIMPLE_SPACE,               // simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,                 // hash-coded space with multi-scale grids: efficient for larger number of items
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*         space_id;       // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  SpaceType     space_type;     // type of space to use
  MinMaxInt     hash_levels;    // #CONDSHOW_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space
  bool          auto_updt_rels; // #DEF_true automatically update relative body positions of other linked bodies whenever a body in this object is translated or rotated with Translate or Rotate functions
  VEBody_Group  bodies;
  VEJoint_Group joints;


  override String       GetDesc() const { return desc; }

  virtual VEWorld* GetWorld();  // #CAT_ODE get world object (parent of this guy)
  virtual void* GetWorldID();   // #CAT_ODE get world id value
  virtual void* GetWorldSpaceID(); // #CAT_ODE get world space id value

  virtual bool  CreateODE();    // #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void  DestroyODE();   // #CAT_ODE destroy object in ode (if created)

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

  virtual void  CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init
  virtual void  UpdateInitToRels();
  // #CAT_ODE go through all the bodies in this object that have rel_body and init_rel set, and compute initial position and rotation from relative offsets compared to the rel_body init values -- called automatically if auto_updt_rels is set
  virtual void  UpdateCurToRels();
  // #CAT_ODE go through all the bodies in this object that have rel_body and init_rel set, and compute current position and rotation from relative offsets compared to the rel_body current values -- called automatically if auto_updt_rels is set
  virtual void  SaveCurAsPrv();
  // #IGNORE go through all the bodies in this object and save current vals as prv_* -- needed for UpdateCurToRels

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

  SIMPLE_COPY(VEObject);
  SIMPLE_INITLINKS(VEObject);
  override void CutLinks();
  TA_BASEFUNS(VEObject);
protected:
  SpaceType     cur_space_type; // current type that was previously set
  //  override CheckConfig_impl() // todo
private:
  void  Initialize();
  void  Destroy();
};

SmartRef_Of(VEObject); // VEObjectRef

#endif // VEObject_h
