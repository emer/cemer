#ifndef VIRTENV_ODE_H
#define VIRTENV_ODE_H

#undef QT_SHARED                             // Already defined in config.h.
#include "ta_plugin.h"                       // Defines the plugin architecture.
#include "ta_base.h"                         // Definition of the taBase object.
#include "ta_geometry.h"
#include "virtenv_ode_TA_type.h"

#define VEODE_API                      // Needed for windows compatability.

class VEOdePlugin;		// 

#ifndef __MAKETA__
// The VEOdePlugin class allows this to be a plugin. It usually won't
// need to be majorly modified. It is not seen by `maketa' as it
// cannot grok many Qt c++ constructs.

class VEODE_API VEOdePlugin : public QObject, public IPlugin {
  // #NO_CSS #NO_MEMBERS
  Q_OBJECT

  // Tells Qt which interfaces are implemented by this class
  Q_INTERFACES(IPlugin)
public:
  static const taVersion	version;
  
  VEOdePlugin(QObject* par = NULL);

 public: // IPlugin interface
  TYPED_OBJECT(VEOdePlugin) 
  const char*	desc() {return "Virtual Simulated Environment, using ODE physics engine";}
  const char*	name() {return "VEOdePlugin";}
  const char*	uniqueId() {return "virtenvplugin.ccnlab.psych.colorado.edu";}
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass ta/css version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
};

// Associates a string with the interface VEOdePlugin
Q_DECLARE_INTERFACE(VEOdePlugin, "pdp.VEOdePlugin/1.0")
#endif // !__MAKETA__


/////////////////////////////////////////////////////////////////////////////////
//		Actual ODE Code

#include <ode/ode.h>
#include "colorscale.h"		// taColor
#include "minmax.h"

class VEBody;
class VEBody_Group;
class VEJoint;
class VEJoint_Group;
class VEObject;
class VEObject_Group;
class VEWorld;
class VEWorldView;

class VEODE_API ODEIntParams : public taBase {
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
//		objects (bodies)

class VEODE_API VESurface : public taBase {
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


class VEODE_API VEBody : public taNBase {
  // ##CAT_VirtEnv ##EXT_vebod virtual environment body (rigid structural element)
INHERITED(taNBase)
public:	
  enum BodyFlags { // #BITS flags for bodies
    BF_NONE		= 0, // #NO_BIT
    FIXED 		= 0x0001, // body cannot move at all
    PLANE2D		= 0x0002, // body is constrained to the Z=0 plane
    FM_FILE		= 0x0004, // load object image features from Inventor (iv) object file
  };
    //    COLLIDE_FM_FILE	= 0x0008, // use object shape from file for collision detection (NOTE: this is more computationally expensive and requires trimesh feature to be enabled in ode)

  enum Shape {			// shape of the object -- used for intertial mass and for collision detection (unless use_fname
    SPHERE,
    CAPSULE,			// a cylinder with half-spheres on each end -- preferred to standard cylinders for collision detection
    CYLINDER,
    BOX,
  };
  enum LongAxis {
    LONG_X=1,			// long axis is in X direction
    LONG_Y,			// long axis is in Y direction
    LONG_Z,			// long axis is in Z direction
  };

  void*		body_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the body (cast to a dBodyID which is dxbody*)
  void*		geom_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry associated with the body (cast to a dGeomID which is dxgeom*)
  BodyFlags	flags;		// #APPLY_IMMED flags for various body properties
  FloatTDCoord	init_pos;  	// initial position of body (when creating it)
  FloatRotation	init_rot;  	// initial rotation of body (when creating it)
  FloatTDCoord	init_lin_vel;	// initial linear velocity
  FloatTDCoord	init_ang_vel;	// initial angular velocity

  FloatTDCoord	cur_pos;  	// current position of body
  FloatRotation	cur_rot;  	// current rotation of body
  FloatTDCoord	cur_lin_vel;	// current linear velocity
  FloatTDCoord	cur_ang_vel;	// current angular velocity

