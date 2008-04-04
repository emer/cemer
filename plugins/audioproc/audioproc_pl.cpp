#include "audioproc_pl.h"
#include <QtPlugin>

const taVersion AudioProcPlugin::version(1,0,0,0);

AudioProcPlugin::AudioProcPlugin(QObject*){}

int AudioProcPlugin::InitializeTypes() {
  ta_Init_audioproc();  // call the maketa-generated type initialization routine
  return 0;
}

int AudioProcPlugin::InitializePlugin() {
  return 0;
}

const char* AudioProcPlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(AudioProcPlugin, AudioProcPlugin)
