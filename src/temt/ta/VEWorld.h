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

#ifndef VEWorld_h
#define VEWorld_h 1

// parent includes:
#include <taNBase>
#include <ODEIntParams>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <MinMaxInt>
#include <taVector3f>
#include <VEObject_Group>
#include <VESpace_Group>
#include <VETexture_List>
#include <VECamera>
#include <taColor>
#include <VELightParams>
#include <VELight>

// declare all other types mentioned but not required to include:
class VEWorldView; // 
class T3Panel; // 
class QImage; // 
class taImage; // 

class VEWorld; //
SmartRef_Of(VEWorld); // VEWorldRef

taTypeDef_Of(ODEWorldParams);

class TA_API ODEWorldParams : public ODEIntParams {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
  INHERITED(ODEIntParams)
public:
  float         max_cor_vel;    // #DEF_1e+06 maximum correcting velocity for contacts (how quickly they can pop things out of contact)
  float         contact_depth;  // #DEF_0.001 depth of the surface layer arond all geometry objects -- allows things to go this deep into a surface before actual contact is made -- increased value can help prevent jittering
  int           max_col_pts;    // #DEF_4 maximum number of collision points to get (must be less than 64, which is a hard maximum)

  void  Initialize();
  void  Destroy()       { };
  SIMPLE_COPY(ODEWorldParams);
  TA_BASEFUNS(ODEWorldParams);
protected:
  void  UpdateAfterEdit_impl();
};

taTypeDef_Of(VEWorld);

class TA_API VEWorld : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vewld a virtual environment world
INHERITED(taNBase)
public:
  enum  StepType {              // which type of stepping function to use
    STD_STEP,
    QUICK_STEP,
  };
  enum SpaceType {
    SIMPLE_SPACE,               // simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,                 // hash-coded space with multi-scale grids: efficient for larger number of items
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*         world_id;       // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the world (cast to a dWorldID which is dxworld*)
  void*         space_id;       // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  void*         cgp_id;         // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the contact group (cast to a dJointGroupID
  SpaceType     space_type;     // type of space to use (typically HASH_SPACE is good for worlds having more objects)
  MinMaxInt     hash_levels;    // #CONDSHOW_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  StepType      step_type;      // what type of stepping function to use
  float         stepsize;       // how big of a step to take
  int           quick_iters;    // #CONDSHOW_ON_step_type:QUICK_STEP how many iterations to take in quick step mode
  taVector3f    gravity;        // gravitational setting for world (0,0,-9.81) is std
  bool          updt_display;   // if true, will update any attached display after each time step
  ODEWorldParams ode_params;    // parameters for tuning the ODE engine

  VEObject_Group objects;       // objects in the world
  VESpace_Group  spaces;        // spaces (static elements) in the world
  VETexture_List textures;      // shared textures used by bodies in the world

  VECameraRef   camera_0;       // first camera to use in rendering images (first person view) -- must be set to point to a camera in the set of objects for it to be used
  VECameraRef   camera_1;       // second camera to use in rendering images (for stereo vision)-- must be set to point to a camera in the set of objects for it to be used
  taColor       bg_color;       // background color of display for camera images
  VELightParams sun_light;      // parameters for a sun (overhead directional) light
  VELightRef    light_0;        // first light to add to scene -- must be set to point to a light in the set of objects for it to be used
  VELightRef    light_1;        // second light to add to scene -- must be set to point to a light in the set of objects for it to be used

  static taBaseRef  last_to_set_ode; // #IGNORE last VE object to set values to ode -- useful for debugging errors
  static VEWorldRef last_ve_stepped; // #IGNORE last VEWorld to run -- useful for debugging errors

  override String       GetDesc() const { return desc; }
  virtual bool  CreateODE();    // #CAT_ODE create world in ode (if not already created) -- returns false if unable to create
  virtual void  DestroyODE();   // #CAT_ODE destroy world in ode (if created)
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void  CurFromODE();
  // #CAT_ODE get the current updated values from ODE after computing (called after each step)

  virtual void  Init();
  // #BUTTON #CAT_ODE initialize the virtual environment, placing all objects in their init configurations, updating with any added objects, etc
  virtual void  Step();
  // #BUTTON #CAT_ODE take one step of integration, and get updated values
  virtual void  Reset() { DestroyODE(); SetValsToODE(); }
  // #BUTTON #CAT_ODE completely reset the ODE environment -- this is necessary if bad float numbers have been generated (nan, inf)

  virtual void  CurToODE();
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call
  virtual void  CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init or SetValsToODE -- for all bodies
  virtual void  SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of all bodies and static objects to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos

  VEWorldView*  NewView(T3Panel* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #BUTTON #CAT_Display make a new viewer of this world (NULL=use existing empty frame if any, else make new frame)
  VEWorldView*  FindView();
  // #CAT_Display find first existing viewer of this world (if it exists)
  virtual void  UpdateView();
  // #CAT_Display manually update the view information -- used for offscreen render updates

  virtual QImage GetCameraImage(int camera_no);
  // #CAT_ODE #IGNORE get camera image from given camera number (image may be null if camera not set)

  virtual bool GetCameraTaImage(taImage& ta_img, int camera_no);
  // #CAT_ODE gets camera image from given camera number into given taImage object -- returns false if unsuccessful

  static float SnapVal(float val, float grid_size);
  // snap value to grid increment

  //////////////////////////////////////
  //    IMPL functions
#ifndef __MAKETA__
  virtual void  CollisionCallback(dGeomID o1, dGeomID o2);
  // #IGNORE callback function for the collision function, with two objects that are actual objects and not spaces
#endif

  void Copy_(const VEWorld& cp);
  SIMPLE_INITLINKS(VEWorld);
  override void CutLinks();
  TA_BASEFUNS(VEWorld);
protected:
  SpaceType     cur_space_type; // current type that was previously set

private:
  void  Initialize();
  void  Destroy();
};


#endif // VEWorld_h
