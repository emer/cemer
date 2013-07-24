// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "VEWorld.h"

#include <taMisc>

#include <ode/ode.h>
#include <ode/error.h>

VEWorldRef VEWorld::last_ve_stepped;
taBaseRef  VEWorld::last_to_set_ode;


// this is the default message guy from ODE
static void VE_Err_printMessage (int num, const char *msg1, const char *msg2,
                                 va_list ap)
{
  fflush (stderr);
  fflush (stdout);
  if (num) fprintf (stderr,"\n%s %d: ",msg1,num);
  else fprintf (stderr,"\n%s: ",msg1);
  vfprintf (stderr,msg2,ap);
  fprintf (stderr,"\n");
  fflush (stderr);
}

extern "C" void VE_ErrorHandler(int errnum, const char* msg, va_list ap) {
  String step_nm = "n/a";
  String set_vals_nm = "n/a";
  if(VEWorld::last_ve_stepped) {
    step_nm = VEWorld::last_ve_stepped->name;
  }
  if(VEWorld::last_to_set_ode) {
    set_vals_nm = VEWorld::last_to_set_ode->GetName();
  }

  VE_Err_printMessage(errnum, "ODE Error", msg, ap); // provide ap stuff

  taMisc::Error("ODE Fatal Error (details on console) number:", String(errnum), msg,
                "last VEWorld Step'd:", step_nm,
                "last VE item doing Init:", set_vals_nm,
                "DO NOT ATTEMPT TO RUN VIRTUAL ENV without restarting");
}

extern "C" void VE_DebugHandler(int errnum, const char* msg, va_list ap) {
  String step_nm = "n/a";
  String set_vals_nm = "n/a";
  if(VEWorld::last_ve_stepped) {
    step_nm = VEWorld::last_ve_stepped->name;
  }
  if(VEWorld::last_to_set_ode) {
    set_vals_nm = VEWorld::last_to_set_ode->GetName();
  }

  VE_Err_printMessage(errnum, "ODE INTERNAL ERROR", msg, ap); // provide ap stuff

  taMisc::Error("ODE Debug Error (details on console) number:", String(errnum), msg,
                "last VEWorld Step'd:", step_nm,
                "last VE item doing Init:", set_vals_nm,
                "DO NOT ATTEMPT TO RUN VIRTUAL ENV without restarting");
}

extern "C" void VE_MessageHandler(int errnum, const char* msg, va_list ap) {
  String step_nm = "n/a";
  String set_vals_nm = "n/a";
  if(VEWorld::last_ve_stepped) {
    step_nm = VEWorld::last_ve_stepped->name;
  }
  if(VEWorld::last_to_set_ode) {
    set_vals_nm = VEWorld::last_to_set_ode->GetName();
  }

  VE_Err_printMessage(errnum, "ODE Message", msg, ap); // provide ap stuff

  taMisc::Warning("ODE Message number:", String(errnum), msg,
                "last VEWorld Step'd:", step_nm,
                "last VE item doing Init:", set_vals_nm);
}

///////////////////////////////////////////////////////////////
//      World!

void ODEWorldParams::Initialize() {
  max_cor_vel = 1.0e6f;
  contact_depth = 0.001f;
  max_col_pts = 4;
}

void ODEWorldParams::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(max_col_pts > 64) max_col_pts = 64;
  if(max_col_pts < 0) max_col_pts = 1;
}

void VEWorld::Initialize() {
  world_id = NULL;
  space_id = NULL;
  cgp_id = NULL;
  space_type = HASH_SPACE;
  cur_space_type = HASH_SPACE;
  hash_levels.min = -3;  hash_levels.max = 10;
  gravity.y = -9.81f;
  updt_display = true;
  step_type = STD_STEP;
  stepsize = .01f;
  quick_iters = 20;
  bg_color.no_a = true;
  bg_color.r = 0.8f;
  bg_color.g = 0.8f;
  bg_color.b = 0.8f;
}

void VEWorld::Destroy() {
  CutLinks();
}

void VEWorld::CutLinks() {
  objects.CutLinks();
  spaces.CutLinks();
  DestroyODE();                 // do this last!
  inherited::CutLinks();
}

void VEWorld::Copy_(const VEWorld& cp) {
  TA_VEWorld.CopyOnlySameType((void*)this, (void*)&cp);
  UpdatePointers_NewPar((taBase*)&cp, this); // update all the pointers
}

bool VEWorld::CreateODE() {
  if(!world_id)
    world_id = (dWorldID)dWorldCreate();
  if(TestError(!world_id, "CreateODE", "could not create world id!"))
    return false;
  if(space_id && space_type == cur_space_type) return true;
  if(space_id) {
    dSpaceDestroy((dSpaceID)space_id);
    space_id = NULL;
  }
  switch(space_type) {
  case SIMPLE_SPACE:
    space_id = (dSpaceID)dSimpleSpaceCreate(NULL);
    break;
  case HASH_SPACE:
    space_id = (dSpaceID)dHashSpaceCreate(NULL);
    break;
  }
  if(TestError(!space_id, "CreateODE", "could not create space id!"))
    return false;
  cur_space_type = space_type;

  if(!cgp_id)
    cgp_id = dJointGroupCreate(0); // 0 = max_size = not used
  if(TestError(!cgp_id, "CreateODE", "could not create contact group id!"))
    return false;

  dSetErrorHandler(VE_ErrorHandler);
  dSetDebugHandler(VE_DebugHandler);
  dSetMessageHandler(VE_MessageHandler);

  return true;
}

