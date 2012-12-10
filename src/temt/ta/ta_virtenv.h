// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef TA_VIRTENV_H
#define TA_VIRTENV_H

#include "ta_base.h"                         // Definition of the taBase object.
#include "ta_geometry.h"
#include "ta_datatable.h"
#include "colorscale.h"		// taColor
#include "minmax.h"

/////////////////////////////////////////////////////////////////////////////////
//		Actual ODE Code
#ifndef __MAKETA__
# include <ode/ode.h>
#endif

class VEBody;
class VEBody_Group;
class VEJoint;
class VEJoint_Group;
class VEObject;
class VEObject_Group;
class VEStatic;
class VEStatic_Group;
class VESpace;
class VESpace_Group;
class VEWorld;
class VEWorldView;
class VEArm;
class VELinearMuscle;
class VELinMuscle_Group;

class taImage;

class SoSwitch;			// #IGNORE

class TA_API ODEIntParams : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
INHERITED(taOBase)
public:
  float		erp;		// #DEF_0.2 (0-1, .1-.8 useful range) error reduction parameter: how much of the joint error is reduced at the next time step
  float		cfm;		// #DEF_1e-05 (0-1, 1e-9 - 1 useful range) constraint force mixing parameter: how "soft" is the constraint (0 = hard, 1 = soft)

  TA_SIMPLE_BASEFUNS(ODEIntParams);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class TA_API ODEDamping : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE damping parameters
INHERITED(taOBase)
public:
  bool		on;		// #DEF_false whether to use any of these damping parameters
  float		lin;		// #DEF_0 #CONDSHOW_ON_on The body's linear damping scale.
  float		lin_thr;	// #DEF_0 #CONDSHOW_ON_on The body's linear damping threshold. Damping will be applied only if the linear speed is above the threshold limit
  float		ang;		// #DEF_0 #CONDSHOW_ON_on The body's angular damping scale.
  float		ang_thr;	// #DEF_0 #CONDSHOW_ON_on The body's angular damping threshold. Damping will be applied only if the angular speed is above the threshold limit
  float         ang_speed;         // #DEF_0 #CONDSHOW_ON_on You can also limit the maximum angular speed. In contrast to the damping functions, the angular velocity is affected before the body is moved. This means that it will introduce errors in joints that are forcing the body to rotate too fast. Some bodies have naturally high angular velocities (like cars' wheels), so you may want to give them a very high (like the default, infinity) limit.

  TA_SIMPLE_BASEFUNS(ODEDamping);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class TA_API ODEFiniteRotation : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv finite rotation mode settings
INHERITED(taOBase)
public:
  bool		on;		// #DEF_false whether to enable finite rotation mode -- False: An "infinitesimal" orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. True: A finite orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
  taVector3f  axis; // #CONDSHOW_ON_on This sets the finite rotation axis for a body, if finite rotation mode is being used. If this axis is zero (0,0,0), full finite rotations are performed on the body. If this axis is nonzero, the body is rotated by performing a partial finite rotation along the axis direction followed by an infinitesimal rotation along an orthogonal direction. This can be useful to alleviate certain sources of error caused by quickly spinning bodies. For example, if a car wheel is rotating at high speed you can set the wheel's hinge axis here to try and improve its behavior.

  TA_SIMPLE_BASEFUNS(ODEFiniteRotation);
private:
  void	Initialize();
  void	Destroy()	{ };
};


////////////////////////////////////////////////
//		Bodies

class TA_API VESurface : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv surface properties for collisions
INHERITED(taOBase)
public:

  float		friction; 	// (0-1e22) coulomb friction coefficient (mu). 0 = frictionless, 1e22 = infinity = no slipping
  float		bounce;		// (0-1) how bouncy is the surface (0 = hard, 1 = maximum bouncyness) 
  float		bounce_vel;	// minimum incoming velocity necessary for bounce -- incoming velocities below this will have a bounce parameter of 0

  TA_SIMPLE_BASEFUNS(VESurface);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class SoTexture2; // #IGNORE
class SoTexture2Transform; // #IGNORE

class TA_API VETexture : public taNBase {
  // #STEM_BASE #NO_UPDATE_AFTER ##CAT_VirtEnv texture mapping of an image onto a 3d object -- defined as a shared resource in the VEWorld that individual objects can point to
INHERITED(taNBase)
public:
  enum Mode {
    MODULATE,			// texture color is multiplied by the underlying object color, result is shaded by lighting
    DECAL,			// image overwrites polygon color entirely, result is NOT shaded by lighting (does not work for grayscale texture images! see REPLACE)
    BLEND,			// for monochrome textures, the intensity is used to blend between the shaded color of the object and the color in blend_color
    REPLACE,			// image overwrites object color entirely (same as DECAL but works for greyscale images)
  };
 
  enum Wrap {
    REPEAT,			// repeat texture to fill object size
    CLAMP,			// clamp (repeats border pixels to end of object dimension)
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  String       	fname;		// #FILE_DIALOG_LOAD #FILETYPE_Image full file name (including path if not in same dir as project) for the texture image file (formats supported: JPEG, PNG, GIF, TIFF, RGB, PIC, TGA)
  Mode		mode;		// how to apply texture to object
  Wrap		wrap_horiz;	// how to wrap in the horizontal (largest) dimension
  Wrap		wrap_vert;	// how to wrap in the vertical (smallest) dimension
  taVector2f    offset;	// [0,0] offset of image from 0,0 (x=horizontal dim, y=vertical dim, shape dependent)
  taVector2f    scale;		// [1,1] scaling of image (x=horizontal dim, y=vertical dim, shape dependent)
  float 	rot;		// [0] rotation in the plane, in degrees
  taVector2f    center;	// [0,0] center point for scale and rotation
  taColor	blend_color;	// #CONDSHOW_ON_mode:BLEND color to use in blend case

  int		idx;
  // #READ_ONLY #HIDDEN #NO_COPY #CAT_Structure index of this unit within containing unit group

  virtual void		SetTexture(SoTexture2* sotx);
  // configure the texture based on values
  virtual bool		UpdateTexture();
  // #BUTTON if environment is already initialized and viewed, this will update the texture actually shown in the view

  virtual bool	  	NeedsTransform(); // determines if SoTexture2Transform is needed
  virtual void		SetTransform(SoTexture2Transform* sotx);
  // configure the transform based on values
  
  override String	GetDesc() const { return desc; }
  override int	GetIndex() const { return idx; }
  override void	SetIndex(int i) { idx = i; }

  void	Initialize();
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(VETexture);
// protected:
//   void	UpdateAfterEdit_impl();
};

SmartRef_Of(VETexture,TA_VETexture); // VETextureRef

class TA_API VETexture_List : public taList<VETexture> {
  // ##CAT_VirtEnv a list of virtual environment textures
INHERITED(taList<VETexture>)
public:
  TA_BASEFUNS_NOCOPY(VETexture_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_VETexture); }
  void 	Destroy()		{ };
};


