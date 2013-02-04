#ifndef TEMPLATE_PL_H
#define TEMPLATE_PL_H

#include "template_def.h" // defines win macro, and includes ta_global and our own maketa types
#include <IPlugin>    // Defines the plugin architecture.

// forward or external declarations
class TemplatePlugin;
class TemplatePluginState; //

TypeDef_Of(TemplatePlugin);
TypeDef_Of(TemplatePluginState);


// you can replace these with your own version
// TODO: if you change these values, change them in CMakeLists.txt as well

#define PLUGIN_VERSION_MAJOR @PLUGIN_VERSION_MAJOR@
#define PLUGIN_VERSION_MINOR @PLUGIN_VERSION_MINOR@
#define PLUGIN_VERSION_PATCH @PLUGIN_VERSION_PATCH@

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
  const char*	desc();
  const char*	name() {return "Template";}
  const char*	uniqueId();
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass temt version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
};

// Associates a string with the interface TemplatePlugin
Q_DECLARE_INTERFACE(TemplatePlugin, "emergent.TemplatePlugin/1.0")
#endif // !__MAKETA__

   
#endif
