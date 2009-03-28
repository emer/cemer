#include "PyEmergent_pl.h"

#include <QtPlugin>

//TODO: you can update your Plugin's version here
const taVersion PyemergentPlugin::version(
  PLUGIN_VERSION_MAJOR,
  PLUGIN_VERSION_MINOR,
  PLUGIN_VERSION_PATCH,
  0);

PyemergentPlugin::PyemergentPlugin(QObject*){}

int PyemergentPlugin::InitializeTypes() {
  ta_Init_PyEmergent();  // call the maketa-generated type initialization routine
  return 0;
}

int PyemergentPlugin::InitializePlugin() {
// NOTE: if you have a PyemergentPluginState obj, it has been created
//  and the previous saved state has been restored at this point

// TODO: you can put code in here that does any of these things:
// * creates global wizards in taMisc::root.wizards
// * creates global objects in taMisc::root.objs
// * creates any internal global objects your plugin uses
  return 0;
}

const char* PyemergentPlugin::desc() {
  return "Python + Emergent";
}

const char* PyemergentPlugin::uniqueId() {
  return "PyEmergent";
}

const char* PyemergentPlugin::url() {
  return "Python + Emergent";
}
 
Q_EXPORT_PLUGIN2(PyEmergent, PyemergentPlugin)
