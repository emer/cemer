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

FloatTDCoord ftdc_plus(const FloatTDCoord& a, const FloatTDCoord& b) {
  FloatTDCoord rv; 
  rv.x = a.x + b.x; rv.y = a.y + b.y;  rv.z = a.z + b.z;
  return rv;
}

void HelloBase::AddCoords() {
  FloatTDCoord t(a);
 // t = a;
  t = ftdc_plus(a, b); // demonstrates use of temporary objects
  t = a + b; // demonstrates use of temporary objects
  c = t;
  
  UpdateAfterEdit(); // updates any associate members, and refreshes display
}
