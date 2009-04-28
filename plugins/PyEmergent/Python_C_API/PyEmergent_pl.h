#ifndef PYEMERGENT_PL_H
#define PYEMERGENT_PL_H

#include "PyEmergent_def.h" // defines win macro, and includes ta_global and our own maketa types
#include "ta_plugin.h"    // Defines the plugin architecture.

// forward or external declarations
class PyemergentPlugin;
class PyemergentPluginState; //

// you can replace these with your own version
// TODO: if you change these values, change them in CMakeLists.txt as well

#define PLUGIN_VERSION_MAJOR 0
#define PLUGIN_VERSION_MINOR 0
#define PLUGIN_VERSION_PATCH 0

#ifndef __MAKETA__
// The PyemergentPlugin class allows this to be a plugin. It usually won't
// need to be modified. It is not seen by `maketa' because its base classes
// have not been scanned, and no programmatic access to this class is needed.

class PYEMERGENT_API PyemergentPlugin : public QObject,
  public IPlugin
{ // #NO_CSS #NO_MEMBERS
  Q_OBJECT

  // Tells Qt which interfaces are implemented by this class
  Q_INTERFACES(IPlugin)
public:
  static const taVersion	version;
  
  PyemergentPlugin(QObject* par = NULL);

public: // IPlugin interface
  TYPED_OBJECT(PyemergentPlugin)
  const char*	desc();
  const char*	name() {return "Pyemergent";}
  const char*	uniqueId();
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass temt version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
};

// Associates a string with the interface PyemergentPlugin
Q_DECLARE_INTERFACE(PyemergentPlugin, "emergent.PyemergentPlugin/1.0")
#endif // !__MAKETA__

   
#endif
