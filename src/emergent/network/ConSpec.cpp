// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ConSpec.h"
#include <Network>
#include <RecvCons>
#include <SendCons>
#include <Projection>
#include <Connection>
#include <UnitSpec>

void ConSpec::Initialize() {
  min_obj_type = &TA_Connection;
  rnd.type = Random::UNIFORM;
  rnd.mean = 0.0f;
  rnd.var = .5f;
}

void ConSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ConSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(rnd,this);
  taBase::Own(wt_limits,this);
}

void ConSpec::CutLinks() {
  BaseSpec::CutLinks();
}

void ConSpec::Copy_(const ConSpec& cp) {
  //  min_con_type = cp.min_con_type;  // don't do this -- could be going between types
  rnd = cp.rnd;
  wt_limits = cp.wt_limits;
}

bool ConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  return true;
}

void ConSpec::Init_Weights_Net() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->Init_Weights();
  net->UpdateAllViews();
}

bool ConSpec::CheckObjectType_impl(taBase* obj) {
  TypeDef* con_tp = &TA_Connection;
  if(obj->InheritsFrom(&TA_RecvCons)) {
    con_tp = ((RecvCons*)obj)->con_type;
  }
  else if(obj->InheritsFrom(&TA_SendCons)) {
    con_tp = ((SendCons*)obj)->con_type;
  }
  else if(obj->InheritsFrom(&TA_Projection)) {
    con_tp = ((Projection*)obj)->con_type;
  }
  else if(obj->InheritsFrom(&TA_UnitSpec)) {
    if(((UnitSpec*)obj)->bias_con_type)
      con_tp = ((UnitSpec*)obj)->bias_con_type;
  }
  if(!con_tp->InheritsFrom(min_obj_type))
    return false;
  return true;
}

bool ConSpec::CheckType_impl(TypeDef* td) {
  if (td->InheritsFrom(TA_Projection))
    return true;
  return inherited::CheckType_impl(td);
}


void ConSpec::ApplySymmetry(RecvCons* cg, Unit* ru) {
  if(!wt_limits.sym) return;
  Connection* rc, *su_rc;
  for(int i=0; i<cg->size;i++) {
    rc = cg->Cn(i);
    su_rc = BaseCons::FindRecipRecvCon(cg->Un(i), ru, cg->prjn->layer);
    if(su_rc)
      su_rc->wt = rc->wt;       // set other's weight to ours (otherwise no random..)
  }
}