void VEWorld::DestroyODE() {
  objects.DestroyODE();
  spaces.DestroyODE();

  if(cgp_id) dJointGroupDestroy((dJointGroupID)cgp_id);
  cgp_id = NULL;
  if(space_id) dSpaceDestroy((dSpaceID)space_id);
  space_id = NULL;
  if(world_id) dWorldDestroy((dWorldID)world_id);
  world_id = NULL;
}

void VEWorld::Init() {
  last_to_set_ode = this;

  if(!world_id || !space_id || space_type != cur_space_type || !cgp_id) CreateODE();
  if(!world_id || !space_id || !cgp_id) return;
  dWorldID wid = (dWorldID)world_id;
  dSpaceID sid = (dSpaceID)space_id;

  dWorldSetGravity(wid, gravity.x, gravity.y, gravity.z);
  dWorldSetERP(wid, ode_params.erp);
  dWorldSetCFM(wid, ode_params.cfm);
  dWorldSetContactMaxCorrectingVel(wid, ode_params.max_cor_vel);
  dWorldSetContactSurfaceLayer(wid, ode_params.contact_depth);

  if(space_type == HASH_SPACE) {
    dHashSpaceSetLevels(sid, hash_levels.min, hash_levels.max);
  }

  StructUpdate(true);
  objects.Init();
  spaces.Init();
  StructUpdate(false);          // trigger full rebuild!

  last_to_set_ode = NULL; // turn off!
}

void VEWorld::CurToODE() {
  objects.CurToODE();
}

void VEWorld::CurFromODE() {
  bool updt_disp = updt_display;
  if(!taMisc::gui_active)
    updt_disp = false;
  objects.CurFromODE();
  if(updt_disp)
    SigEmitUpdated(); // update displays..

  if(updt_display && taMisc::gui_no_win) {
    // normal data changed stuff doesn't work in gui_no_win, so we use a manual call
    // only if cameras are set
    if((bool)camera_0 || (bool)camera_1) {
      UpdateView();
    }
  }
  // not really nec
  // void dWorldGetGravity (dWorldID, dVector3 gravity);
}

void VEWorld::CurToInit() {
  objects.CurToInit();
}

void VEWorld::SnapPosToGrid(float grid_size, bool init_pos) {
  objects.SnapPosToGrid(grid_size, init_pos);
  spaces.SnapPosToGrid(grid_size);
}

void VEWorld::CollisionCallback(dGeomID o1, dGeomID o2) {
  static dContactGeom cgs[64];
  dWorldID wid = (dWorldID)world_id;
  dJointGroupID gid = (dJointGroupID)cgp_id;
  int num_contact = dCollide(o1,o2, ode_params.max_col_pts, cgs, sizeof(dContactGeom));
  // add these contact points to the simulation ...
  VEBody* b1 = (VEBody*)dGeomGetData(o1);
  VEBody* b2 = (VEBody*)dGeomGetData(o2);
  dContact cont;
  cont.surface.mode = dContactBounce | dContactSoftERP | dContactSoftCFM;
  cont.surface.mu = .5f * (b1->surface.friction + b2->surface.friction);
  cont.surface.bounce = .5f * (b1->surface.bounce + b2->surface.bounce);
  cont.surface.bounce_vel = .5f * (b1->surface.bounce_vel + b2->surface.bounce_vel);
  cont.surface.soft_erp = .5f * (b1->softness.erp + b2->softness.erp);
  cont.surface.soft_cfm = .5f * (b1->softness.cfm + b2->softness.cfm);
  // todo: not seting slip1 or second directions (as in tires)
  if(num_contact > 0) {
    for(int i=0;i<num_contact;i++) {
      cont.geom = cgs[i];
      dJointID c = dJointCreateContact(wid, gid, &cont);
      dJointAttach(c, dGeomGetBody(cont.geom.g1), dGeomGetBody(cont.geom.g2));
    }
  }
}

void nearCallback(void *data, dGeomID o1, dGeomID o2) {
  if (dGeomIsSpace (o1) || dGeomIsSpace (o2)) { // colliding a space with something
    dSpaceCollide2 (o1,o2,data,&nearCallback); // collide all geoms internal to the space(s)
    if (dGeomIsSpace (o1)) dSpaceCollide ((dSpaceID)o1,data,&nearCallback);
    if (dGeomIsSpace (o2)) dSpaceCollide ((dSpaceID)o2,data,&nearCallback);
  } else {
    ((VEWorld*)data)->CollisionCallback(o1, o2);
  }
}

void VEWorld::Step_pre() {
  objects.Step_pre();
}

void VEWorld::Step() {
  if(!world_id || !space_id || !cgp_id) return;

  last_ve_stepped = this;

  Step_pre();

  dWorldID wid = (dWorldID)world_id;
  dSpaceID sid = (dSpaceID)space_id;
  dJointGroupID gid = (dJointGroupID)cgp_id;

  dSpaceCollide(sid, (void*)this, &nearCallback);

  if(step_type == STD_STEP) {
    dWorldStep(wid, stepsize);
  }
  else {
    dWorldQuickStep(wid, stepsize);
  }

  dJointGroupEmpty(gid);

  CurFromODE();

  last_ve_stepped = NULL;
}

float VEWorld::SnapVal(float val, float grid_size) {
  int ival = (int)((val / grid_size) + .5f);
  return (float)ival * grid_size;
}

// in ta_virtenv_qtso.cpp:  QImage VEWorld::GetCameraImage(int cam_no)

