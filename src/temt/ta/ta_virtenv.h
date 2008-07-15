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

class TA_API ODEIntParams : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
INHERITED(taBase)
public:
  float		erp;		// #DEF_0.2 (0-1, .1-.8 useful range) error reduction parameter: how much of the joint error is reduced at the next time step
  float		cfm;		// #DEF_1e-05 (0-1, 1e-9 - 1 useful range) constraint force mixing parameter: how "soft" is the constraint (0 = hard, 1 = soft)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ODEIntParams);
  TA_BASEFUNS(ODEIntParams);
protected:
  void	UpdateAfterEdit_impl();
};


////////////////////////////////////////////////
//		Bodies

class TA_API VESurface : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv surface properties for collisions
INHERITED(taBase)
public:

  float		friction; 	// (0-1e22) coulomb friction coefficient (mu). 0 = frictionless, 1e22 = infinity = no slipping
  float		bounce;		// (0-1) how bouncy is the surface (0 = hard, 1 = maximum bouncyness) 
  float		bounce_vel;	// minimum incoming velocity necessary for bounce -- incoming velocities below this will have a bounce parameter of 0

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(VESurface);
  TA_BASEFUNS(VESurface);
// protected:
//   void	UpdateAfterEdit_impl();
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
  FloatTwoDCoord offset;	// [0,0] offset of image from 0,0 (x=horizontal dim, y=vertical dim, shape dependent)
  FloatTwoDCoord scale;		// [1,1] scaling of image (x=horizontal dim, y=vertical dim, shape dependent)
  float 	rot;		// [0] rotation in the plane, in degrees
  FloatTwoDCoord center;	// [0,0] center point for scale and rotation
  taColor	blend_color;	// #CONDEDIT_ON_mode:BLEND color to use in blend case

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
  override int	GetIndex() { return idx; }
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
    PLANE2D		= 0x0004, // body is constrained to the Z=0 plane
    FM_FILE		= 0x0008, // load object image features from Inventor (iv) object file
  };
    //    COLLIDE_FM_FILE	= 0x0008, // use object shape from file for collision detection (NOTE: this is more computationally expensive and requires trimesh feature to be enabled in ode)

  enum Shape {			// shape of the object -- used for intertial mass and for collision detection (unless use_fname
    SPHERE,
    CAPSULE,			// a cylinder with half-spheres on each end -- preferred to standard cylinders for collision detection
    CYLINDER,
    BOX,
    NO_SHAPE,			// no shape at all -- only for special classes light lights
  };
  enum LongAxis {
    LONG_X=1,			// long axis is in X direction
    LONG_Y,			// long axis is in Y direction
    LONG_Z,			// long axis is in Z direction
  };

  String	desc;	   	// #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  void*		body_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the body (cast to a dBodyID which is dxbody*)
  void*		geom_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry associated with the body (cast to a dGeomID which is dxgeom*)
  BodyFlags	flags;		// #APPLY_IMMED flags for various body properties
  FloatTDCoord	init_pos;  	// initial position of body (when creating it)
  FloatRotation	init_rot;  	// initial rotation of body (when creating it) (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  FloatTDCoord	init_lin_vel;	// initial linear velocity
  FloatTDCoord	init_ang_vel;	// initial angular velocity

  FloatTDCoord	cur_pos;  	// current position of body
  FloatRotation	cur_rot;  	// current rotation of body (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  FloatTDCoord	cur_lin_vel;	// current linear velocity
  FloatTDCoord	cur_ang_vel;	// current angular velocity

  Shape		shape;		// #APPLY_IMMED shape of body for purposes of mass/inertia and collision (and visual rendering if not FM_FILE)
  float		mass;		// total mass of body (in kg)
  float		radius;		// #CONDEDIT_OFF_shape:BOX radius of body, for all but box
  float		length;		// #CONDEDIT_OFF_shape:BOX,SPHERE length of body, for all but box 
  LongAxis	long_axis;	// #CONDEDIT_OFF_shape:BOX,SPHERE direction of the long axis of the body (where length is oriented)
  FloatTDCoord	box;		// #CONDEDIT_ON_shape:BOX length of box in each axis for BOX-shaped body

  FloatTransform obj_xform;	// full transform to apply to body file to align/size/etc with body
  String	obj_fname;	// #CONDEDIT_ON_flags:FM_FILE #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Inventor file that describes body appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  bool		set_color;	// if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor	color; 		// #CONDEDIT_ON_set_color default color of body if not otherwise defined (a used for transparency)
  VETextureRef	texture;	// #SCOPE_VEWorld #NULL_OK texture mapping of an image to the object (textures are shared resources defined in VEWorld)
  VESurface	surface;	// physics surface properties of the body (softness, bounciness)
  ODEIntParams	softness;	// set the cfm and erp values higher here to make the surface softer

  //////////////////////////////
  //	Internal-ish stuff