  Shape		shape;		// #APPLY_IMMED shape of body for purposes of mass/inertia and collision (and visual rendering if not FM_FILE)
  float		mass;		// total mass of body (in kg)
  float		radius;		// #CONDEDIT_OFF_shape:BOX radius of body, for all but box
  float		length;		// #CONDEDIT_OFF_shape:BOX,SPHERE length of body, for all but box 
  LongAxis	long_axis;	// #CONDEDIT_OFF_shape:BOX,SPHERE direction of the long axis of the body (where length is oriented)
  FloatTDCoord	box;		// #CONDEDIT_ON_shape:BOX length of box in each axis for BOX-shaped body

  FloatTransform obj_xform;	// full transform to apply to body file to align/size/etc with body
  String	obj_fname;	// #CONDEDIT_ON_flags:FM_FILE file name of Inventor file that describes body appearance (if empty or FM_FILE flag is not on, basic shape will be rendered)

  taColor	color; 		// default color of body if not otherwise defined (a used for transparency)
  VESurface	surface;	// surface properties of the body
  ODEIntParams	softness;	// set the cfm and erp values higher here to make the surface softer

  //////////////////////////////
  //	Internal-ish stuff

  dMass		mass_ode;	// #IGNORE full ode mass of body
  void*		fixed_joint_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of joint used to fix a FIXED body

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

class VEODE_API VECamera : public VEBody {
  // ##CAT_VirtEnv ##EXT_vebod virtual environment camera -- a body that contains a camera -- position and orientation are used to point the camera -- body shape is not rendered, but mass/inertia etc is used if part of a non-fixed object -- camera must be selected in the VEWorld for it to actually be used to render images!
INHERITED(VEBody)
public:
  TwoDCoord	img_size;	// size of image to record from camera
  bool		color;		// if true, get color information (else monochrome)
  float		focal_dist;	// focal distance of camera -- where is it focused on in scene?
  float		field_of_view;	// field of view of camera (angle in radians) -- how muc of scene is it taking in

  TA_SIMPLE_BASEFUNS(VECamera);
private:
  void 	Initialize();
  void  Destroy() { };
};

SmartRef_Of(VECamera,TA_VECamera); // VECameraRef

class VEODE_API VELight : public VEBody {
  // ##CAT_VirtEnv ##EXT_vebod virtual environment light -- a body that contains a light source -- body shape is not rendered, but mass/inertia etc is used if part of a non-fixed object -- light only affects items after it in the list of objects!
INHERITED(VEBody)
public:	
  enum LightType {
    DIRECTIONAL_LIGHT,		// shines in a given direction, rotation is used for the direction, but position is not -- fastest rendering speed
    POINT_LIGHT,		// radiates in all directions, uses position for location
    SPOT_LIGHT,			// shines in a given direction from a given position
  };

  LightType	light_type;	// type of light
  bool		light_on;	// is the light turned on?
  float		intensity;	// (0-1) how bright is the light
  float		drop_off_rate;	// #CONDEDIT_ON_light_type:SPOT_LIGHT (0-1) how fast light drops off with increasing angle from the direction angle
  float		cut_off_angle;	// #CONDEDIT_ON_light_type:SPOT_LIGHT (radians, PI/4 = 0.7853 default) angle in radians from the direction vector where there will be no light

  TA_SIMPLE_BASEFUNS(VELight);
private:
  void 	Initialize();
  void  Destroy() { };
};

SmartRef_Of(VELight,TA_VELight); // VELightRef

////////////////////////////////////////////////
//		Joints

class VEODE_API VEJointStops : public taBase {
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

class VEODE_API VEJointMotor : public taBase {
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

class VEODE_API ODEJointParams : public ODEIntParams {
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

class VEODE_API VEJoint : public taNBase {
  // ##CAT_VirtEnv ##EXT_vejnt a virtual environment joint, which connects two bodies
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
  float		pos;		// #READ_ONLY #SHOW probed position value (joint dependent; could be angle)
  float		vel;		// #READ_ONLY #SHOW probed velocity value (joint dependent; could be angle)
  float		pos2;		// #CONDSHOW_ON_joint_type:UNIVERSAL #READ_ONLY #SHOW probed position value (joint dependent; could be angle)
  float		vel2;		// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 #READ_ONLY #SHOW probed velocity value (joint dependent; could be angle)

