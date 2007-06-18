#include "hw.h"
#include <QtPlugin>

const taVersion HelloPlugin::version(1,0,0,0);

HelloPlugin::HelloPlugin(QObject*){}

int HelloPlugin::InitializeTypes() {
  ta_Init_hw();  // call the maketa-generated type initialization routine
  return 0;
}

int HelloPlugin::InitializePlugin() {
  return 0;
}

const char* HelloPlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(hw, HelloPlugin)

void HelloBase::Hello() {
  taMisc::Error("Hello World!");
}

void HelloBase::AddCoords() {
  FloatTDCoord t = a + b; // demonstrates use of temporary objects
  c = t;
  UpdateAfterEdit(); // updates any associate members, and refreshes display
}
