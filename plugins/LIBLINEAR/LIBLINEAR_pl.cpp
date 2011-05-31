#include "LIBLINEAR_pl.h"

#include <QtPlugin>

//TODO: you can update your Plugin's version here
const taVersion LIBLINEARPlugin::version(
  PLUGIN_VERSION_MAJOR,
  PLUGIN_VERSION_MINOR,
  PLUGIN_VERSION_PATCH,
  0);

LIBLINEARPlugin::LIBLINEARPlugin(QObject*){}

int LIBLINEARPlugin::InitializeTypes() {
  ta_Init_LIBLINEAR();  // call the maketa-generated type initialization routine
  return 0;
}

int LIBLINEARPlugin::InitializePlugin() {
// NOTE: if you have a LIBLINEARPluginState obj, it has been created
//  and the previous saved state has been restored at this point

// TODO: you can put code in here that does any of these things:
// * creates global wizards in taMisc::root.wizards
// * creates global objects in taMisc::root.objs
// * creates any internal global objects your plugin uses
  return 0;
}

const char* LIBLINEARPlugin::desc() {
  return "enter description of your plugin";
}

const char* LIBLINEARPlugin::uniqueId() {
  return "LIBLINEAR.foo.bar.org";
}

const char* LIBLINEARPlugin::url() {
  return "enter description of your plugin";
}
 
Q_EXPORT_PLUGIN2(LIBLINEAR, LIBLINEARPlugin)
