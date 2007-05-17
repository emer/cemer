#include "virtenv_ode.h"
#include <QtPlugin>
#include <iostream>

const taVersion VEOdePlugin::version(1,0,0,0);

VEOdePlugin::VEOdePlugin(QObject*){}

int VEOdePlugin::InitializeTypes() {
  ta_Init_virtenv_ode();  // call the maketa-generated type initialization routine
  return 0;
}

int VEOdePlugin::InitializePlugin() {
  return 0;
}

const char* VEOdePlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(virtenv_ode, VEOdePlugin)

void VEObj::Initialize() {
  body_id = NULL;
  mass = 1.0f;
}

void VEObj::GetValsFmODE() {
  // todo: do this
}

void VEObj_Group::GetValsFmODE() {
  // todo: maybe take into account grouping??
  VEObj* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObj, ob, this->, i) {
    ob->GetValsFmODE();
  }
}

///////////////////////////////////////////////////////////////
//	World!

void VEWorld::Initialize() {
  world_id = NULL;
  gravity.z = -9.81f;
  stepsize = .01f;
}

void VEWorld::Step() {
  if(!world_id) return;
  dWorldStep((dWorldID)world_id, stepsize);
}
