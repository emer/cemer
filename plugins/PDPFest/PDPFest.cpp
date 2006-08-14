#include "PDPFest.h"
#include <QtPlugin>

PDPFestPlugin::PDPFestPlugin(QObject*){}

int PDPFestPlugin::InitializeTypes() const {
  ta_Init_PDPFest();  // call the maketa-generated type initialization routine
  return 0;
}

int PDPFestPlugin::InitializePlugin() {
  return 0;
}

Q_EXPORT_PLUGIN2(PDPFest, PDPFestPlugin)

void PDPFestBase::Hello() {
  taMisc::Error("Hello, Festival!");
}

