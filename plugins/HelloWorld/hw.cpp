#include "hw.h"
#include "hw_TA_inst.h"

HelloPlugin::HelloPlugin(QObject*){}

void HelloPlugin::Hello() {
  taMisc::Error("Hello World!");
}

int HelloPlugin::InitializeTypes() const {
  ta_Init_hw();  // call the maketa-generated type initialization routine
  return 0;
}

int HelloPlugin::InitializePlugin() {
  return 0;
}

#ifndef __MAKETA__
Q_EXPORT_PLUGIN2(HelloPlugin)
#endif
