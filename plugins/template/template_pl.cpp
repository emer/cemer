#include "template_pl.h"

#include <QtPlugin>

const taVersion TemplatePlugin::version(1,0,0,0);

TemplatePlugin::TemplatePlugin(QObject*){}

int TemplatePlugin::InitializeTypes() {
  ta_Init_template();  // call the maketa-generated type initialization routine
  return 0;
}

int TemplatePlugin::InitializePlugin() {
  return 0;
}

const char* TemplatePlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

TypeDef* TemplatePlugin::GetPluginStateType() {
  return &TA_TemplatePluginState;
}

Q_EXPORT_PLUGIN2(hw, TemplatePlugin)