class TA_API VEBody : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vebod virtual environment body (rigid structural element), subject to physics dynamics
INHERITED(taNBase)
public:	
  enum BodyFlags { // #BITS flags for bodies
    BF_NONE		= 0, // #NO_BIT
    OFF 		= 0x0001, // turn this object off -- do not include in the virtual world
    FIXED 		= 0x0002, // body cannot move at all
    EULER_ROT		= 0x0004, // use euler rotation angles instead of axis and angle
    PLANE2D		= 0x0008, // body is constrained to the Z=0 plane
    FM_FILE		= 0x0010, // load object image features from Inventor (iv) object file
    NO_COLLIDE		= 0x0020, // this body is not part of the collision detection system -- useful for light beams and other ephemera
    CUR_FM_FILE		= 0x0040, // #NO_SHOW #READ_ONLY current flag setting load object image features from Inventor (iv) object file
    GRAVITY_ON		= 0x0080, // does gravitational force affect this body?
  };
    //    COLLIDE_FM_FILE	= 0x0008, // use object shape from file for collision detection (NOTE: this is more computationally expensive and requires trimesh feature to be enabled in ode)

  enum Shape {			// shape of the object -- used for intertial mass and for collision detection (unless use_fname
    SPHERE,
    CAPSULE,			// a cylinder with half-spheres on each end -- preferred to standard cylinders for collision detection
    CYLINDER,
    BOX,
    NO_SHAPE,			// no shape at all -- only for special classes like lights
  };
  enum LongAxis {
    LONG_X=1,			// long axis is in X direction
    LONG_Y,			// long axis is in Y direction
    LONG_Z,			// long axis is in Z direction
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*		body_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the body (cast to a dBodyID which is dxbody*)
  void*		geom_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry associated with the body (cast to a dGeomID which is dxgeom*)
  BodyFlags	flags;		// flags for various body properties
  taVector3f	init_pos;  	// initial position of body (when creating it)
  taAxisAngle	init_rot;  	// #CONDSHOW_OFF_flags:EULER_ROT initial rotation of body in terms of an axis and angle (when creating it) (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  taVector3f	init_euler;  	// #CONDSHOW_ON_flags:EULER_ROT initial rotation of body in Euler angles in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854
  taQuaternion  init_quat;  	// #READ_ONLY initial rotation of body in Quaternion form -- automatically converted from init_rot or init_euler depending on EULER_ROT flag
  taVector3f	init_lin_vel;	// initial linear velocity
  taVector3f	init_ang_vel;	// initial angular velocity

  taVector3f	cur_pos;  	// current position of body
  taAxisAngle	cur_rot;  	// #CONDSHOW_OFF_flags:EULER_ROT current rotation of body (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  taVector3f	cur_euler;  	// #CONDSHOW_ON_flags:EULER_ROT current rotation of body in Euler angles in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854
  taQuaternion  cur_quat;  	// #READ_ONLY current rotation of body in Quaternion form
  taVector3f	cur_lin_vel;	// current linear velocity
  taVector3f	cur_ang_vel;	// current angular velocity

  Shape		shape;		// shape of body for purposes of mass/inertia and collision (and visual rendering if not FM_FILE)
  float		mass;		// total mass of body (in kg)
  float		radius;		// #CONDSHOW_OFF_shape:BOX radius of body, for all but box
  float		length;		// #CONDSHOW_OFF_shape:BOX,SPHERE length of body, for all but box 
  LongAxis	long_axis;	// #CONDSHOW_OFF_shape:BOX,SPHERE direction of the long axis of the body (where length is oriented) -- only for CAPSULE and CYLINDER -- this creates an additional rotation between init_ rotation and cur_ rotation, only applied at time of init
  taVector3f	box;		// #CONDSHOW_ON_shape:BOX length of box in each axis for BOX-shaped body

  taTransform obj_xform;	// #CONDSHOW_ON_flags:FM_FILE full transform to apply to body file to align/size/etc with body
  String	obj_fname;	// #CONDSHOW_ON_flags:FM_FILE #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Inventor file that describes body appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  bool		set_color;	// if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor	color; 		// #CONDSHOW_ON_set_color default color of body if not otherwise defined ('a' alpha value used for transparency -- 1 = fully opaque (transparency = 0), 0 = fully transparent (transparency = 1)) -- this is the diffuse color, which reflects light sources in a diffuse way -- turn on full_colors for other options
  bool		full_colors; 	// #CONDSHOW_ON_set_color if true, then specify the full set of colors that can be specified for an object (ambient, specular, emissive, and shininess) -- main color is the diffuse color, which is basic color that reflects from light sources
  taColor	ambient_color; 	// #CONDSHOW_ON_set_color&&full_colors the ambient color is not affected by any light sources, and gives some background color -- typically dark grey
  taColor	specular_color;	// #CONDSHOW_ON_set_color&&full_colors the specular color is the color of the shiny reflections from an object -- the alpha value of this color determines the shininess parameter for the object, which controls how much of this color is generated
  taColor	emissive_color;	// #CONDSHOW_ON_set_color&&full_colors this is light generated by the object as though it were glowing -- is independent of light sources
  VETextureRef	texture;	// #SCOPE_VEWorld #NULL_OK texture mapping of an image to the object (textures are shared resources defined in VEWorld)
  VESurface	surface;	// physics surface properties of the body (softness, bounciness)
  ODEIntParams	softness;	// set the cfm and erp values higher here to make the surface softer
  ODEDamping 	damp; 		// Linear and angular damping, to remove energy and generally provide greater robustness
  ODEFiniteRotation finite_rotation; // controls the way a body's orientation is updated at each time step

  //////////////////////////////
  //	Internal-ish stuff
#ifndef __MAKETA__
  dMass		mass_ode;	// #IGNORE full ode mass of body
#endif
  void*		fixed_joint_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of joint used to fix a FIXED body

  override int		GetEnabled() const {  return !HasBodyFlag(OFF); }
  override void		SetEnabled(bool value) { SetBodyFlagState(OFF, !value); }
  override String	GetDesc() const { return desc; }
  inline void		SetBodyFlag(BodyFlags flg)   { flags = (BodyFlags)(flags | flg); }
  // set body flag state on
  inline void		ClearBodyFlag(BodyFlags flg) { flags = (BodyFlags)(flags & ~flg); }
  // clear body flag state (set off)
  inline bool		HasBodyFlag(BodyFlags flg) const { return (flags & flg); }
  // check if body flag is set
  inline void		SetBodyFlagState(BodyFlags flg, bool on)
  { if(on) SetBodyFlag(flg); else ClearBodyFlag(flg); }
  // set body flag state according to on bool (if true, set flag, if false, clear it)


  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value
  virtual VEObject* GetObject();// #CAT_ODE get parent object (parent of this guy)
  virtual void*	GetObjSpaceID(); // #CAT_ODE get object space id value

  virtual bool	CreateODE();	// #CAT_ODE create body in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy body in ode (if created)

  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void	Init_Shape();	// #CAT_ODE #EXPERT set shape information
  virtual void	Init_Pos();	// #CAT_ODE #EXPERT set initial position
  virtual void	Init_Rotation();	// #CAT_ODE #EXPERT set initial rotation
  virtual void	Init_Velocity();	// #CAT_ODE #EXPERT set initial velocity (linear and angular)
  virtual void	Init_Mass();	// #CAT_ODE #EXPERT set the mass of body in ODE
  virtual void	Init_FiniteRotation(); // #CAT_ODE #EXPERT set the finite rotation mode of body in ODE
  virtual void	Init_Gravity();	// #CAT_ODE #EXPERT set the gravity mode
  virtual void	Init_Damping();	// #CAT_ODE #EXPERT set the damping parameters

  virtual void	CurToODE();	
  // #CAT_ODE #BUTTON #GHOST_ON_body_id:NULL set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call

  virtual void	CurToODE_Pos();	     // #CAT_ODE #EXPERT set current position
  virtual void	CurToODE_Rotation();	// #CAT_ODE #EXPERT set current rotation
  virtual void	CurToODE_Velocity();	// #CAT_ODE #EXPERT set current velocity (linear and angular)

  virtual void	CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing

  virtual void	UpdateCurRotFmQuat();
  // #CAT_ODE #EXPERT update current rotation parameters from cur_quat read from ODE or whenever cur_quat might be set externally (e.g., gui dragging)
  virtual void	InitRotFromCur();
  // #CAT_ODE #EXPERT set init rotation parameters from current rotation

  virtual void	Translate(float dx, float dy, float dz, bool init);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos 
  virtual void	Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation in Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot

  virtual void 	AddForce(float fx, float fy, float fz, bool torque=false, bool rel=false);
  // #BUTTON #CAT_ODE add given force vector to object at its center of mass -- if torque then it is a torque (angular force), otherwise linear -- if rel then force is relative to the objects own frame of reference (orientation) -- otherwise it is in the global reference frame
  virtual void 	AddForceAtPos(float fx, float fy, float fz, float px, float py, float pz,
			      bool rel_force=false, bool rel_pos=false);
  // #BUTTON #CAT_ODE add given force vector to object at given position on object -- rel_force and rel_pos specify values relative to the reference frame (orientation, position) of the body, in contrast to global reference frame coordinates
  virtual void	CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init 
  virtual void	SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of body to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void	CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  //////////////////////////////
  //	Set Damping

  virtual void SetLinearDamping(float ldamp);
  // #CAT_ODE Set the body's linear damping scale. After setting a damping scale, the body will ignore the world's damping scale until dBodySetDampingDefaults() is called.
  virtual void SetAngularDamping(float adamp);
  // #CAT_ODE Set the body's angular damping scale. After setting a damping scale, the body will ignore the world's damping scale until dBodySetDampingDefaults() is called.
  virtual void SetLinearDampingThreshold(float lthresh);
  // #CAT_ODE Set the body's linear damping threshold. The damping will be applied only if the linear speed is above the threshold limit
  virtual void SetAngularDampingThreshold(float athresh);
  // #CAT_ODE Set the body's angular damping threshold. The damping will be applied only if the angular speed is above the threshold limit
  virtual void SetMaxAngularSpeed(float maxaspeed);
  // #CAT_ODE You can also limit the maximum angular speed. In contrast to the damping functions, the angular velocity is affected before the body is moved. This means that it will introduce errors in joints that are forcing the body to rotate too fast. Some bodies have naturally high angular velocities (like cars' wheels), so you may want to give them a very high (like the default, dInfinity) limit.

  //////////////////////////////
  //	Finite rotation mode - important for wheels

  virtual void SetFiniteRotationMode(int rotmode);
  // #CAT_ODE This function controls the way a body's orientation is updated at each time step. The mode argument can be 0 or 1.0: An "infinitesimal" orientation update is used. This is fast to compute, but it can occasionally cause inaccuracies for bodies that are rotating at high speed, especially when those bodies are joined to other bodies. This is the default for every new body that is created. 1: A finite orientation update is used. This is more costly to compute, but will be more accurate for high speed rotations. Note however that high speed rotations can result in many types of error in a simulation, and this mode will only fix one of those sources of error.
  virtual void SetFiniteRotationAxis(float xr, float yr, float zr);
  //#CAT_ODE This sets the finite rotation axis for a body. This is axis only has meaning when the finite rotation mode is set (see dBodySetFiniteRotationMode). If this axis is zero (0,0,0), full finite rotations are performed on the body. If this axis is nonzero, the body is rotated by performing a partial finite rotation along the axis direction followed by an infinitesimal rotation along an orthogonal direction. This can be useful to alleviate certain sources of error caused by quickly spinning bodies. For example, if a car wheel is rotating at high speed you can call this function with the wheel's hinge axis as the argument to try and improve its behavior.

  //////////////////////////////
  //	Gravity mode

  virtual void SetGravityMode(int mode);
  // #CAT_ODE Set whether the body is influenced by the world's gravity or not. If mode is nonzero it is, if mode is zero, it isn't. Newly created bodies are always influenced by the world's gravity.

  bool	IsCurShape()  { return ((shape == cur_shape) &&
 				(HasBodyFlag(FM_FILE) == HasBodyFlag(CUR_FM_FILE))); }
  // #CAT_ODE is the ODE guy actually configured for the current shape or not?

  SIMPLE_COPY(VEBody);
  SIMPLE_INITLINKS(VEBody);
  override void CutLinks();
  TA_BASEFUNS(VEBody);
protected:
  Shape		cur_shape;	// #IGNORE current shape that was previously set -- for detecting updates
  LongAxis	cur_long_axis;	// #IGNORE long axis that was previously set -- for detecting updates

  override void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEBody,TA_VEBody); // VEBodyRef

