#ifndef HW_PL_H
#define HW_PL_H

#include "hw_def.h"
#include "ta_plugin.h"                       // Defines the plugin architecture.

class HelloPlugin; //

#ifndef __MAKETA__
// The HelloPlugin class allows this to be a plugin. It usually won't
// need to be modified. It is not seen by `maketa' because its base classes
// have not been scanned, and no programmatic access to this class is needed.

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

#endif