#ifndef __MAKETA__
  dMass		mass_ode;	// #IGNORE full ode mass of body
#endif
  void*		fixed_joint_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of joint used to fix a FIXED body

  override int		GetEnabled() const {  return !HasBodyFlag(OFF); }
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
  virtual void	SetValsToODE();	// #CAT_ODE set the current values to ODE (creates id's if not already done)
  virtual void	SetMassToODE();	// #CAT_ODE set the mass of body in ODE

  virtual void	GetValsFmODE(bool updt_disp = false);	// #CAT_ODE get the updated values from ODE after computing

  SIMPLE_COPY(VEBody);
  SIMPLE_INITLINKS(VEBody);
  override void CutLinks();
  TA_BASEFUNS(VEBody);
protected:
  Shape		cur_shape;	// current shape that was previously set

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
  virtual void	SetValsToODE();	// set the current values to ODE
  virtual void	GetValsFmODE(bool updt_disp = false);	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEBody_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEBody); }
  void 	Destroy()		{ };
};


////////////////////////////////////////////////
//		Camera & Lights

class TA_API VELightParams : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env light parameters
INHERITED(taBase)
public:
  bool		on;		// is the light turned on?
  float		intensity;	// #CONDEDIT_ON_on (0-1) how bright is the light
  taColor	color; 		// #CONDEDIT_ON_on color of light

  void	Initialize();
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(VELightParams);
// protected:
//   void	UpdateAfterEdit_impl();
};

class TA_API VECameraDists : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env camera distances
INHERITED(taBase)
public:
  float		near;		// #DEF_0.1 near distance of camera -- closest things can be seen
  float		focal;		// focal distance of camera -- where is it focused on in scene?
  float		far;		// far distance of camera -- furthest things that can be seen

  void	Initialize();
  void	Destroy()	{ };
  TA_SIMPLE_BASEFUNS(VECameraDists);
// protected:
//   void	UpdateAfterEdit_impl();
};

class SoPerspectiveCamera; // #IGNORE

class TA_API VECamera : public VEBody {
  // virtual environment camera -- a body that contains a camera -- position and orientation are used to point the camera -- body shape is not rendered, but mass/inertia etc is used if part of a non-fixed object -- camera must be selected in the VEWorld for it to actually be used to render images!
INHERITED(VEBody)
public:

#ifdef __MAKETA__
  Shape		shape;		// #READ_ONLY #HIDDEN shape for camera must always be a cylinder
  LongAxis	long_axis;	// #READ_ONLY #HIDDEN direction of the long axis of the body (where length is oriented) -- must always be LONG_Z for a camera
  FloatTDCoord	box;		// #READ_ONLY #HIDDEN not relevant
#endif

  TwoDCoord	img_size;	// size of image to record from camera
  bool		color;		// if true, get full color images (else greyscale)
  VECameraDists	view_dist;	// distances that are in view of the camera
  float		field_of_view;	// field of view of camera (angle in degrees) -- how much of scene is it taking in
  int		antialias_scale; // #DEF_2 to achieve antialiasing, renders at a larger scale (determined by this parameter), and is then downscaled to target size
  VELightParams	light;		// directional "headlight" associated with the camera -- ensures that whatever is being viewed can be seen (but makes lighting artificially consistent and good)
  FloatTDCoord	dir_norm;	// #READ_ONLY #SHOW normal vector for where the camera is facing

  virtual void 		ConfigCamera(SoPerspectiveCamera* cam);
  // config So camera parameters

