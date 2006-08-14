#include "hw.h"
#include <QtPlugin>

HelloPlugin::HelloPlugin(QObject*){}

int HelloPlugin::InitializeTypes() const {
  ta_Init_hw();  // call the maketa-generated type initialization routine
  return 0;
}

int HelloPlugin::InitializePlugin() {
  return 0;
}

Q_EXPORT_PLUGIN2(hw, HelloPlugin)

void HelloBase::Hello() {
  taMisc::Error("Hello World!");
}

