#ifndef TEMPLATE_PL_H
#define TEMPLATE_PL_H

#include "template_def.h" // defines win macro, and includes ta_global and our own maketa types
#include "ta_plugin.h"    // Defines the plugin architecture.

// forward or external declarations
class TemplatePlugin;
class TemplatePluginState; //

#ifndef __MAKETA__
// The TemplatePlugin class allows this to be a plugin. It usually won't
// need to be modified. It is not seen by `maketa' because its base classes
// have not been scanned, and no programmatic access to this class is needed.

class TEMPLATE_API TemplatePlugin : public QObject, 
                           public IPlugin
{ // #NO_CSS #NO_MEMBERS
  Q_OBJECT

  // Tells Qt which interfaces are implemented by this class
  Q_INTERFACES(IPlugin)
public:
  static const taVersion	version;
  
  TemplatePlugin(QObject* par = NULL);

 public: // IPlugin interface
  TYPED_OBJECT(TemplatePlugin)
  const char*	desc() {return "Sample plugin provided with PDP++";}
  const char*	name() {return "TemplatePlugin";}
  const char*	uniqueId() {return "helloplugin.ccnlab.psych.colorado.edu";}
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass temt version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
  TypeDef*	GetPluginStateType();

};

// Associates a string with the interface TemplatePlugin
Q_DECLARE_INTERFACE(TemplatePlugin, "emergent.TemplatePlugin/1.0")
#endif // !__MAKETA__

   
#endif
