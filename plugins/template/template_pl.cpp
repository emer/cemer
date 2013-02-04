#include "template_pl.h"

#include <QtPlugin>
#include <taMisc>

//TODO: you can update your Plugin's version here
const taVersion TemplatePlugin::version(
  PLUGIN_VERSION_MAJOR,
  PLUGIN_VERSION_MINOR,
  PLUGIN_VERSION_PATCH,
  0);

TemplatePlugin::TemplatePlugin(QObject*){}

int TemplatePlugin::InitializeTypes() {
  taMisc::Init_Types(); // loads all the type information for this plugin
  return 0;
}

int TemplatePlugin::InitializePlugin() {
// NOTE: if you have a TemplatePluginState obj, it has been created
//  and the previous saved state has been restored at this point

// TODO: you can put code in here that does any of these things:
// * creates global wizards in taMisc::root.wizards
// * creates global objects in taMisc::root.objs
// * creates any internal global objects your plugin uses
  return 0;
}

const char* TemplatePlugin::desc() {
  return "@EMERGENT_PLUGIN_DESC@";
}

const char* TemplatePlugin::uniqueId() {
  return "@EMERGENT_PLUGIN_UNIQUEID@";
}

const char* TemplatePlugin::url() {
  return "@EMERGENT_PLUGIN_URL@";
}
 
Q_EXPORT_PLUGIN2(template, TemplatePlugin)
