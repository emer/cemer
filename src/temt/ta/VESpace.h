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

#ifndef VESpace_h
#define VESpace_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <VEStatic_Group>
#include <MinMaxInt>

// declare all other types mentioned but not required to include:
class VEWorld; // 
class VEStatic; // 


class TA_API VESpace : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_veobj a virtual environment that represents a sub-space of objects, containing static elements only -- helps optimize the collision detection process to group proximal items into sub spaces
INHERITED(taNBase)
public:
  enum SpaceType {
    SIMPLE_SPACE,               // simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,                 // hash-coded space with multi-scale grids: efficient for larger number of items
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  VEStatic_Group static_els;    // static elements of the space

  void*         space_id;       // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  SpaceType     space_type;     // type of space to use
  MinMaxInt     hash_levels;    // #CONDSHOW_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

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

  SIMPLE_COPY(VESpace);
  SIMPLE_INITLINKS(VESpace);
  override void CutLinks();
  TA_BASEFUNS(VESpace);
protected:
  SpaceType     cur_space_type; // current type that was previously set
  //  override CheckConfig_impl() // todo
private:
  void  Initialize();
  void  Destroy();
};

SmartRef_Of(VESpace,TA_VESpace); // VESpaceRef

#endif // VESpace_h
