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


#include <ode/ode.h>

/////////////////////////////////////////////////////////////////////////////////

class VEODE_API VEObj : public taNBase {
  // a virtual environment object
INHERITED(taNBase)
public:	
  void*		body_id;	// id of the body (cast to a dBodyID which is dxbody*)
  FloatTDCoord	pos;  		// position of object
  FloatRotation	orient;  	// orientation of object
  FloatTDCoord	lin_vel;	// linear velocity
  FloatTDCoord	ang_vel;	// angular velocity
  float		mass;		// mass of object
  dMass		ode_mass;	// full ode mass of object

  FloatTransform obj_xform;	// full transform to apply to object file to align/size/etc with object
  String	obj_fname;	// file name of Inventor file that describes object appearance

  virtual void	GetValsFmODE();	// get the updated values from ODE after computing

  TA_SIMPLE_BASEFUNS(VEObj);
private:
  void 	Initialize();
  void  Destroy() { CutLinks(); }
};

class TA_API VEObj_Group : public taGroup<VEObj> {
  // ##CAT_Data a group of virtual environment objects
INHERITED(taGroup<VEObj>)
public:
  virtual void		GetValsFmODE();	// get the updated values from ODE after computing

  TA_BASEFUNS_NOCOPY(VEObj_Group);
private:
  void	Initialize() 		{ SetBaseType(&TA_VEObj); }
  void 	Destroy()		{ };
};

class VEODE_API VEWorld : public taNBase {
  // a virtual environment world
INHERITED(taNBase)
public:	
  void*		world_id;	// id of the world (cast to a dWorldID which is dxworld*)
  FloatTDCoord	gravity;	// gravitational setting for world (0,0,-9.81) is std
  float		stepsize;	// how big of a step to take

  VEObj_Group	objects;	// objects in the world

  virtual void	Step();		
  // #BUTTON take one step of integration

  TA_SIMPLE_BASEFUNS(VEWorld);
private:
  void 	Initialize();
  void  Destroy() { CutLinks(); }
};


#endif // VIRTENV_ODE_H