  override void	SetValsToODE();
  override void	GetValsFmODE(bool updt_disp = false);

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
  float		drop_off_rate;	// #CONDEDIT_ON_light_type:SPOT_LIGHT (0-1) how fast light drops off with increasing angle from the direction angle
  float		cut_off_angle;	// #CONDEDIT_ON_light_type:SPOT_LIGHT (45 default) angle in degrees from the direction vector where there will be no light
  FloatTDCoord	dir_norm;	// #READ_ONLY #SHOW normal vector for where the camera is facing

  virtual SoLight*	CreateLight();
  // create the So light of correct type
  virtual void 		ConfigLight(SoLight* lgt);
  // config So light parameters
  virtual bool		UpdateLight();
  // #BUTTON if environment is already initialized and viewed, this will update the light in the display based on current settings

  override void	SetValsToODE();
  override void	GetValsFmODE(bool updt_disp = false);

  TA_SIMPLE_BASEFUNS(VELight);
private:
  void 	Initialize();
  void  Destroy() { };
};

SmartRef_Of(VELight,TA_VELight); // VELightRef

////////////////////////////////////////////////
//		Joints

class TA_API VEJointStops : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint stop parameters
INHERITED(taBase)
public:
  float		lo;		// stop for low angle or position value of joint
  float		hi;		// stop for high angle or position value of joint
  float		bounce;		// how bouncy is the joint (0 = no bounce, 1 = maximum bounce)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(VEJointStops);
  TA_BASEFUNS(VEJointStops);
// protected:
//   void	UpdateAfterEdit_impl();
};

class TA_API VEJointMotor : public taBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv virtual env joint motor parameters
INHERITED(taBase)
public:
  float		vel;		// target joint velocity to achieve (angular or linear)
  float		f_max;		// maximum force or torque to drive the joint to achieve desired velocity

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(VEJointMotor);
  TA_BASEFUNS(VEJointMotor);
// protected:
//   void	UpdateAfterEdit_impl();
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
    FEEDBACK		= 0x0001, // collect feedback information about the joint
    USE_STOPS		= 0x0002, // set the lo and hi stop values and bounce 
    USE_MOTOR		= 0x0004, // set the joint motor velocity and maximum force parameters
    USE_ODE_PARAMS	= 0x0008, // use special ODE parameters for this joint (else uses world settings)
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
  JointFlags	flags;		// #APPLY_IMMED joint flags
  VEBodyRef	body1;		// #SCOPE_VEObject first body in the joint
  VEBodyRef	body2;		// #SCOPE_VEObject second body in the joint
  JointType    	joint_type;	// #APPLY_IMMED type of joint
  FloatTDCoord	anchor;  	// anchor location for joint, specified RELATIVE TO BODY1 (note this is different from ODE -- we just add body1's position to this anchor position)
  FloatTDCoord	axis;  		// #CONDEDIT_OFF_joint_type:BALL axis orientation vector
  FloatTDCoord	axis2;  	// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 second axis for universal joint and hinge2
  VEJointStops	stops;		// #CONDEDIT_ON_flags:USE_STOPS stop parameters for first joint: where the joint will stop (specific meaning is joint-dependent)
  VEJointStops	stops2;		// #CONDEDIT_ON_flags:USE_STOPS stop parameters for second joint: where the joint will stop (specific meaning is joint-dependent)
  VEJointMotor	motor;		// #CONDEDIT_ON_flags:USE_MOTOR motor parameters that drive a powered joint
  VEJointMotor	motor2;		// #CONDEDIT_ON_flags:USE_MOTOR motor parameters for second joint that drive a powered joint
  ODEJointParams ode_params;	// #CONDEDIT_ON_flags:USE_ODE_PARAMS ode integration parameters specifically for joints -- only used when USE_ODE_PARAMS is set
  ODEIntParams  suspension;	// #CONDEDIT_ON_joint_type:HINGE2 ode integration parameters for the hinge2 joint

  ////////////////////////////////////////////////////
  // feedback information -- only if feedback flag is set
  float		pos;		// #SHOW probed position value (joint dependent; could be angle)
  float		vel;		// #SHOW probed velocity value (joint dependent; could be angle)
  float		pos2;		// #CONDSHOW_ON_joint_type:UNIVERSAL #SHOW probed position value (joint dependent; could be angle)
  float		vel2;		// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #SHOW probed velocity value (joint dependent; could be angle)

  FloatTDCoord	cur_force1;  	// #READ_ONLY #SHOW force that joint applies to body 1
  FloatTDCoord	cur_torque1;  	// #READ_ONLY #SHOW torque that joint applies to body 1
  FloatTDCoord	cur_force2;  	// #READ_ONLY #SHOW force that joint applies to body 2
  FloatTDCoord	cur_torque2;  	// #READ_ONLY #SHOW torque that joint applies to body 2

  override String	GetDesc() const { return desc; }

  inline void		SetJointFlag(JointFlags flg)   { flags = (JointFlags)(flags | flg); }
  // set joint flag state on
  inline void		ClearJointFlag(JointFlags flg) { flags = (JointFlags)(flags & ~flg); }
  // clear joint flag state (set off)
  inline bool		HasJointFlag(JointFlags flg) const { return (flags & flg); }
  // check if joint flag is set
  inline void		SetJointFlagState(JointFlags flg, bool on)
  { if(on) SetJointFlag(flg); else ClearJointFlag(flg); }
  // set joint flag state according to on bool (if true, set flag, if false, clear it)

  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value

  virtual bool	CreateODE();	// #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy object in ode (if created)
  virtual void	SetValsToODE();	// #CAT_ODE set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE(bool updt_disp = false);	// #CAT_ODE get the updated values from ODE after computing

  virtual void	ApplyForce(float force1, float force2 = 0.0f);
  // #BUTTON #CAT_Force apply force(s) (or torque(s) as the case may be) to the joint (only good for next time step)

  SIMPLE_COPY(VEJoint);
  SIMPLE_INITLINKS(VEJoint);
  override void CutLinks();
  TA_BASEFUNS(VEJoint);