class TA_API VEBody_Group : public taGroup<VEBody> {
  // ##CAT_VirtEnv a group of virtual environment bodies
INHERITED(taGroup<VEBody>)
public:
  virtual void	Init();
  // #CAT_ODE #BUTTON initialize all ODE params and set to init_ settings

  virtual void	CurToODE();	
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call

  virtual void	CurFromODE(bool updt_disp = false);	// get the updated values from ODE after computing
  virtual void	DestroyODE();	// #CAT_ODE destroy ODE objs for these items

  virtual void	CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init

  virtual void	Translate(float dx, float dy, float dz, bool init);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos 
  virtual void	Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot
  virtual void	SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of bodies to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void	CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  TA_BASEFUNS_NOCOPY(VEBody_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEBody); }
  void 	Destroy()		{ };
};


////////////////////////////////////////////////
//		Camera & Lights

class TA_API VELightParams : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env light parameters
INHERITED(taOBase)
public:
  bool		on;		// is the light turned on?
  float		intensity;	// #CONDSHOW_ON_on (0-1) how bright is the light
  taColor	color; 		// #CONDSHOW_ON_on color of light

  TA_SIMPLE_BASEFUNS(VELightParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class TA_API VECameraDists : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env camera distances
INHERITED(taOBase)
public:
  float		near;		// #DEF_0.1 near distance of camera -- closest things can be seen
  float		focal;		// focal distance of camera -- where is it focused on in scene?
  float		far;		// far distance of camera -- furthest things that can be seen

  TA_SIMPLE_BASEFUNS(VECameraDists);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class SoPerspectiveCamera; // #IGNORE

class TA_API VECamera : public VEBody {
  // virtual environment camera -- a body that contains a camera -- position and orientation are used to point the camera -- body shape always a cylinder, with LONG_Z axis, but mass/inertia etc is used if part of a non-fixed object -- camera must be selected in the VEWorld for it to actually be used to render images!
INHERITED(VEBody)
public:

#ifdef __MAKETA__
  Shape		shape;		// #READ_ONLY #HIDDEN shape for camera must always be a cylinder
  LongAxis	long_axis;	// #READ_ONLY #HIDDEN direction of the long axis of the body (where length is oriented) -- must always be LONG_Z for a camera
  taVector3f	box;		// #READ_ONLY #HIDDEN not relevant
#endif

  taVector2i	img_size;	// size of image to record from camera -- IMPORTANT -- if you will be using multiple cameras, it is very important that they all are the same size, otherwise the rendering will consume a lot of memory because a new buffer has to be created and destroyed each time (a shared buffer is used for all cameras)
  bool		color_cam;	// if true, get full color images (else greyscale)
  VECameraDists	view_dist;	// distances that are in view of the camera
  float		field_of_view;	// field of view of camera (angle in degrees) -- how much of scene is it taking in
  VELightParams	light;		// directional "headlight" associated with the camera -- ensures that whatever is being viewed can be seen (but makes lighting artificially consistent and good)
  taVector3f	dir_norm;	// #READ_ONLY #SHOW normal vector for where the camera is facing

  virtual void 		ConfigCamera(SoPerspectiveCamera* cam);
  // config So camera parameters

  override void	Init();
  override void	CurFromODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VECamera);
private:
  void 	Initialize();
  void  Destroy() { };
};

SmartRef_Of(VECamera,TA_VECamera); // VECameraRef

class SoLight; // #IGNORE

class TA_API VELight : public VEBody {
  // virtual environment light -- a body that contains a light source -- body shape is not rendered, but mass/inertia etc is used if part of a non-fixed object -- light only affects items after it in the list of objects!
INHERITED(VEBody)
public:	
  enum LightType {
    DIRECTIONAL_LIGHT,		// shines in a given direction, rotation is used for the direction, but position is not -- fastest rendering speed
    POINT_LIGHT,		// radiates in all directions, uses position for location
    SPOT_LIGHT,			// shines in a given direction from a given position
  };

#ifdef __MAKETA__
  LongAxis	long_axis;	// #READ_ONLY #HIDDEN direction of the long axis of the body (where length is oriented) -- must always be LONG_Z for a light
#endif

  LightType	light_type;	// type of light
  VELightParams	light;		// light parameters
  float		drop_off_rate;	// #CONDSHOW_ON_light_type:SPOT_LIGHT (0-1) how fast light drops off with increasing angle from the direction angle
  float		cut_off_angle;	// #CONDSHOW_ON_light_type:SPOT_LIGHT (45 default) angle in degrees from the direction vector where there will be no light
  taVector3f	dir_norm;	// #READ_ONLY #SHOW normal vector for where the camera is facing

  virtual SoLight*	CreateLight();
  // create the So light of correct type
  virtual void 		ConfigLight(SoLight* lgt);
  // config So light parameters
  virtual bool		UpdateLight();
  // #BUTTON if environment is already initialized and viewed, this will update the light in the display based on current settings

  override void	Init();
  override void	CurFromODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VELight);
private:
  void 	Initialize();
  void  Destroy() { };
};

SmartRef_Of(VELight,TA_VELight); // VELightRef

class TA_API VEObjCarousel : public VEBody {
  // virtual environment object carousel -- contains a number of different objects that are pre-loaded and then selectable by name or number (info is contained in a DataTable)
INHERITED(VEBody)
public:
#ifdef __MAKETA__
  String	obj_fname;	// #READ_ONLY #HIDDEN object FileName corresponding to given object number
#endif
  DataTableRef	obj_table;	// the data table containing FileName, FilePath columns (other columns can be present but are ignored) -- objects are loaded from FilePath and can be selected by FileName or row number
  int		cur_obj_no;	// #READ_ONLY #SHOW current object number to view -- select using ViewObjNo button/function, which drives the view update as well
  SoSwitch*	obj_switch;	// #IGNORE saved switch, to minimize reloading

  virtual bool	ViewObjNo(int obj_no);
  // #BUTTON select object to view by number, corresponding to the rows of the obj_table data table
  virtual bool	ViewObjName(const String& obj_name);
  // #BUTTON select object to view by FileName value in the obj_table data table
  virtual bool	LoadObjs(bool force = false);
  // #BUTTON load the objects listed in the obj_table -- if not forcing, checks to see if count is the same as number of rows in obj_table and does not reload if so (and returns false)

  virtual void	MakeSwitch();	// #IGNORE make the switch obj if it doesn't yet exist

  TA_SIMPLE_BASEFUNS(VEObjCarousel);
private:
  void 	Initialize();
  void  Destroy();
};


////////////////////////////////////////////////
//		Joints

