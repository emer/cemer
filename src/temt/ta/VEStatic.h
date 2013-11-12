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

#ifndef VEStatic_h
#define VEStatic_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taVector3f>
#include <taAxisAngle>
#include <taQuaternion>
#include <taVector2f>
#include <taTransform>
#include <taColor>
#include <VETexture>
#include <VESurface>
#include <ODEIntParams>

// declare all other types mentioned but not required to include:
class VEWorld; // 
class VESpace; // 

class VEStatic; //
SmartRef_Of(VEStatic); // VEStaticRef

taTypeDef_Of(VEStatic);

class TA_API VEStatic : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vestc virtual environment static environment element -- not subject to physics and only interacts with bodies via collisions (cannot be part of a joint)
INHERITED(taNBase)
public:
  enum StaticFlags { // #BITS flags for static elements
    SF_NONE             = 0, // #NO_BIT
    OFF                 = 0x0001, // turn this object off -- do not include in the virtual world
    FM_FILE             = 0x0002, // load object image features from Inventor (iv) object file
    EULER_ROT           = 0x0004, // use euler rotation angles instead of axis and angle
    CUR_FM_FILE         = 0x0020, // #NO_SHOW #READ_ONLY current flag setting load object image features from Inventor (iv) object file

    INIT_WAS_ABS        = 0x1000, // #NO_BIT init vals were specified in abs coords last time (not rel)
  };

  enum Shape {                  // shape of the object -- used for intertial mass and for collision detection (unless use_fname
    SPHERE,
    CAPSULE,                    // a cylinder with half-spheres on each end -- preferred to standard cylinders for collision detection
    CYLINDER,
    BOX,
    PLANE,                      // flat plane -- useful for ground surfaces
    NO_SHAPE,                   // no shape at all -- only for special classes light lights
  };
  enum LongAxis {
    LONG_X=1,                   // long axis is in X direction
    LONG_Y,                     // long axis is in Y direction
    LONG_Z,                     // long axis is in Z direction
  };
  enum NormAxis {
    NORM_X=1,                   // normal axis is in X direction
    NORM_Y,                     // normal axis is in Y direction
    NORM_Z,                     // normal axis is in Z direction
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*         geom_id;        // #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry associated with the static item (cast to a dGeomID which is dxgeom*)
  StaticFlags   flags;          // flags for various env el properties

  bool          relative;       // position and rotation are relative to another static object
  VEStaticRef   rel_static;     // #CONDSHOW_ON_relative other static body that our position and rotatio are computed relative to -- in general better if rel_static is before this one in list of objects, especially if it is also relative to something else, so everything gets updated in the proper order.  definitely avoid loops!
  taVector3f    rel_pos;        // #CONDSHOW_OFF_shape:PLANE||!relative relative position of static item, relative to rel_static
  taAxisAngle   rel_rot;        // #CONDSHOW_OFF_flags:EULER_ROT||!relative relative rotation of static item compared to rel_static in terms of axis and angle (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- not applicable to PLANE shape
  taVector3f    rel_euler;      // #CONDSHOW_ON_flags:EULER_ROT&&relative relative rotation of static item compard to rel_static (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- not applicable to PLANE shape
  taQuaternion  rel_quat;      // #READ_ONLY final quat for either rel_rot or rel_euler -- doesn't have the final correction for capsule axis etc

  taVector3f    pos;            // #CONDSHOW_OFF_shape:PLANE||relative position of static item
  taAxisAngle   rot;            // #CONDEDIT_OFF_flags:EULER_ROT||relative rotation of static item in terms of axis and angle (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- not applicable to PLANE shape
  taVector3f    rot_euler;      // #CONDEDIT_ON_flags:EULER_ROT&&!relative rotation of static item (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- not applicable to PLANE shape
  taQuaternion  rot_quat;       // #READ_ONLY quaternion representation of the rotation -- automatically converted from rot or rot_euler depending on EULER_ROT flag

  Shape         shape;          // shape of static item for purposes of collision (and visual rendering if not FM_FILE)
  float         radius;         // #CONDSHOW_OFF_shape:BOX,PLANE radius of body, for all but box
  float         length;         // #CONDSHOW_OFF_shape:BOX,PLANE,SPHERE length of body, for all but box
  LongAxis      long_axis;      // #CONDSHOW_OFF_shape:BOX,PLANE,SPHERE direction of the long axis of the body (where length is oriented)
  taVector3f    box;            // #CONDSHOW_ON_shape:BOX length of box in each axis for BOX-shaped body
  NormAxis      plane_norm;     // #CONDSHOW_ON_shape:PLANE direction of the plane normal axis (which way is "up" for a ground plane)
  float         plane_height;   // #CONDSHOW_ON_shape:PLANE height of the plane above/below 0 in the plane norm axis
  taVector2f    plane_vis_size; // #CONDSHOW_ON_shape:PLANE extent of the plane to actually render in the display (displayed as a very thin box of this size, centered at 0,0,0) -- actual plane in physical system is of infinite extent!

  taTransform   obj_xform;      // #CONDSHOW_ON_flags:FM_FILE full transform to apply to object file to align/size/etc with static item
  String        obj_fname;      // #CONDSHOW_ON_flags:FM_FILE #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Inventor file that describes static item appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  bool          set_color;      // if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor       color;          // #CONDSHOW_ON_set_color default color of body if not otherwise defined ('a' alpha value used for transparency -- 1 = fully opaque (transparency = 0), 0 = fully transparent (transparency = 1)) -- this is the diffuse color, which reflects light sources in a diffuse way -- turn on full_colors for other options
  bool          full_colors;    // #CONDSHOW_ON_set_color if true, then specify the full set of colors that can be specified for an object (ambient, specular, emissive, and shininess) -- main color is the diffuse color, which is basic color that reflects from light sources
  taColor       ambient_color;  // #CONDSHOW_ON_set_color&&full_colors the ambient color is not affected by any light sources, and gives some background color -- typically dark grey
  taColor       specular_color; // #CONDSHOW_ON_set_color&&full_colors the specular color is the color of the shiny reflections from an object -- the alpha value of this color determines the shininess parameter for the object, which controls how much of this color is generated
  taColor       emissive_color; // #CONDSHOW_ON_set_color&&full_colors this is light generated by the object as though it were glowing -- is independent of light sources
  VETextureRef  texture;        // #SCOPE_VEWorld texture mapping of an image to the object (textures are shared resources defined in VEWorld)
  VESurface     surface;        // surface properties of the static item
  ODEIntParams  softness;       // set the cfm and erp values higher here to make the surface softer

  //////////////////////////////
  //    Internal-ish stuff

  override int          GetEnabled() const {  return !HasStaticFlag(OFF); }
  override void         SetEnabled(bool value) { SetStaticFlagState(OFF, !value); }
  override String       GetDesc() const { return desc; }
  inline void           SetStaticFlag(StaticFlags flg)   { flags = (StaticFlags)(flags | flg); }
  // set body flag state on
  inline void           ClearStaticFlag(StaticFlags flg) { flags = (StaticFlags)(flags & ~flg); }
  // clear body flag state (set off)
  inline bool           HasStaticFlag(StaticFlags flg) const { return (flags & flg); }
  // check if body flag is set
  inline void           SetStaticFlagState(StaticFlags flg, bool on)
  { if(on) SetStaticFlag(flg); else ClearStaticFlag(flg); }
  // set body flag state according to on bool (if true, set flag, if false, clear it)

  virtual VEWorld* GetWorld();  // #CAT_ODE get world object (parent of this guy)
  virtual void* GetWorldID();   // #CAT_ODE get world id value
  virtual VESpace* GetSpace();  // #CAT_ODE get parent space (parent of this guy)
  virtual void* GetSpaceID();   // #CAT_ODE get space id value from space

  virtual bool  CreateODE();    // #CAT_ODE create static element in ode (if not already created) -- returns false if unable to create
  virtual void  DestroyODE();   // #CAT_ODE destroy static element in ode (if created)

  virtual void  Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void  SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void  GetInitFromRel();
  // #CAT_ODE #EXPERT if relative is on, this will compute init values from relative values -- called automatically during Init() and UAE
  virtual void  InitRotFromCur();
  // #IGNORE set init rotation parameters from current rotation (rot_quat)
  virtual void  UpdateCurFromRel();
  // #CAT_ODE #EXPERT if relative is on, this will compute current position and rotation from relative offsets compared to the rel_static current values -- see also VEStatic::UpdateCurToRels
  virtual void  SaveCurAsPrv();
  // #IGNORE save current vals as prv_* -- needed for UpdateCurToRels

  virtual void  Init_Shape();   // #CAT_ODE set shape information
  virtual void  Init_PosRot();  // #CAT_ODE set position and rotation

  bool  IsCurShape()  { return ((shape == cur_shape) &&
                                (HasStaticFlag(FM_FILE) == HasStaticFlag(CUR_FM_FILE))); }
  // #CAT_ODE is the ODE guy actually configured for the current shape or not?

  virtual void  Translate(float dx, float dy, float dz, bool abs_pos = false);
  // #BUTTON #DYN1 #CAT_ODE move object given distance (can select multiple and operate on all at once) -- if abs_pos then set directly to coordinates instead of adding them to current values
  virtual void  Scale(float sx, float sy=0.0f, float sz=0.0f);
  // #BUTTON #DYN1 #CAT_ODE scale size of object -- if sy or sz is 0, then sx is used for that dimension (can select multiple and operate on all at once)
  virtual void  RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool abs_rot = false);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though -- if abs_rot then set directly to coordinates instead of adding them to current values
  virtual void  RotateEuler(float euler_x, float euler_y, float euler_z, bool abs_rot = false);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values -- if abs_rot then set directly to coordinates instead of adding them to current values
  virtual void  SnapPosToGrid(float grid_size=0.05f);
  // #BUTTON #DYN1 #CAT_ODE snap the position of static body to grid of given size
  virtual void  CopyColorFrom(VEStatic* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other object

  SIMPLE_COPY(VEStatic);
  SIMPLE_INITLINKS(VEStatic);
  override void CutLinks();
  TA_BASEFUNS(VEStatic);
protected:
  Shape         cur_shape;      // current shape that was previously set
  taVector3f    prv_pos;      // #IGNORE previous cur_pos value -- set prior to a Translate function move, for use by UpdateCurFromRel
  taQuaternion  prv_quat;      // #IGNORE previous cur_quat rotation value -- set prior to a Rotate function rotation, for use by UpdateCurFromRel

  override void         UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
};

#endif // VEStatic_h