protected:
  JointType		cur_type;	// current type that was previously set
#ifndef __MAKETA__
  dJointFeedback	ode_fdbk_obj;	// #IGNORE ode feedback object
#endif

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
  virtual void	SetValsToODE();	// #CAT_ODE set the current values to ODE
  virtual void	GetValsFmODE(bool updt_disp = false);	// #CAT_ODE get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEJoint_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEJoint); }
  void 	Destroy()		{ };
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
  MinMaxInt	hash_levels;	// #CONDEDIT_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  override String	GetDesc() const { return desc; }

  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value
  virtual void*	GetWorldSpaceID(); // #CAT_ODE get world space id value

  virtual bool	CreateODE();	// #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy object in ode (if created)

  virtual void	SetValsToODE();
  // #CAT_ODE set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE(bool updt_disp = false);
  // #CAT_ODE get the updated values from ODE after computing

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
  virtual void	SetValsToODE();	// set the current values to ODE
  virtual void	GetValsFmODE(bool updt_disp = false);	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEObject_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEObject); }
  void 	Destroy()		{ };
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
  StaticFlags	flags;		// #APPLY_IMMED flags for various env el properties
  FloatTDCoord	pos;  		// #CONDEDIT_OFF_shape:PLANE position of static item
  FloatRotation	rot;  		// #CONDEDIT_OFF_shape:PLANE rotation of static item (rot is in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)

  Shape		shape;		// #APPLY_IMMED shape of static item for purposes of collision (and visual rendering if not FM_FILE)
  float		radius;		// #CONDEDIT_OFF_shape:BOX,PLANE radius of body, for all but box
  float		length;		// #CONDEDIT_OFF_shape:BOX,PLANE,SPHERE length of body, for all but box 
  LongAxis	long_axis;	// #CONDEDIT_OFF_shape:BOX,PLANE,SPHERE direction of the long axis of the body (where length is oriented)
  FloatTDCoord	box;		// #CONDEDIT_ON_shape:BOX length of box in each axis for BOX-shaped body
  NormAxis	plane_norm;	// #CONDEDIT_ON_shape:PLANE direction of the plane normal axis (which way is "up" for a ground plane)
  float		plane_height;	// #CONDEDIT_ON_shape:PLANE height of the plane above/below 0 in the plane norm axis
  FloatTwoDCoord plane_vis_size; // #CONDEDIT_ON_shape:PLANE extent of the plane to actually render in the display (displayed as a very thin box of this size, centered at 0,0,0) -- actual plane in physical system is of infinite extent!

  FloatTransform obj_xform;	// full transform to apply to object file to align/size/etc with static item
  String	obj_fname;	// #CONDEDIT_ON_flags:FM_FILE #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Inventor file that describes static item appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  bool		set_color;	// if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor	color; 		// #CONDEDIT_ON_set_color default color of static item if not otherwise defined (a used for transparency)
  VETextureRef	texture;	// #SCOPE_VEWorld texture mapping of an image to the object (textures are shared resources defined in VEWorld)
  VESurface	surface;	// surface properties of the static item
  ODEIntParams	softness;	// set the cfm and erp values higher here to make the surface softer

  //////////////////////////////
  //	Internal-ish stuff

  override int		GetEnabled() const {  return !HasStaticFlag(OFF); }
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
  virtual void	SetValsToODE();	// #CAT_ODE set the current values to ODE (creates id's if not already done)

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
  virtual void	SetValsToODE();	// set the current values to ODE

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
  FloatTDCoord	box;		// #READ_ONLY #HIDDEN not relevant
