// Copyright 2013-2017, Regents of the University of Colorado,
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
#include <Projection>
#include <Connection>
#include <UnitSpec>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(ConSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(ConSpec_SPtr);
TA_BASEFUNS_CTORS_DEFN(WeightLimits);
SMARTREF_OF_CPP(ConSpec);


void ConSpec::Initialize() {
  min_obj_type = &TA_Connection;
  min_user_type = &TA_Projection;

  Initialize_core();
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

void ConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(TestWarning(rnd.var == 0.0f && wt_limits.sym, "UAE",
                 "random variance rnd.var is 0, so wt_limits.sym is now turned off to prevent other weights from symmetrizing against fixed non-random weights")) {
    SetUnique("wt_limits", true);
    wt_limits.sym = false;
  }
}

bool ConSpec::CheckConfig_RecvCons(Projection* prjn, bool quiet) {
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
  if(obj->InheritsFrom(&TA_Projection)) { // this SHOULD be the only case..
    con_tp = ((Projection*)obj)->con_type;
    if(!con_tp->InheritsFrom(min_obj_type))
      return false;
    return true;
  }
  return true; 
}

bool ConSpec::CheckType_impl(TypeDef* td) {
  if (td->InheritsFrom(TA_Projection))
    return true;
  return inherited::CheckType_impl(td);
}

void ConSpec::UpdateStateSpecs() {
  Network* net = GET_MY_OWNER(Network);
  if(!net || !net->IsBuiltIntact() || spec_idx < 0) return;
  CopyToState(net->net_state->con_specs[spec_idx], net->net_state->GetStateSuffix());
#ifdef CUDA_COMPILE
  CopyToState(net->cuda_state->con_specs[spec_idx], net->cuda_state->GetStateSuffix());
#endif
}

void ConSpec::ResetAllSpecIdxs() {
  spec_idx = -1;
  inherited::ResetAllSpecIdxs(); // calls on children
}