  FloatTDCoord	cur_force1;  	// #READ_ONLY #SHOW force that joint applies to body 1
  FloatTDCoord	cur_torque1;  	// #READ_ONLY #SHOW torque that joint applies to body 1
  FloatTDCoord	cur_force2;  	// #READ_ONLY #SHOW force that joint applies to body 2
  FloatTDCoord	cur_torque2;  	// #READ_ONLY #SHOW torque that joint applies to body 2

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

class VEODE_API VEObject : public taNBase {
  // ##CAT_VirtEnv ##EXT_veobj a virtual environment object, which contains interconnected bodies and their joints, and represents a sub-space of objects
INHERITED(taNBase)
public:	
  enum SpaceType {
    SIMPLE_SPACE,		// simple list of items, requires O(n^2) but ok for very small spaces
    HASH_SPACE,			// hash-coded space with multi-scale grids: efficient for larger number of items
  };

  VEBody_Group	bodies;
  VEJoint_Group	joints;

  void*		space_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  SpaceType	space_type;	// type of space to use
  MinMaxInt	hash_levels;	// #CONDEDIT_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

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
//		World

class T3DataViewFrame;

class VEODE_API ODEWorldParams : public ODEIntParams {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_VirtEnv ODE integration parameters
  INHERITED(ODEIntParams)
public:
  float 	max_cor_vel;	// #DEF_1e06 maximum correcting velocity for contacts (how quickly they can pop things out of contact)
  float		contact_depth;	// #DEF_0.001 depth of the surface layer arond all geometry objects -- allows things to go this deep into a surface before actual contact is made -- increased value can help prevent jittering
  int		max_col_pts;	// #DEF_4 maximum number of collision points to get (must be less than 64, which is a hard maximum)

  void	Initialize();
  void	Destroy()	{ };
  SIMPLE_COPY(ODEWorldParams);
  TA_BASEFUNS(ODEWorldParams);
protected:
  void	UpdateAfterEdit_impl();
};

class VEODE_API VEWorld : public taNBase {
  // ##CAT_VirtEnv ##EXT_vewld a virtual environment world
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

  void*		world_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the world (cast to a dWorldID which is dxworld*)
  void*		space_id;	// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the geometry space (cast to a dSpaceID which is dxspace*)
  void*		cgp_id;		// #READ_ONLY #HIDDEN #NO_SAVE #NO_COPY id of the contact group (cast to a dJointGroupID
  SpaceType	space_type;	// type of space to use (typically HASH_SPACE is good for worlds having more objects)
  MinMaxInt	hash_levels;	// #CONDEDIT_ON_space_type:HASH_SPACE minimum and maximum spacing levels in hash space

  StepType	step_type;	// #APPLY_IMMED what type of stepping function to use
  float		stepsize;	// how big of a step to take
  int		quick_iters;	// #CONDEDIT_ON_step_type:QUICK_STEP how many iterations to take in quick step mode
  FloatTDCoord	gravity;	// gravitational setting for world (0,0,-9.81) is std
  bool		sun_on;		// turn on a sun (overhead directional) light 
  bool		updt_display;	// if true, will update any attached display after each time step
  ODEWorldParams ode_params;	// parameters for tuning the ODE engine

  VEObject_Group objects;	// objects in the world
  VECameraRef	camera_0;	// first camera to use in rendering images (first person view) -- must be set to point to a camera in the set of objects for it to be used
  VECameraRef	camera_1;	// second camera to use in rendering images (for stereo vision)-- must be set to point to a camera in the set of objects for it to be used 

  virtual bool	CreateODE();	// #CAT_ODE create world in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// #CAT_ODE destroy world in ode (if created)
  virtual void	SetValsToODE();
  // #BUTTON #CAT_ODE set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE();
  // #CAT_ODE get the updated values from ODE after computing (called after each step)

  virtual void	Step();		
  // #BUTTON #CAT_Run take one step of integration, and get updated values

  VEWorldView*	NewView(T3DataViewFrame* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #BUTTON #CAT_Display make a new viewer of this world (NULL=use existing empty frame if any, else make new frame)

  //////////////////////////////////////
  // 	IMPL functions

  virtual void	CollisionCallback(dGeomID o1, dGeomID o2);
  // #IGNORE callback function for the collision function, with two objects that are actual objects and not spaces

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


/////////////////////////////////////////////////////////////////////////
//		So Inventor Code

#include "t3node_so.h"

class SoOffscreenRenderer; // #IGNORE
class SoSwitch;			// #IGNORE
class SoDirectionalLight; // #IGNORE

class VEODE_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEWorld);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEWorld(void* world = NULL);