class TA_API VEJointStops : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint stop parameters
INHERITED(taOBase)
public:
  bool		stops_on;	// turn on stops -- otherwise not used
  float		lo;		// #CONDSHOW_ON_stops_on stop for low angle or position value of joint
  float		hi;		// #CONDSHOW_ON_stops_on stop for high angle or position value of joint
  float		def;		// #CONDSHOW_ON_stops_on default angle or position value of joint -- where it likes to be
  float		bounce;		// #CONDSHOW_ON_stops_on how bouncy is the joint (0 = no bounce, 1 = maximum bounce)
  float		def_force;	// #CONDSHOW_ON_stops_on how much force to apply to return joint to default position -- effectively adds springs to the joint that pull it back to the default position -- NOTE: must call ApplyForce to have this computed and updated

  inline float	Range()	const		{ return (hi - lo); }
  inline float	Scale()	const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }

  float	Normalize(float val) const	{ return (val - lo) * Scale(); }
  // normalize given value to 0-1 range given current in hi

  float	Project(float val) const	{ return lo + (val * Range()); }
  // project a normalized value into the current lo-hi range

  TA_SIMPLE_BASEFUNS(VEJointStops);
// protected:
//   void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class TA_API VEJointMotor : public taOBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint motor parameters, including servo system -- drives joint into specified position -- forces computed and applied during the CurFromODE call, using the motor system (be sure to set f_max!)
INHERITED(taOBase)
public:
  bool		motor_on;	// turn on motor mechanism, defined by subsequent parameters
  float		vel;		// #CONDSHOW_ON_motor_on target joint velocity to achieve (angular or linear) -- set to 0 to provide a resistive damping force
  float		f_max;		// #CONDSHOW_ON_motor_on maximum force or torque to drive the joint to achieve desired velocity
  bool		servo_on;	// #CONDSHOW_ON_motor_on turn on servo mechanism, defined by subsequent parameters
  float		trg_pos;	// #CONDSHOW_ON_servo_on servo: target joint position to drive toward -- IMPORTANT: do not get too close to the stops with this, as it can cause numerical problems -- leave a .02 or so buffer
  float		gain;		// #CONDSHOW_ON_servo_on servo: how high to set the velocity on each step to move toward the target position: vel = gain * (trg_pos - pos) -- this can be quite high in fact: 20 or 50 have worked in various models, depending on the stepsize, masses involved, etc

  TA_SIMPLE_BASEFUNS(VEJointMotor);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
};

class TA_API ODEJointParams : public ODEIntParams {
  // ODE integration parameters for joints
INHERITED(ODEIntParams)
public:
  float 	no_stop_cfm;	// #DEF_1e-05 (0-1, 1e-9 - 1 useful range) constraint force mixing parameter when not at a stop: how "soft" is the constraint (0 = hard, 1 = soft)
  float		fudge;		// #DEF_1 (0-1) fudge factor for reducing force when a motor is present -- reduce this value if there is excessive jumping at the joint

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ODEJointParams);
  TA_BASEFUNS(ODEJointParams);
// protected:
//   void	UpdateAfterEdit_impl();
};

class TA_API VEJoint : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vejnt a virtual environment joint, which connects two bodies
INHERITED(taNBase)
public:	
  enum JointFlags { // #BITS flags for joints
    JF_NONE		= 0, // #NO_BIT
    OFF			= 0x0001, // joint is not functional and turned off
    FEEDBACK		= 0x0002, // collect feedback information about the joint
    USE_ODE_PARAMS	= 0x0004, // use special ODE parameters for this joint (else uses world settings)
  };

  enum JointType { 	// type of joint: Important -- must be sync'd with joint types in ode/common.h!!!
    NO_JOINT = 0,	// no joint type set
    BALL,		// ball joint -- no constraints on relative orientation
    HINGE,		// hinged -- only bends in one axis
    SLIDER,		// slider -- moves 
    UNIVERSAL = 5, 	// hinged in 2 axes -- also like a ball joint but transmits torque
    HINGE2,	     	// two hinges -- first one is like a suspension with a soft constraint, and second one can be used for a rotating wheel or similar
    FIXED,		// fixed -- use of this is discouraged (just redefine body shape), but it can be quite useful
  };

  // note this seems to be missing in 0.7:
  //    PR = dJointTypePR,		// rotoide & prismatic

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*		joint_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the joint (cast to a dJointID which is dxjoint*)
  JointFlags	flags;		// joint flags
  VEBodyRef	body1;		// #SCOPE_VEWorld first body in the joint
  VEBodyRef	body2;		// #SCOPE_VEWorld second body in the joint
  JointType    	joint_type;	// type of joint
  taVector3f	anchor;  	// anchor location for joint, specified RELATIVE TO BODY1 (note this is different from ODE -- we just add body1's position to this anchor position)
  taVector3f	axis;  		// #CONDSHOW_OFF_joint_type:BALL axis orientation vector
  taVector3f	axis2;  	// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 second axis for universal joint and hinge2 -- for universal, the first axis should be 1,0,0 and second 0,1,0 if those are the two axes being used -- otherwise a dRFrom2Axes zero length vector error will occur!
  float		vis_size;	// visual size of the joint, for when show_joint is active in viewer -- has NO implication for function whatsoever -- this is typically the length of the axis rod
  VEJointStops	stops;		// stop parameters for first joint: where the joint will stop (specific meaning is joint-dependent)
  VEJointStops	stops2;		// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 stop parameters for second joint: where the joint will stop (specific meaning is joint-dependent)
  VEJointMotor	motor;		// motor parameters that drive a powered joint
  VEJointMotor	motor2;		// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 motor parameters for second joint that drive a powered joint
  ODEJointParams ode_params;	// #CONDSHOW_ON_flags:USE_ODE_PARAMS ode integration parameters specifically for joints -- only used when USE_ODE_PARAMS is set
  ODEIntParams  suspension;	// #CONDSHOW_ON_joint_type:HINGE2 ode integration parameters for the hinge2 joint

  ////////////////////////////////////////////////////
  // feedback information -- only if feedback flag is set
  float		pos;		// #READ_ONLY #SHOW probed position value (joint dependent; could be angle)
  float		pos_norm; 	// #READ_ONLY #SHOW normalized position value (joint dependent; could be angle) -- if stops.stops_on, then 0 = lo stop, 1 = hi stop
  float		vel;		// #READ_ONLY #SHOW probed velocity value (joint dependent; could be angle)
  float		pos2;		// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW probed position value for 2nd axis (joint dependent; could be angle)
  float		pos2_norm; 	// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW normalized position value for 2nd axis (joint dependent; could be angle) -- if stops.stops_on, then 0 = lo stop, 1 = hi stop
  float		vel2;		// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW probed velocity value for 2nd axis (joint dependent; could be angle)

  taVector3f	cur_force1;  	// #READ_ONLY #SHOW force that joint applies to body 1
  taVector3f	cur_torque1;  	// #READ_ONLY #SHOW torque that joint applies to body 1
  taVector3f	cur_force2;  	// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW force that joint applies to body 2
  taVector3f	cur_torque2;  	// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW torque that joint applies to body 2

  override String	GetDesc() const { return desc; }
  override int		GetEnabled() const {  return !HasJointFlag(OFF); }
  override void		SetEnabled(bool value) { SetJointFlagState(OFF, !value); }

  inline void		SetJointFlag(JointFlags flg)   { flags = (JointFlags)(flags | flg); }
  // set joint flag state on
  inline void		ClearJointFlag(JointFlags flg) { flags = (JointFlags)(flags & ~flg); }
  // clear joint flag state (set off)
  inline bool		HasJointFlag(JointFlags flg) const { return (flags & flg); }
  // check if joint flag is set
  inline void		SetJointFlagState(JointFlags flg, bool on)
  { if(on) SetJointFlag(flg); else ClearJointFlag(flg); }
  // set joint flag state according to on bool (if true, set flag, if false, clear it)

  inline bool		HasTwoAxes()
  { return (joint_type == UNIVERSAL || joint_type == HINGE2); }
  // determine if joint has two axes of movement (else 1)

  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value

  virtual bool	CreateODE();	// #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy object in ode (if created)

  virtual void	Init();		
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void	CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing

  virtual void	ApplyForce(float force1, float force2 = 0.0f);
  // #BUTTON #CAT_Force apply force(s) (or torque(s) as the case may be) to the joint (only good for next time step)
  virtual void	ApplyMotor(float vel1, float f_max1, float vel2 = 0.0f, float f_max2 = 0.0f);
  // #BUTTON #CAT_Force apply motor target velocity and max force parameters to joint (persist until further changes) -- set f_max = 0 to turn off -- automatically turns ON motor_on and OFF servo_on (servo otherwise takes control of motor parameters)
  virtual void	ApplyServo(float trg_pos1, float trg_pos2 = 0.0f);
  // #BUTTON #CAT_Force set servo_on and update target positions for the servos -- servo control automatically applied when the system is stepped
  virtual void	ApplyServoNorm(float trg_norm_pos1, float trg_norm_pos2 = 0.0f, float stop_buffer=0.02f);
  // #BUTTON #CAT_Force set servo_on and update target positions for the servos using *normalized* values relative to lo-hi joint stops, with stop_buffer bounds to prevent numerical errors, so it doesn't go lower than lo + stop_buffer and higher than hi - stop_buffer -- servo control automatically applied when the system is stepped

  virtual void	Init_Anchor(); // #CAT_ODE set ODE anchor(s) and axes to the anchor and axisX values
  virtual void	Init_Stops(); // #CAT_ODE set stop(s) (including suspension for hinge 2)
  virtual void	Init_Motor(); // #CAT_ODE set motor params
  virtual void	Init_ODEParams(); // #CAT_ODE set ode integration parameters (erp, cfm)

  bool	IsCurType()  { return (joint_type == cur_type); }
  // #CAT_ODE is the ODE guy actually configured for the current joint type or not?

  SIMPLE_COPY_UPDT_PTR_PAR(VEJoint, VEWorld);
  SIMPLE_INITLINKS(VEJoint);
  override void CutLinks();
  TA_BASEFUNS(VEJoint);
