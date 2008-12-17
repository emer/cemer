#include "template_pl.h"

#include <QtPlugin>

//NOTE: do not change the interface_version unless you manually update the 
// method signature in the header file with the new version
const taVersion TemplatePlugin::interface_version(2,0,0,0);

//TODO: you can update your Plugin's version here
const taVersion TemplatePlugin::plugin_version(
  PLUGIN_VERSION_MAJOR,
  PLUGIN_VERSION_MINOR,
  PLUGIN_VERSION_PATCH,
  0);

TemplatePlugin::TemplatePlugin(QObject*){}

int TemplatePlugin::InitializeTypes() {
  ta_Init_template();  // call the maketa-generated type initialization routine
  return 0;
}

int TemplatePlugin::InitializePlugin() {
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

// v1.1 interface methods
TypeDef* TemplatePlugin::GetPluginStateType() {
  return &TA_TemplatePluginState;
}

TypeDef* TemplatePlugin::GetPluginWizardType(int idx) {
//TODO: if you declare any wizards, return the type for the index
// 0..N-1 of each Wizard, then return NULL
  return NULL;
}
 
Q_EXPORT_PLUGIN2(template, TemplatePlugin)