#endif

  // todo: lookup column in data table..

  DataTableRef	height_data;	// data table containing height field data
  String	data_col;	// column name within table that has the data -- IMPORTANT: must be a 2d float/double matrix column!
  int		row_num;	// row number containing height field data

  override void	SetValsToODE();	

  TA_SIMPLE_BASEFUNS(VEHeightField);
private:
  void 	Initialize();
  void  Destroy() { };
};

////////////////////////////////////////////////
//	Space: collection of static elements

class TA_API VESpace : public taNBase {
  // #STEM_BASE ##CAT_VirtEnv ##EXT_veobj a virtual environment object, which contains interconnected bodies and their joints, and represents a sub-space of objects
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
  MinMaxInt	hash_levels;	// #CONDEDIT_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  override String	GetDesc() const { return desc; }
  virtual VEWorld* GetWorld();	// #CAT_ODE get world object (parent of this guy)
  virtual void*	GetWorldID();	// #CAT_ODE get world id value
  virtual void*	GetWorldSpaceID(); // #CAT_ODE get world space id value

  virtual bool	CreateODE();	// #CAT_ODE create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy object in ode (if created)

  virtual void	SetValsToODE();
  // #CAT_ODE set the current values to ODE (creates id's if not already done)

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
  virtual void	SetValsToODE();	// set the current values to ODE

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
  MinMaxInt	hash_levels;	// #CONDEDIT_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  StepType	step_type;	// #APPLY_IMMED what type of stepping function to use
  float		stepsize;	// how big of a step to take
  int		quick_iters;	// #CONDEDIT_ON_step_type:QUICK_STEP how many iterations to take in quick step mode
  FloatTDCoord	gravity;	// gravitational setting for world (0,0,-9.81) is std
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
  virtual void	SetValsToODE();
  // #CAT_ODE set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE();
  // #CAT_ODE get the updated values from ODE after computing (called after each step)

  virtual void	Init() { SetValsToODE(); }
  // #BUTTON #CAT_ODE initialize the virtual environment, placing all objects in their init configurations, updating with any added objects, etc
  virtual void	Step();		
  // #BUTTON #CAT_ODE take one step of integration, and get updated values

  VEWorldView*	NewView(T3DataViewFrame* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #BUTTON #CAT_Display make a new viewer of this world (NULL=use existing empty frame if any, else make new frame)
  VEWorldView*	FindView();
  // #CAT_Display find first existing viewer of this world (if it exists)

  virtual QImage GetCameraImage(int camera_no);
  // #CAT_ODE get camera image from given camera number (image may be null if camera not set)

  //////////////////////////////////////
  // 	IMPL functions
#ifndef __MAKETA__
  virtual void	CollisionCallback(dGeomID o1, dGeomID o2);
  // #IGNORE callback function for the collision function, with two objects that are actual objects and not spaces
#endif

  SIMPLE_COPY(VEWorld);
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
