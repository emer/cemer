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

class VEBody;
class VEBody_Group;
class VEObject;
class VEObject_Group;
class VEWorld;
class VEWorldView;

////////////////////////////////////////////////
//		objects (bodies)

class VEODE_API VEBody : public taNBase {
  // ##CAT_VirtEnv ##SCOPE_VEObject virtual environment body (rigid structural element)
INHERITED(taNBase)
public:	
  enum BodyFlags { // #BITS flags for bodies
    BF_NONE		= 0, // #NO_BIT
    FIXED 		= 0x0001, // body cannot move at all
    PLANE2D		= 0x0002, // body is constrained to the Z=0 plane
  };

  enum MassShape {		// shape of the object's mass (not detailed shape, just gross shape) -- not used for collision
    SPHERE,
    CAPPED_CYLINDER,
    CYLINDER,
    BOX,
  };
  enum LongAxis {
    LONG_X=1,			// long axis is in X direction
    LONG_Y,			// long axis is in Y direction
    LONG_Z,			// long axis is in Z direction
  };

  void*		body_id;	// #READ_ONLY #HIDDEN #NO_SAVE id of the body (cast to a dBodyID which is dxbody*)
  BodyFlags	flags;		// flags for various body properties
  FloatTDCoord	init_pos;  	// initial position of body (when creating it)
  FloatRotation	init_rot;  	// initial rotation of body (when creating it)
  FloatTDCoord	init_lin_vel;	// initial linear velocity
  FloatTDCoord	init_ang_vel;	// initial angular velocity

  FloatTDCoord	cur_pos;  	// current position of body
  FloatRotation	cur_rot;  	// current rotation of body
  FloatTDCoord	cur_lin_vel;	// current linear velocity
  FloatTDCoord	cur_ang_vel;	// current angular velocity

  float		mass;		// total mass of object (in kg)
  MassShape	mass_shape;	// shape of object for purposes of mass/inertia
  float		mass_radius;	// #CONDEDIT_OFF_mass_shape:BOX radius of mass, for all but box
  float		mass_length;	// #CONDEDIT_OFF_mass_shape:BOX,SPHERE length of mass, for all but box 
  LongAxis	mass_long_axis;	// #CONDEDIT_OFF_mass_shape:BOX,SPHERE direction of the long axis of the object (where mass_length is oriented)
  FloatTDCoord	mass_box;	// #CONDEDIT_ON_mass_shape:BOX length of box in each axis for BOX-shaped mass

  FloatTransform obj_xform;	// full transform to apply to object file to align/size/etc with object
  bool		use_fname;	// #APPLY_IMMED if true, use obj_fname field (if non-empty) to load object from file (else shows the basic mass_shape)
  String	obj_fname;	// #CONDEDIT_ON_use_fname file name of Inventor file that describes object appearance (if empty, mass_shape will be rendered)
  taColor	color; 		// default color of object if not otherwise defined (a used for transparency)

  //////////////////////////////
  //	Internal-ish stuff

  dMass		mass_ode;	// #IGNORE full ode mass of object
  void*		fixed_joint_id;	// #READ_ONLY #HIDDEN #NO_SAVE id of joint used to fix a FIXED object

  inline void		SetBodyFlag(BodyFlags flg)   { flags = (BodyFlags)(flags | flg); }
  // set body flag state on
  inline void		ClearBodyFlag(BodyFlags flg) { flags = (BodyFlags)(flags & ~flg); }
  // clear body flag state (set off)
  inline bool		HasBodyFlag(BodyFlags flg) const { return (flags & flg); }
  // check if body flag is set
  inline void		SetBodyFlagState(BodyFlags flg, bool on)
  { if(on) SetBodyFlag(flg); else ClearBodyFlag(flg); }
  // set body flag state according to on bool (if true, set flag, if false, clear it)


  virtual VEWorld* GetWorld();	// get world object (parent of this guy)
  virtual void*	GetWorldID();	// get world id value

  virtual bool	CreateODE();	// create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// destroy object in ode (if created)
  virtual void	SetValsToODE();	// set the current values to ODE (creates id's if not already done)
  virtual void	SetMassToODE();	// set the mass of object in ODE

  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  SIMPLE_COPY(VEBody);
  SIMPLE_INITLINKS(VEBody);
  override void CutLinks();
  TA_BASEFUNS(VEBody);
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEBody,TA_VEBody); // VEBodyRef

class TA_API VEBody_Group : public taGroup<VEBody> {
  // ##CAT_VirtEnv  ##SCOPE_VEObject a group of virtual environment bodies
INHERITED(taGroup<VEBody>)
public:
  virtual void	SetValsToODE();	// set the current values to ODE
  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEBody_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEBody); }
  void 	Destroy()		{ };
};

////////////////////////////////////////////////
//		Joints

class VEODE_API VEJoint : public taNBase {
  // ##CAT_VirtEnv  ##SCOPE_VEObject a virtual environment joint, which connects two bodies
INHERITED(taNBase)
public:	
  enum JointFlags { // #BITS flags for joints
    JF_NONE		= 0, // #NO_BIT
    FEEDBACK		= 0x0001, // collect feedback information about the joint
    USE_STOPS		= 0x0002, // set the lo and hi stop values and bounce
  };

  enum JointType { 	// type of joint: Important -- must be sync'd with joint types in ode/common.h!!!
    NO_JOINT = 0,	// no joint type set
    BALL,		// ball joint -- no constraints on relative orientation
    HINGE,		// hinged -- only bends in one axis
    SLIDER,		// slider -- moves 
    UNIVERSAL = 5, 	// like a ball joint but transmits torque
    HINGE2,	     	// a hinge and a 
    FIXED,		// fixed -- use of this is discouraged (just redefine body shape)
  };

