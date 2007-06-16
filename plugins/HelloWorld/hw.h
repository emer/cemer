#ifndef HW_H
#define HW_H

#undef QT_SHARED                             // Already defined in config.h.
#include "hw_def.h"
#include "ta_plugin.h"                       // Defines the plugin architecture.
#include "ta_base.h"                         // Definition of the taBase object.
#include "minmax.h"
#include "ta_geometry.h"
#include "hw_TA_type.h"                      // Record of type information for HelloBase class.
                                             // This file is generated by running the `maketa'.
                                             // type scanning tool on this header file.
class HelloPlugin; //

#ifndef __MAKETA__
// The HelloPlugin class allows this to be a plugin. It usually won't
// need to be majorly modified. It is not seen by `maketa' as it
// cannot grok many Qt c++ constructs.

class HW_API HelloPlugin : public QObject, 
                           public IPlugin
{ // #NO_CSS #NO_MEMBERS
  Q_OBJECT

  // Tells Qt which interfaces are implemented by this class
  Q_INTERFACES(IPlugin)
public:
  static const taVersion	version;
  
  HelloPlugin(QObject* par = NULL);

 public: // IPlugin interface
  TYPED_OBJECT(HelloPlugin)
  const char*	desc() {return "Sample plugin provided with PDP++";}
  const char*	name() {return "HelloPlugin";}
  const char*	uniqueId() {return "helloplugin.ccnlab.psych.colorado.edu";}
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass ta/css version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
};

// Associates a string with the interface HelloPlugin
Q_DECLARE_INTERFACE(HelloPlugin, "pdp.HelloPlugin/1.0")
#endif // !__MAKETA__

// The actual content of the plugin follows. Inheriting from taNBase
// (where the N stands for Named) interfaces you with the TypeAccess
// system

class HW_API HelloBase : public taNBase
{
  INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references

public:
  FixedMinMax		min_max; // example of complex types as fields
  FloatTDCoord		a; // 1st input parameter to AddCoords
  FloatTDCoord		b; // 1st input parameter to AddCoords
  FloatTDCoord		c; // 1st input parameter to AddCoords
  
  // Comment directives such as the one below are explained in section
  // 18.3.3 of the PDP++ User Guide. This particular directive creates
  // a menu for the function Hello() in an edit dialog
  void Hello(); // #MENU Hello, World! Function
  void AddCoords(); // #MENU adds a + b and puts result in c


  SIMPLE_LINKS(HelloBase) // automatically links embedded taBase objects into ownership chain
  TA_BASEFUNS(HelloBase) /* Defines a default constructor that calls
    Initialize(), Register() and SetDefaultName() in that order.*/

private:
  SIMPLE_COPY(HelloBase) // enables this object to be copied
  void	Initialize() {}
  void	Destroy() {}
};

#endif