  void			setSunLightDir(float x_dir, float y_dir, float z_dir);
  void			setSunLightOn(bool on);
  SoDirectionalLight* 	getSunLight()	  { return sun_light; }
  SoSwitch*		getCameraSwitch() { return camera_switch; }

protected:
  SoDirectionalLight* 	sun_light;
  SoSwitch*		camera_switch; // switching between diff cameras
  
  ~T3VEWorld();
};

class VEODE_API T3VEObject : public T3NodeParent {
  // object parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEObject);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEObject(void* obj = NULL);

protected:
  ~T3VEObject();
};

class VEODE_API T3VEBody : public T3NodeLeaf {
  // body for virtual environment 
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEBody);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEBody(void* bod = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3VEBody();
};

///////////////////////////////////////////////////////////////////////
//		T3 DataView Code

#include "t3viewer.h"

class VEBodyView;
class VEWorldView;
class VEWorldViewPanel;

class VEODE_API VEBodyView: public T3DataView {
  // view of one body
INHERITED(T3DataView)
friend class VEWorldView;
public:
  String	name;		// name of body this one is associated with

  VEBody*		Body() const { return (VEBody*)data();}
  virtual void		SetBody(VEBody* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEBodyView& cp);
  TA_BASEFUNS(VEBodyView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class VEODE_API VEObjectView: public T3DataViewPar {
  // view of one object
INHERITED(T3DataView)
friend class VEWorldView;
public:
  String	name;		// name of body this one is associated with

  VEObject*		Object() const { return (VEObject*)data();}
  virtual void		SetObject(VEObject* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  override void		BuildAll();
  
  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEObjectView& cp);
  TA_BASEFUNS(VEObjectView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class VEODE_API VEWorldView : public T3DataViewMain {
  // a virtual environment world viewer
INHERITED(T3DataViewMain)
friend class VEWorldViewPanel;
public:
  static VEWorldView* New(VEWorld* wl, T3DataViewFrame*& fr);

  bool		display_on;  	// #DEF_true 'true' if display should be updated

  virtual const String	caption() const; // what to show in viewer

  VEWorld*		World() const {return (VEWorld*)data();}
  virtual void		SetWorld(VEWorld* wl);

  VEObjectView*		ObjectView(int i) const
  { return (VEObjectView*)children.SafeEl(i); } 
  inline int		ObjectViewCount() const { return children.size;}

  virtual void		InitDisplay(bool init_panel = true);
  // does a hard reset on the display, reinitializing variables etc.  Note does NOT do Updatedisplay -- that is a separate step
  virtual void		UpdateDisplay(bool update_panel = true);
  // full re-render of the display (generally calls Render_impl)

  virtual void		InitPanel();
  // lets panel init itself after struct changes
  virtual void		UpdatePanel();
  // after changes to props

  virtual QImage	RenderCamera(int cam_no);
  // get the output of the given camera number (currently 0 or 1)

  bool			isVisible() const; // gui_active, mapped and display_on

  override void		BuildAll();
  
  override String	GetLabel() const;
  override String	GetName() const;
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const VEWorldView& cp);
  T3_DATAVIEWFUNS(VEWorldView, T3DataViewMain) // 
protected:
#ifndef __MAKETA__
  QPointer<VEWorldViewPanel> m_wvp;
#endif
  SoOffscreenRenderer*	cam_renderer;

  override void 	UpdateAfterEdit_impl();

  override void		Render_pre();
  override void		Render_impl();

};

class VEODE_API VEWorldViewPanel: public iViewPanelFrame {
  // frame for gui interface to a VEWorldView -- usually posted by the worldview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:

  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layDispCheck;
  QLabel*		  labcam0;
  QLabel*		  labcam1;

  VEWorldView*		wv() {return (VEWorldView*)m_dv;} //

  virtual void		InitPanel();

  VEWorldViewPanel(VEWorldView* dv_);
  ~VEWorldViewPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;} //
  override TypeDef*	GetTypeDef() const {return &TA_VEWorldViewPanel;}

protected:
  int			updating; // to prevent recursion
  override void		GetImage_impl();

// public slots:
//   void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview

  //protected slots:
};


#endif // VIRTENV_ODE_H