  // note this seems to be missing in 0.7:
  //    PR = dJointTypePR,		// rotoide & prismatic

  void*		joint_id;	// #READ_ONLY #HIDDEN #NO_SAVE id of the joint (cast to a dJointID which is dxjoint*)
  JointFlags	flags;		// joint flags
  VEBodyRef	body1;		// first body in the joint
  VEBodyRef	body2;		// second body in the joint
  JointType    	joint_type;	// type of joint
  FloatTDCoord	anchor;  	// anchor location for joint, specified RELATIVE TO BODY1 (note this is different from ODE -- we just add body1's position to this anchor position)
  FloatTDCoord	axis;  		// #CONDEDIT_OFF_joint_type:BALL axis orientation vector
  FloatTDCoord	axis2;  	// #CONDSHOW_ON_joint_type:UNIVERSAL,HINGE2 second axis for universal joint
  float		lo_stop;	// stop for low angle or position value of joint
  float		hi_stop;	// stop for high angle or position value of joint
  float		stop_bounce;	// how bouncy is the joint (0 = no bounce, 1 = maximum bounce)

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

  virtual VEWorld* GetWorld();	// get world object (parent of this guy)
  virtual void*	GetWorldID();	// get world id value

  virtual bool	CreateODE();	// create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// destroy object in ode (if created)
  virtual void	SetValsToODE();	// set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  virtual void	ApplyForce(float force1, float force2 = 0.0f);
  // #BUTTON apply force(s) (or torque(s) as the case may be) to the joint (only good for next time step)

  SIMPLE_COPY(VEJoint);
  SIMPLE_INITLINKS(VEJoint);
  override void CutLinks();
  TA_BASEFUNS(VEJoint);
protected:
  JointType		set_type;	// type that was previously set
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
  // ##CAT_VirtEnv  ##SCOPE_VEObject a group of virtual environment joints
INHERITED(taGroup<VEJoint>)
public:
  virtual void	SetValsToODE();	// set the current values to ODE
  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEJoint_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEJoint); }
  void 	Destroy()		{ };
};

////////////////////////////////////////////////
//	Object: collection of bodies and joints

class VEODE_API VEObject : public taNBase {
  // ##CAT_VirtEnv ##SCOPE_VEWorld a virtual environment object, which contains interconnected bodies and their joints
INHERITED(taNBase)
public:	
  VEBody_Group	bodies;
  VEJoint_Group	joints;

  virtual VEWorld* GetWorld();	// get world object (parent of this guy)
  virtual void*	GetWorldID();	// get world id value

  virtual void	SetValsToODE();
  // set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE();
  // get the updated values from ODE after computing

  TA_SIMPLE_BASEFUNS(VEObject);
protected:
  //  override CheckConfig_impl() // todo
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEObject,TA_VEObject); // VEObjectRef

class TA_API VEObject_Group : public taGroup<VEObject> {
  // ##CAT_VirtEnv ##SCOPE_VEWorld a group of virtual environment objects
INHERITED(taGroup<VEObject>)
public:
  virtual void	SetValsToODE();	// set the current values to ODE
  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEObject_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEObject); }
  void 	Destroy()		{ };
};

////////////////////////////////////////////////
//		World

class T3DataViewFrame;

class VEODE_API VEWorld : public taNBase {
  // ##CAT_VirtEnv a virtual environment world
INHERITED(taNBase)
public:	
  enum	StepType {		// which type of stepping function to use
    STD_STEP,
    QUICK_STEP,
  };

  void*		world_id;	// #READ_ONLY #HIDDEN #NO_SAVE id of the world (cast to a dWorldID which is dxworld*)
  StepType	step_type;	// what type of stepping function to use
  float		stepsize;	// how big of a step to take
  int		quick_iters;	// #CONDEDIT_ON_step_type:QUICK_STEP how many iterations to take in quick step mode
  FloatTDCoord	gravity;	// gravitational setting for world (0,0,-9.81) is std

  VEObject_Group objects;	// objects in the world

  virtual bool	CreateODE();	// create world in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// destroy world in ode (if created)
  virtual void	SetValsToODE();
  // #BUTTON set the current values to ODE (creates id's if not already done)
  virtual void	GetValsFmODE();
  // get the updated values from ODE after computing (called after each step)

  virtual void	Step();		
  // #BUTTON take one step of integration, and get updated values

  VEWorldView*	NewView(T3DataViewFrame* fr = NULL);
  // #NULL_OK #NULL_TEXT_0_NewFrame #BUTTON #CAT_Display make a new viewer of this world (NULL=use existing empty frame if any, else make new frame)

  SIMPLE_COPY(VEWorld);
  SIMPLE_INITLINKS(VEWorld);
  override void CutLinks();
  TA_BASEFUNS(VEWorld);
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEWorld,TA_VEWorld); // VEWorldRef


/////////////////////////////////////////////////////////////////////////
//		So Inventor Code

#include "t3node_so.h"

class VEODE_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEWorld);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEWorld(void* world = NULL);

protected:
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
  // ##SCOPE_VEWorldView view of one body
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
  // ##SCOPE_VEWorldView view of one object
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

class VEODE_API VEWorldView : public T3DataViewPar {
  // a virtual environment world viewer
INHERITED(T3DataViewPar)
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
  T3_DATAVIEWFUNS(VEWorldView, T3DataViewPar) // 
protected:
#ifndef __MAKETA__
  QPointer<VEWorldViewPanel> m_wvp;
#endif
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
