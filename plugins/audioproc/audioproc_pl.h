#ifndef AUDIOPROC_PL_H
#define AUDIOPROC_PL_H

#include "ta_plugin.h"                       // Defines the plugin architecture.

#include "audioproc_def.h"

class AudioProcPlugin; //

#ifndef __MAKETA__
// The AudioProcPlugin class allows this to be a plugin. It usually won't
// need to be majorly modified. It is not seen by `maketa' as it
// cannot grok many Qt c++ constructs.

class AUDIOPROC_API AudioProcPlugin : public QObject, 
                           public IPlugin
{ // #NO_CSS #NO_MEMBERS
  Q_OBJECT

  // Tells Qt which interfaces are implemented by this class
  Q_INTERFACES(IPlugin)
public:
  static const taVersion	version;
  
  AudioProcPlugin(QObject* par = NULL);

 public: // IPlugin interface
  TYPED_OBJECT(AudioProcPlugin); 
  const char*	desc() {return "Audio processing for tacss";}
  const char*	name() {return "AudioProc";}
  const char*	uniqueId() {return "audioproc.plugins.brad-aisa.com";}
  const char*	url();
  
  int		NotifyTacssVersion(const taVersion& tav, bool& is_ok) {return 0;}
    // we pass ta/css version; set is_ok false if this version is no good for plugin
  int		GetVersion(taVersion& tav) {tav = version; return 0;}
  int 		InitializeTypes();
  int 		InitializePlugin();
};

// Associates a string with the interface AudioProcPlugin
Q_DECLARE_INTERFACE(AudioProcPlugin, "tacss.audioproc/1.0")
#endif // !__MAKETA__

#endif