protected:
  JointType		cur_type;	// current type that was previously set

#ifndef __MAKETA__
  dJointFeedback	ode_fdbk_obj;	// #IGNORE ode feedback object
#endif
protected:
  void 	UpdateAfterEdit_impl();

  //  override CheckConfig_impl() // todo
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEJoint,TA_VEJoint); // VEJointRef

class TA_API VEJoint_Group : public taGroup<VEJoint> {
  // ##CAT_VirtEnv a group of virtual environment joints
INHERITED(taGroup<VEJoint>)
public:
  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void	CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing
  virtual void	DestroyODE();
  // #CAT_ODE destroy ODE objs for these items

  TA_BASEFUNS_NOCOPY(VEJoint_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEJoint); }
  void 	Destroy()		{ };
};


////////////////////////////////////////////////
//	Special VE stuff for robotic arm sims

class TA_API VELambdaMuscle : public taNBase {
  // a Lambda-model (Gribble et al, 1998) muscle, used in context of a VE arm joint -- as a fairly accurate simplification, we assume a linear relationship between joint angle and muscle length, and a constant moment arm (accurate for extensors)
INHERITED(taNBase)
public:
  enum MuscleType {
    FLEXOR,			// pulls a joint closed -- toward hi stop -- pectoralis for shoulder, biceps long head for elbow
    EXTENSOR,			// pulls a joint open -- toward lo stop -- deltoid for shoulder, triceps lateral head
  };

  //////////////////////////////
  //	Control signals

  float		lambda_norm;	// normalized (0-1) desired length of the muscle: this is the only control signal
  float		lambda;		// #READ_ONLY #SHOW desired length of the muscle in muscle-length units
  float		co_contract_pct; // normalized (0-1) percent of co-contraction to apply -- shortens lambda by a fixed proportion of the co_contract_len value (below)
  float		extra_force;	// a constant additional force value to apply to the muscle -- can be used for co-contraction or additional force commands beyond the equilibrium point specification

  //////////////////////////////
  //	Parameters

  // statics
  MuscleType	muscle_type;		// what type of muscle is it -- controls relationship between angle and muscle length
  float		moment_arm;		// (m, .02 for elbow, .04 for shoulder) moment arm length for applying force (assumed fixed) -- positive for flexors and negative for extensors
  MinMaxRange	len_range;		// (m) effective length range of the muscle over which it can contract and expand -- this corresponds to the lo-hi stop range of angles of the joint -- for flexors, min = hi stop, max = lo stop, for extensors, min = lo stop, max = hi stop.  for elbow bicep/tricep, min=0.28 max=0.37, for shoulder pectoralis/deltoid min=0.05 max=0.15 (est)
  float		co_contract_len;	// maximum length available for co-contraction -- must be < .95 * len_range.min (i.e., if muscle is at its shortest length for the joint stop, this is how much extra shorter it can possibly command to be from there, and still be a positive number)
  float		rest_len;		// #READ_ONLY #SHOW resting length, computed during init from resting angle

  // dynamics
  float		step_size;		// #READ_ONLY (s) set in init -- world step size in seconds
  float		vel_damp;		// #DEF_0.06 (mu, s) velocity damping
  float		reflex_delay;		// #DEF_0.025 (d, s) reflex delay -- how slowly reflex control reacts to changes in muscle length and velocity
  int		reflex_delay_idx;	// #READ_ONLY (d, steps) reflex delay, computed in units of step size
  float		m_rec_grad;		// #DEF_11.2 (c, mm^-1) muscle MN recruitment gradient
  float		m_mag;			// (rho, m^2) muscle force-generating magnitude (multiplier), related to cross-section of muscle size: biceps short head 2.1; biceps long head 11; deltoid 14.9; pectoralis 14.9; triceps lateral head 12.1; triceps long head 6.7;
  float		ca_dt;			// #DEF_0.015 (tau, s) calcium kinetics time constant -- note only using a first-order exponential time decay filter
  float		ca_dt_cmp;		// #READ_ONLY (tau, 1/steps) calcium kinetics time constant actually used in cmputations -- note only using a first-order exponential time decay filter -- this value is in time steps, not time per se
  float		fv1;			// #DEF_0.82 (f1, s/m) muscle force velocity dependence factor: constant offset
  float		fv2;			// #DEF_0.5 (f2, s/m) muscle force velocity dependence factor: atan multiplier
  float		fv3;			// #DEF_0.43 (f3, s/m) muscle force velocity dependence factor: atan constant offset
  float		fv4;			// #DEF_0.58 (f4, s/m) muscle force velocity dependence factor: velocity multiplier
  float		passive_k;		// (k, N/m) passive stiffness: biceps short head 36.5; biceps long head 190.9; deltoid 258.5; pectoralis 258.5; triceps lateral head 209.9; triceps long head 116.3;

  //////////////////////////////
  //	State values
  float		len;		// #READ_ONLY #SHOW (l, cm) current muscle length
  float		dlen;		// #READ_ONLY #SHOW (l-dot, cm/s) current muscle length velocity: rate of change of length
  float		act;		// #READ_ONLY #SHOW (A, N?) current muscle activation value
  float		m_act_force;	// #READ_ONLY #SHOW (~M, N) current muscle force from activation
  float		m_force;	// #READ_ONLY #SHOW (M, N) current muscle force after low-pass filtering by ca_dt
  float		force;		// #READ_ONLY #SHOW (N) final force value 
  float		torque;		// #READ_ONLY #SHOW (N) final torque (force * moment_arm)

  float_CircBuffer len_buf;	// #READ_ONLY #NO_SAVE current muscle length buffer (for delays)
  float_CircBuffer dlen_buf;	// #READ_ONLY #NO_SAVE current muscle length velocity buffer (for delays)

  VEBodyRef	muscle_obj;	// #SCOPE_VEWorld if non-null, update this object with the new length information as the muscle changes (must be cylinder or capsule obj shape)

  virtual float	LenFmAngle(float norm_angle);
  // #CAT_Muscle compute muscle length from *normalized* joint angle (0 = lo stop, 1 = hi stop) -- uses a simple linear projection onto len_range which is fairly accurate

  virtual void	Init(float step_sz, float rest_norm_angle, float init_norm_angle,
		     float co_contract);
  // #CAT_Muscle initialize all parameters back to initial values, compute params, and set arm at initial angle (clear buffers, etc)

  virtual void	Compute_Force(float cur_norm_angle);
  // #CAT_Muscle compute force based on current parameters with given normalized angle (0 = lo stop, 1 = hi stop) (given by ODE presumably)

  virtual void	Compute_Lambda();
  // #CAT_Muscle compute lambda value from lambda_norm and co_contract_pct

  virtual void	SetTargAngle(float targ_norm_angle, float co_contract_pct);
  // #BUTTON #CAT_Muscle set target *normalized* (0 = lo stop, 1 = hi stop) angle for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put

  TA_SIMPLE_BASEFUNS(VELambdaMuscle);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};

// note on lengths and masses for typical arm: upper = .34m, 2.1kg; lower = .46m, 1.65kg

class TA_API VEMuscleJoint : public VEJoint {
  // a Lambda-model (Gribble et al, 1998) arm joint -- updates forces at every time step, in GetVAlsFromODE, applies them using SetForce
INHERITED(VEJoint)
public:
  VELambdaMuscle	flexor; // #SHOW_TREE flexor muscle
  VELambdaMuscle	extensor; // #SHOW_TREE extensor muscle
  VELambdaMuscle	flexor2; // #SHOW_TREE flexor muscle for joint 2
  VELambdaMuscle	extensor2; // #SHOW_TREE extensor muscle for joint 2

  float			targ_norm_angle; // #READ_ONLY #SHOW current target normalized angle
  float			targ_angle; // #READ_ONLY #SHOW current target raw angle
  float			targ_norm_angle2; // #READ_ONLY #SHOW #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 current target normalized angle 2
  float			targ_angle2; // #READ_ONLY #SHOW #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 current target raw angle 2

  float			co_contract_pct; // #READ_ONLY #SHOW current co-contraction pct

