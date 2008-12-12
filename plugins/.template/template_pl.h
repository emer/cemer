#ifndef TEMPLATE_PL_H
#define TEMPLATE_PL_H

#include "template_def.h" // defines win macro, and includes ta_global and our own maketa types
#include "ta_plugin.h"    // Defines the plugin architecture.

class TemplatePlugin; //

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
};

// Associates a string with the interface TemplatePlugin
Q_DECLARE_INTERFACE(TemplatePlugin, "emergent.TemplatePlugin/1.0")
#endif // !__MAKETA__

/* TODO: add user options or internal state to the following class -- examples of
   how to do each are below -- add these to the public section -- you can add your
   own internal values to a protected section, but those won't get saved
   
   String	user_option1; // this text here will explain to the user what "user_option1" does
   int		user_status1; // #READ_ONLY #NO_SAVE this item will display for user as read-only, and won't get saved
   int		internal_state1; // #HIDDEN the user won't see this, but it will still be saved/loaded
*/
   
class TEMPLATE_API TemplatePluginState : taNBase {
// this class is used to hold and save/restore user options and internal state for the TemplatePlugin 
  INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references
public:
  
  SIMPLE_LINKS(TemplatePluginState) // automatically links embedded taBase objects into ownership chain
  TA_BASEFUNS(TemplatePluginState) // Defines a default constructor that calls

private:
  SIMPLE_COPY(TemplatePluginState) // enables this object to be copied automatically
  void	Initialize(); // called in constructor for member initialization
  void	Destroy(); // called in destructor for release of any allocated resources
};

#endif
