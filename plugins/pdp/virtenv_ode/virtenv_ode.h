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

class VEObj;
class VEObj_Group;
class VEWorld;
class VEWorldView;

class VEODE_API VEObj : public taNBase {
  // a virtual environment object
INHERITED(taNBase)
public:	
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
  FloatTDCoord	pos;  		// position of object
  FloatRotation	orient;  	// orientation of object
  FloatTDCoord	lin_vel;	// linear velocity
  FloatTDCoord	ang_vel;	// angular velocity
  float		mass;		// total mass of object (in kg)
  MassShape	mass_shape;	// shape of object for purposes of mass/inertia
  float		mass_radius;	// #CONDEDIT_OFF_mass_shape:BOX radius of mass, for all but box
  float		mass_length;	// #CONDEDIT_OFF_mass_shape:BOX,SPHERE length of mass, for all but box 
  LongAxis	mass_long_axis;	// #CONDEDIT_OFF_mass_shape:BOX,SPHERE direction of the long axis of the object (where mass_length is oriented)
  FloatTDCoord	mass_box;	// #CONDEDIT_ON_mass_shape:BOX length of box in each axis for BOX-shaped mass

  FloatTransform obj_xform;	// full transform to apply to object file to align/size/etc with object
  bool		use_fname;	// if true, use obj_fname field (if non-empty) to load object from file (else shows the basic mass_shape)
  String	obj_fname;	// #CONDEDIT_ON_use_fname file name of Inventor file that describes object appearance (if empty, mass_shape will be rendered)
  taColor	color; 		// default color of object if not otherwise defined (a used for transparency)

  dMass		mass_ode;	// #IGNORE full ode mass of object

  virtual VEWorld* GetWorld();	// get world object (parent of this guy)
  virtual void*	GetWorldID();	// get world id value

  virtual bool	CreateODE();	// create object in ode (if not already created) -- returns false if unable to create
  virtual void	DestroyODE();	// destroy object in ode (if created)
  virtual void	SetValsToODE();	// set the current values to ODE (creates id's if not already done)
  virtual void	SetMassToODE();	// set the mass of object in ODE

  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  virtual void	ResetPosVel();	// #BUTTON reset position and velocity

  SIMPLE_COPY(VEObj);
  SIMPLE_INITLINKS(VEObj);
  override void CutLinks();
  TA_BASEFUNS(VEObj);
private:
  void 	Initialize();
  void  Destroy();
};

SmartRef_Of(VEObj,TA_VEObj); // VEObjRef

class TA_API VEObj_Group : public taGroup<VEObj> {
  // ##CAT_Data a group of virtual environment objects
INHERITED(taGroup<VEObj>)
public:
  virtual void	SetValsToODE();	// set the current values to ODE
  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEObj_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEObj); }
  void 	Destroy()		{ };
};

class T3DataViewFrame;

class VEODE_API VEWorld : public taNBase {
  // a virtual environment world
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

  VEObj_Group	objects;	// objects in the world

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
//		So Inventor Objects


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


class VEODE_API T3VEObj : public T3NodeLeaf {
  // object for virtual environment 
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEObj);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEObj(void* obj = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3VEObj();
};

///////////////////////////////////////////////////////////////////////
//		T3 DataView Guys

#include "t3viewer.h"

class VEObjView;
class VEWorldView;
class VEWorldViewPanel;

class VEODE_API VEObjView: public T3DataView {
  // ##SCOPE_VEWorldView view of one object
INHERITED(T3DataView)
friend class VEWorldView;
public:
  String	name;		// name of object this one is associated with

  VEObj*		Obj() const { return (VEObj*)data();}
  virtual void		SetObj(VEObj* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEObjView& cp);
  TA_BASEFUNS(VEObjView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class VEODE_API VEWorldView : public T3DataViewPar {
  // #VIRT_BASE #NO_TOKENS base class of grid and graph views
INHERITED(T3DataViewPar)
friend class VEWorldViewPanel;
public:
  static VEWorldView* New(VEWorld* wl, T3DataViewFrame*& fr);

  bool		display_on;  	// #DEF_true 'true' if display should be updated

  virtual const String	caption() const; // what to show in viewer

  VEWorld*		World() const {return (VEWorld*)data();}
  virtual void		SetWorld(VEWorld* wl);

  VEObjView*		ObjView(int i) const
  { return (VEObjView*)children.SafeEl(i); } 
  inline int		ObjViewCount() const { return children.size;}

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