  virtual void	SetTargAngle(float trg_angle, float co_contract, float trg_angle2 = 0.0f);
  // #BUTTON #CAT_Force set target angle for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put (at least around .2 is needed, with .5 being better, to prevent big oscillations)

  virtual void	SetTargNormAngle(float trg_norm_angle, float co_contract,
				 float trg_norm_angle2 = 0.0f);
  // #BUTTON #CAT_Force set normalized target angle (0 = lo stop, 1 = hi stop) for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put (at least around .2 is needed, with .5 being better, to prevent big oscillations)

  override void	Init();
  override void	CurFromODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VEMuscleJoint);
protected:
  void 	UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()	{ };
};


////////////////////////////////////////////////
//	Object: collection of bodies and joints

class TA_API VEObject : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_veobj a virtual environment object, which contains interconnected bodies and their joints, and represents a sub-space of objects
INHERITED(taNBase)
public:	
  enum SpaceType {
    SIMPLE_SPACE,		// simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,			// hash-coded space with multi-scale grids: efficient for larger number of items
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  VEBody_Group	bodies;
  VEJoint_Group	joints;

  void*		space_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  SpaceType	space_type;	// type of space to use
  MinMaxInt	hash_levels;	// #CONDSHOW_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  override String	GetDesc() const { return desc; }

  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value
  virtual void*	GetWorldSpaceID(); // #CAT_ODE get world space id value

  virtual bool	CreateODE();	// #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy object in ode (if created)

  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void	CurToODE();	
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call
  virtual void	CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing

  virtual void	CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init
  virtual void	Translate(float dx, float dy, float dz, bool init);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos 
  virtual void	Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot
  virtual void	SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of bodies to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void	CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  SIMPLE_COPY(VEObject);
  SIMPLE_INITLINKS(VEObject);
  override void CutLinks();
  TA_BASEFUNS(VEObject);
protected:
  SpaceType	cur_space_type;	// current type that was previously set
  //  override CheckConfig_impl() // todo
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEObject,TA_VEObject); // VEObjectRef


class TA_API VEObject_Group : public taGroup<VEObject> {
  // ##CAT_VirtEnv a group of virtual environment objects
INHERITED(taGroup<VEObject>)
public:
  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void	CurToODE();	
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call
  virtual void	CurFromODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing
  virtual void	DestroyODE();
  // #CAT_ODE destroy ODE objs for these items

  virtual void	CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init or SetValsToODE
  virtual void	Translate(float dx, float dy, float dz, bool init);
  // #BUTTON #DYN1 #CAT_ODE move body given distance (can select multiple and operate on all at once)  -- if init is true, then apply to init_pos, else to cur_pos 
  virtual void	Scale(float sx, float sy, float sz);
  // #BUTTON #DYN1 #CAT_ODE scale size of body --  (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- if init is true, then apply to init_rot, else to cur_rot -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z, bool init);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values -- if init is true, then apply to init_rot, else to cur_rot
  virtual void	SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of bodies to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos
  virtual void	CopyColorFrom(VEBody* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other body

  TA_BASEFUNS_NOCOPY(VEObject_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEObject); }
  void 	Destroy()		{ };
};

///////////////////////////////////////////////////////////////
//   Linear Muscle: exerts force proportional to its input.

class TA_API VELinearMuscle : public taNBase {
// A muscle that exerts force proportional to its input
  INHERITED(taNBase)
public:

  taVector3f IPprox;	// proximal/medial insertion point
  taVector3f IPdist; 	// distal/lateral insertion point
  taVector3f p3; 	// point of intersection with the bending line
  float gain;
	bool bend;	// true if the muscle is currently bending
	float OldLength1, OldLength2; // past lengths, used to obtain contraction speed with the 3 point method. UpdateIPs keeps them actualized. OldLength2 is the length 2 timesteps ago.

  taVector3f Contract(float stim);
  // Returns the force vector (pointing towards the proximal insertion point) resulting from a given stimulation of the muscle;

	float Length(); 	// Returns current length of muscle
	float Speed(); 	// Returns muscle's contraction speed one world stepsize ago

  TA_SIMPLE_BASEFUNS(VELinearMuscle);

private: 

  void Initialize();
  void Initialize(taVector3f prox, taVector3f dist, float MrG);
  void Initialize(taVector3f prox, taVector3f dist, float MrG, taVector3f pp3, bool bending);
  void Destroy();
	virtual VEArm* GetArm(); // Get pointer to VEArm containing muscle

};

SmartRef_Of(VELinearMuscle,TA_VELinearMuscle); // VELinearMuscleRef

////////////////////////////////////////////////////////////////
// Linear Muscle Group

class TA_API VELinMuscle_Group : public taGroup<VELinearMuscle> {
  INHERITED(taGroup<VELinearMuscle>)
public:

  TA_BASEFUNS_NOCOPY(VELinMuscle_Group);

private:
  void	Initialize() 		{ SetBaseType(&TA_VELinearMuscle); }
  void 	Destroy()		{ };
};

///////////////////////////////////////////////////////////////
//      Arm: bodies and joints representing an arm

class TA_API VEArm : public VEObject{
  // a virtual environment arm object, consisting of 3 bodies: humerus, ulna, hand, and 3 joints: shoulder (a ball joint), elbow (a 2Hinge joint), and wrist (a FIXED joint for now)-- all constructed via ConfigArm -- bodies and joints are accessed by index so the order must not be changed
INHERITED(VEObject)
public:
  enum ArmBodies {		// indices of bodies for arm
    HUMERUS,
    ULNA,
    HAND,
    N_ARM_BODIES,
  };
  enum ArmJoints {		// indices of joints for arm
    SHOULDER,
    ELBOW,
    WRIST,
    N_ARM_JOINTS,
  };
  enum ArmSide {		// which arm are we simulating here?
    RIGHT_ARM,
    LEFT_ARM,
  };

  ArmSide	arm_side;	// is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  VEBodyRef	torso;		// the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to

  float La;     // #READ_ONLY #SHOW the length of the humerus
  float Lf;     // #READ_ONLY #SHOW length of the forearm (ulna,hand radius,gaps) 
  float elbow_gap;  // #READ_ONLY #SHOW the distance between ulna and humerus
  float wrist_gap;  // #READ_ONLY #SHOW the distance between hand and ulna
	float WorldStep;	// #READ_ONLY a copy of the owner VEWorld's stepsize, used for calculating speeds
  float_Matrix ShouldIP; // shoulder insertion points at rest
  float_Matrix ArmIP; // humerus insertion points at rest
  float_Matrix FarmIP; // ulna insertion points at rest 
  float_Matrix p1; // first end points for bending lines
  float_Matrix p2; // second end points for bending lines
  int Nmusc; // the total number of muscles, as implied by the IP matrices
  
  VELinMuscle_Group muscles;  // all the muscles attached to the arm

  virtual bool	CheckArm(bool quiet = false);
  // check to see if the arm is all configured OK -- it flags an error if not unless quiet -- returns true if OK, false if not

  virtual bool	ConfigArm(const String& name_prefix="", 
			  float humerus_length = 0.3, float humerus_radius = 0.04,
			  float ulna_length = 0.24, float ulna_radius = 0.03,
			  float hand_length = 0.08, float hand_radius = 0.03);
  // #BUTTON configure the arm bodies and joints, using the given length parameters and other options -- will update the lengths if the arm has already been created before -- returns success

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z);
  // #BUTTON place the hand at the specified target. This method can crash if the arm hasn't been set to its initial position. Returns true if a move is made (even if the target is not reachable).

  virtual bool TargetLengths(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z);
  // Obtain the muscle lengths which position the hand at the given coordinates, and place them in the given matrix, which should have a length equal to the number of muscles. Returns false if failed.

  virtual bool UpdateIPs();
  // #BUTTON Setting the muscle IPs to the values in the xxxIP matrices

	virtual bool Lengths(float_Matrix &Len); 
	// Put the current lengths of all muscles in the given matrix

	virtual bool Speeds(float_Matrix &Vel); 
	// Put the muscle contraction speeds of the last time step in the given matrix

  TA_SIMPLE_BASEFUNS(VEArm);
protected:
  //  override CheckConfig_impl(); // todo

  virtual bool bender(taVector3f &p3, taVector3f a, taVector3f c, taVector3f p1, taVector3f p2);
  // This function is the C++ equivalent of piece5.m. 
  // The points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line.
  // If the muscle wraps around the bending line, bender returns true, and inserts the value of
  // the point of intersection with the muscle in the vector p3.

private:
  void  Initialize();
  void  Destroy();
};


////////////////////////////////////////////////
//	Static bodies

class TA_API VEStatic : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vestc virtual environment static environment element -- not subject to physics and only interacts with bodies via collisions (cannot be part of a joint)
INHERITED(taNBase)
public:	
  enum StaticFlags { // #BITS flags for static elements
    SF_NONE		= 0, // #NO_BIT
    OFF 		= 0x0001, // turn this object off -- do not include in the virtual world
    FM_FILE		= 0x0002, // load object image features from Inventor (iv) object file
    EULER_ROT		= 0x0004, // use euler rotation angles instead of axis and angle
    CUR_FM_FILE		= 0x0020, // #NO_SHOW #READ_ONLY current flag setting load object image features from Inventor (iv) object file
  };

  enum Shape {			// shape of the object -- used for intertial mass and for collision detection (unless use_fname
    SPHERE,
    CAPSULE,			// a cylinder with half-spheres on each end -- preferred to standard cylinders for collision detection
    CYLINDER,
    BOX,
    PLANE,			// flat plane -- useful for ground surfaces
    NO_SHAPE,			// no shape at all -- only for special classes light lights
  };
  enum LongAxis {
    LONG_X=1,			// long axis is in X direction
    LONG_Y,			// long axis is in Y direction
    LONG_Z,			// long axis is in Z direction
  };
  enum NormAxis {
    NORM_X=1,			// normal axis is in X direction
    NORM_Y,			// normal axis is in Y direction
    NORM_Z,			// normal axis is in Z direction
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*		geom_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry associated with the static item (cast to a dGeomID which is dxgeom*)
  StaticFlags	flags;		// flags for various env el properties
  taVector3f	pos;  		// #CONDSHOW_OFF_shape:PLANE position of static item
  taAxisAngle	rot;  		// #CONDSHOW_OFF_flags:EULER_ROT rotation of static item in terms of axis and angle (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- not applicable to PLANE shape
  taVector3f	rot_euler;  	// #CONDSHOW_ON_flags:EULER_ROT rotation of static item (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) -- not applicable to PLANE shape
  taQuaternion  rot_quat;	// #READ_ONLY quaternion representation of the rotation -- automatically converted from rot or rot_euler depending on EULER_ROT flag

  Shape		shape;		// shape of static item for purposes of collision (and visual rendering if not FM_FILE)
  float		radius;		// #CONDSHOW_OFF_shape:BOX,PLANE radius of body, for all but box
  float		length;		// #CONDSHOW_OFF_shape:BOX,PLANE,SPHERE length of body, for all but box 
  LongAxis	long_axis;	// #CONDSHOW_OFF_shape:BOX,PLANE,SPHERE direction of the long axis of the body (where length is oriented)
  taVector3f	box;		// #CONDSHOW_ON_shape:BOX length of box in each axis for BOX-shaped body
  NormAxis	plane_norm;	// #CONDSHOW_ON_shape:PLANE direction of the plane normal axis (which way is "up" for a ground plane)
  float		plane_height;	// #CONDSHOW_ON_shape:PLANE height of the plane above/below 0 in the plane norm axis
  taVector2f    plane_vis_size; // #CONDSHOW_ON_shape:PLANE extent of the plane to actually render in the display (displayed as a very thin box of this size, centered at 0,0,0) -- actual plane in physical system is of infinite extent!

  taTransform   obj_xform;	// #CONDSHOW_ON_flags:FM_FILE full transform to apply to object file to align/size/etc with static item
  String	obj_fname;	// #CONDSHOW_ON_flags:FM_FILE #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Inventor file that describes static item appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  bool		set_color;	// if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor	color; 		// #CONDSHOW_ON_set_color default color of body if not otherwise defined ('a' alpha value used for transparency -- 1 = fully opaque (transparency = 0), 0 = fully transparent (transparency = 1)) -- this is the diffuse color, which reflects light sources in a diffuse way -- turn on full_colors for other options
  bool		full_colors; 	// #CONDSHOW_ON_set_color if true, then specify the full set of colors that can be specified for an object (ambient, specular, emissive, and shininess) -- main color is the diffuse color, which is basic color that reflects from light sources
  taColor	ambient_color; 	// #CONDSHOW_ON_set_color&&full_colors the ambient color is not affected by any light sources, and gives some background color -- typically dark grey
  taColor	specular_color;	// #CONDSHOW_ON_set_color&&full_colors the specular color is the color of the shiny reflections from an object -- the alpha value of this color determines the shininess parameter for the object, which controls how much of this color is generated
  taColor	emissive_color;	// #CONDSHOW_ON_set_color&&full_colors this is light generated by the object as though it were glowing -- is independent of light sources
  VETextureRef	texture;	// #SCOPE_VEWorld texture mapping of an image to the object (textures are shared resources defined in VEWorld)
  VESurface	surface;	// surface properties of the static item
  ODEIntParams	softness;	// set the cfm and erp values higher here to make the surface softer

  //////////////////////////////
  //	Internal-ish stuff

  override int		GetEnabled() const {  return !HasStaticFlag(OFF); }
  override void		SetEnabled(bool value) { SetStaticFlagState(OFF, !value); }
  override String	GetDesc() const { return desc; }
  inline void		SetStaticFlag(StaticFlags flg)   { flags = (StaticFlags)(flags | flg); }
  // set body flag state on
  inline void		ClearStaticFlag(StaticFlags flg) { flags = (StaticFlags)(flags & ~flg); }
  // clear body flag state (set off)
  inline bool		HasStaticFlag(StaticFlags flg) const { return (flags & flg); }
  // check if body flag is set
  inline void		SetStaticFlagState(StaticFlags flg, bool on)
  { if(on) SetStaticFlag(flg); else ClearStaticFlag(flg); }
  // set body flag state according to on bool (if true, set flag, if false, clear it)

  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value
  virtual VESpace* GetSpace();  // #CAT_ODE get parent space (parent of this guy)
  virtual void*	GetSpaceID(); 	// #CAT_ODE get space id value from space

  virtual bool	CreateODE();	// #CAT_ODE create static element in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy static element in ode (if created)

  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void	InitRotFromCur();
  // #IGNORE set init rotation parameters from current rotation (rot_quat)

  virtual void	Init_Shape();	// #CAT_ODE set shape information
  virtual void	Init_PosRot();	// #CAT_ODE set position and rotation

  bool	IsCurShape()  { return ((shape == cur_shape) && 
				(HasStaticFlag(FM_FILE) == HasStaticFlag(CUR_FM_FILE))); }
  // #CAT_ODE is the ODE guy actually configured for the current shape or not?

  virtual void	Translate(float dx, float dy, float dz);
  // #BUTTON #DYN1 #CAT_ODE move object given distance (can select multiple and operate on all at once)
  virtual void	Scale(float sx, float sy=0.0f, float sz=0.0f);
  // #BUTTON #DYN1 #CAT_ODE scale size of object -- if sy or sz is 0, then sx is used for that dimension (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values
  virtual void	SnapPosToGrid(float grid_size=0.05f);
  // #BUTTON #DYN1 #CAT_ODE snap the position of static body to grid of given size
  virtual void	CopyColorFrom(VEStatic* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other object

  SIMPLE_COPY(VEStatic);
  SIMPLE_INITLINKS(VEStatic);
  override void CutLinks();
  TA_BASEFUNS(VEStatic);
protected:
  Shape		cur_shape;	// current shape that was previously set

  override void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEStatic,TA_VEStatic); // VEStaticRef

class TA_API VEStatic_Group : public taGroup<VEStatic> {
  // ##CAT_VirtEnv a group of virtual environment static elements
INHERITED(taGroup<VEStatic>)
public:
  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void	DestroyODE();	// #CAT_ODE destroy ODE objs for these items

  virtual void	Translate(float dx, float dy, float dz);
  // #BUTTON #DYN1 #CAT_ODE move object given distance (can select multiple and operate on all at once)
  virtual void	Scale(float sx, float sy=0.0f, float sz=0.0f);
  // #BUTTON #DYN1 #CAT_ODE scale size of object -- if sy or sz is 0, then sx is used for that dimension (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values 
  virtual void	SnapPosToGrid(float grid_size=0.05f);
  // #BUTTON #DYN1 #CAT_ODE snap the position of static bodies to grid of given size
  virtual void	CopyColorFrom(VEStatic* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other object


  TA_BASEFUNS_NOCOPY(VEStatic_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEStatic); }
  void 	Destroy()		{ };
};

class TA_API VEHeightField : public VEStatic {
  // virtual environment height field -- 3d surface defined by a grid of height values
INHERITED(VEStatic)
public:

#ifdef __MAKETA__
  Shape		shape;		// #READ_ONLY #HIDDEN shape is always height field
  taVector3f	box;		// #READ_ONLY #HIDDEN not relevant
#endif

  // todo: lookup column in data table..

  DataTableRef	height_data;	// data table containing height field data
  String	data_col;	// column name within table that has the data -- IMPORTANT: must be a 2d float/double matrix column!
  int		row_num;	// row number containing height field data

  override void	Init();	

  TA_SIMPLE_BASEFUNS(VEHeightField);
private:
  void 	Initialize();
  void  Destroy() { };
};

////////////////////////////////////////////////
//	Space: collection of static elements

class TA_API VESpace : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_veobj a virtual environment that represents a sub-space of objects, containing static elements only -- helps optimize the collision detection process to group proximal items into sub spaces
INHERITED(taNBase)
public:	
  enum SpaceType {
    SIMPLE_SPACE,		// simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,			// hash-coded space with multi-scale grids: efficient for larger number of items
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  VEStatic_Group static_els;	// static elements of the space

  void*		space_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  SpaceType	space_type;	// type of space to use
  MinMaxInt	hash_levels;	// #CONDSHOW_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  override String	GetDesc() const { return desc; }
  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value
  virtual void*	GetWorldSpaceID(); // #CAT_ODE get world space id value

  virtual bool	CreateODE();	// #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy object in ode (if created)

  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done

  virtual void	Translate(float dx, float dy, float dz);
  // #BUTTON #DYN1 #CAT_ODE move object given distance (can select multiple and operate on all at once)
  virtual void	Scale(float sx, float sy=0.0f, float sz=0.0f);
  // #BUTTON #DYN1 #CAT_ODE scale size of object -- if sy or sz is 0, then sx is used for that dimension (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values
  virtual void	SnapPosToGrid(float grid_size=0.05f);
  // #BUTTON #DYN1 #CAT_ODE snap the position of static bodies to grid of given size
  virtual void	CopyColorFrom(VEStatic* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other object

  SIMPLE_COPY(VESpace);
  SIMPLE_INITLINKS(VESpace);
  override void CutLinks();
  TA_BASEFUNS(VESpace);
protected:
  SpaceType	cur_space_type;	// current type that was previously set
  //  override CheckConfig_impl() // todo
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VESpace,TA_VESpace); // VESpaceRef

class TA_API VESpace_Group : public taGroup<VESpace> {
  // ##CAT_VirtEnv a group of virtual environment objects
INHERITED(taGroup<VESpace>)
public:
  virtual void	Init();
  // #CAT_ODE #BUTTON re-initialize this object -- sets all the object current information to the init_ settings, and initializes the physics engine -- only works if the VEWorld has been initialized already
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void	DestroyODE();
  // #CAT_ODE destroy ODE objs for these items

  virtual void	Translate(float dx, float dy, float dz);
  // #BUTTON #DYN1 #CAT_ODE move object given distance (can select multiple and operate on all at once)
  virtual void	Scale(float sx, float sy=0.0f, float sz=0.0f);
  // #BUTTON #DYN1 #CAT_ODE scale size of object -- if sy or sz is 0, then sx is used for that dimension (can select multiple and operate on all at once)
  virtual void	RotateAxis(float x_ax, float y_ax, float z_ax, float rot);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation around given axis to current rotation values -- IMPORTANT: axis values cannot all be 0 -- it will automatically normalize though
  virtual void	RotateEuler(float euler_x, float euler_y, float euler_z);
  // #CAT_ODE #BUTTON #DYN1 apply (multiply) rotation using given Euler angles to current rotation values
  virtual void	SnapPosToGrid(float grid_size=0.05f);
  // #BUTTON #DYN1 #CAT_ODE snap the position of static bodies to grid of given size
  virtual void	CopyColorFrom(VEStatic* cpy_fm);
  // #BUTTON #DROP1 #DYN1 #CAT_ODE copy color (and texture) information from given other object

  TA_BASEFUNS_NOCOPY(VESpace_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VESpace); }
  void 	Destroy()		{ };
};

////////////////////////////////////////////////
//		World

class T3DataViewFrame;

class TA_API ODEWorldParams : public ODEIntParams {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
  INHERITED(ODEIntParams)
public:
  float 	max_cor_vel;	// #DEF_1e+06 maximum correcting velocity for contacts (how quickly they can pop things out of contact)
  float		contact_depth;	// #DEF_0.001 depth of the surface layer arond all geometry objects -- allows things to go this deep into a surface before actual contact is made -- increased value can help prevent jittering
  int		max_col_pts;	// #DEF_4 maximum number of collision points to get (must be less than 64, which is a hard maximum)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ODEWorldParams);
  TA_BASEFUNS(ODEWorldParams);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API VEWorld : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_vewld a virtual environment world
INHERITED(taNBase)
public:	
  enum	StepType {		// which type of stepping function to use
    STD_STEP,
    QUICK_STEP,
  };
  enum SpaceType {
    SIMPLE_SPACE,		// simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,			// hash-coded space with multi-scale grids: efficient for larger number of items
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*		world_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the world (cast to a dWorldID which is dxworld*)
  void*		space_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  void*		cgp_id;		// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the contact group (cast to a dJointGroupID
  SpaceType	space_type;	// type of space to use (typically HASH_SPACE is good for worlds having more objects)
  MinMaxInt	hash_levels;	// #CONDSHOW_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  StepType	step_type;	// what type of stepping function to use
  float		stepsize;	// how big of a step to take
  int		quick_iters;	// #CONDSHOW_ON_step_type:QUICK_STEP how many iterations to take in quick step mode
  taVector3f	gravity;	// gravitational setting for world (0,0,-9.81) is std
  bool		updt_display;	// if true, will update any attached display after each time step
  ODEWorldParams ode_params;	// parameters for tuning the ODE engine

  VEObject_Group objects;	// objects in the world
  VESpace_Group  spaces;	// spaces (static elements) in the world
  VETexture_List textures;	// shared textures used by bodies in the world

  VECameraRef	camera_0;	// first camera to use in rendering images (first person view) -- must be set to point to a camera in the set of objects for it to be used
  VECameraRef	camera_1;	// second camera to use in rendering images (for stereo vision)-- must be set to point to a camera in the set of objects for it to be used 
  taColor	bg_color; 	// background color of display for camera images
  VELightParams	sun_light;	// parameters for a sun (overhead directional) light 
  VELightRef	light_0;	// first light to add to scene -- must be set to point to a light in the set of objects for it to be used
  VELightRef	light_1;	// second light to add to scene -- must be set to point to a light in the set of objects for it to be used 

  override String	GetDesc() const { return desc; }
  virtual bool	CreateODE();	// #CAT_ODE create world in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy world in ode (if created)
  virtual void	SetValsToODE() { Init(); }
  // #CAT_Obsolete NOTE: Obsolete -- just use Init() -- set the initial values to ODE, and creates id's if not already done
  virtual void	CurFromODE();
  // #CAT_ODE get the current updated values from ODE after computing (called after each step)

  virtual void	Init();
  // #BUTTON #CAT_ODE initialize the virtual environment, placing all objects in their init configurations, updating with any added objects, etc
  virtual void	Step();		
  // #BUTTON #CAT_ODE take one step of integration, and get updated values
  virtual void	Reset() { DestroyODE(); SetValsToODE(); }
  // #BUTTON #CAT_ODE completely reset the ODE environment -- this is necessary if bad float numbers have been generated (nan, inf)

  virtual void	CurToODE();	
  // #CAT_ODE #BUTTON set the current values to ODE -- if you have updated these values external to the physics, then call this to update the physics engine so it is using the right thing -- only works after an Init call
  virtual void	CurToInit();
  // #BUTTON #CAT_ODE set the current position, rotation, etc values to the initial values that will be used for an Init or SetValsToODE -- for all bodies
  virtual void	SnapPosToGrid(float grid_size=0.05f, bool init_pos=true);
  // #BUTTON #DYN1 #CAT_ODE snap the position of all bodies and static objects to grid of given size -- operates on initial position if init_pos is set, otherwise on cur_pos

  VEWorldView*	NewView(T3DataViewFrame* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #BUTTON #CAT_Display make a new viewer of this world (NULL=use existing empty frame if any, else make new frame)
  VEWorldView*	FindView();
  // #CAT_Display find first existing viewer of this world (if it exists)
  virtual void	UpdateView();
  // #CAT_Display manually update the view information -- used for offscreen render updates

  virtual QImage GetCameraImage(int camera_no);
  // #CAT_ODE get camera image from given camera number (image may be null if camera not set)

  virtual bool GetCameraTaImage(taImage& ta_img, int camera_no);
  // #CAT_ODE gets camera image from given camera number into given taImage object -- returns false if unsuccessful

  //////////////////////////////////////
  // 	IMPL functions
#ifndef __MAKETA__
  virtual void	CollisionCallback(dGeomID o1, dGeomID o2);
  // #IGNORE callback function for the collision function, with two objects that are actual objects and not spaces
#endif

  void Copy_(const VEWorld& cp);
  SIMPLE_INITLINKS(VEWorld);
  override void CutLinks();
  TA_BASEFUNS(VEWorld);
protected:
  SpaceType	cur_space_type;	// current type that was previously set

private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEWorld,TA_VEWorld); // VEWorldRef


#endif // TA_VIRTENV_H
