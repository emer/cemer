// Copyright 2017, Regents of the University of Colorado,
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

#include "ProjectionSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(RenormInitWtsSpec);
TA_BASEFUNS_CTORS_DEFN(ProjectionSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(ProjectionSpec_SPtr);
SMARTREF_OF_CPP(ProjectionSpec);

void RenormInitWtsSpec::Initialize() {
  on = false;
  mult_norm = true;
  avg_wt = 0.5f;
}

void ProjectionSpec::Initialize() {
  min_obj_type = &TA_Projection;
  min_user_type = &TA_Projection;
  self_con = false;
  init_wts = false;
  set_scale = false;
  init_wt_val = 1.0f;
  add_rnd_var = false;
}

void ProjectionSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ProjectionSpec); // allow all of this general spec type to be created under here
  taBase::Own(renorm_wts, this);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void ProjectionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(set_scale) {
    Network* net = GET_MY_OWNER(Network);
    if(TestWarning(net && !net->InheritsFromName("LeabraNetwork"),
                   "Init_Weights_Prjn", "set_scale can only be used with Leabra networks -- turning off")) {
      set_scale = false;
    }
  }
}

void ProjectionSpec::Connect_Sizes(Projection* prjn) {
  prjn->SetFrom();
  if(TestWarning(!(bool)prjn->from, "Connect", "from pointer is NULL -- cannot make this projection"))
    return;
  if(!prjn->IsActive()) return;
  Connect_impl(prjn, false);
}

void ProjectionSpec::Connect_Cons(Projection* prjn) {
  if(!prjn->IsActive()) return;
  Connect_impl(prjn, true);
  // Init_Weights(prjn); // connection is NOT init weights -- now definitivitely 2 separate steps -- this is super slow for large nets
  prjn->projected = true;
}

int ProjectionSpec::ProbAddCons_impl(Projection* prjn, float p_add_con, float init_wt) {
  return 0;
}

int ProjectionSpec::ProbAddCons(Projection* prjn, float p_add_con, float init_wt) {
  int rval = ProbAddCons_impl(prjn, p_add_con);
  return rval;
}

void ProjectionSpec::SetCnWt(float wt_val, ConState_cpp* cg, int cn_idx, Network* net, int thr_no) {
  NetworkState_cpp* net_state = net->net_state;
  ConSpec_cpp* cs = cg->GetConSpec(net_state);
  if(add_rnd_var) {
    int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
    cs->C_Init_Weight_AddRndVar(wt_val, eff_thr_no);
  }
  cs->C_ApplyLimits(wt_val);
  cg->Cn(cn_idx,ConState_cpp::WT,net_state) = wt_val;
  cs->SetConScale(1.0f, cg, cn_idx, net_state, thr_no); // reset scale..
  cs->C_Init_dWt(cg->Cn(cn_idx,ConState_cpp::DWT,net_state));
}

void ProjectionSpec::SetCnWtRnd(ConState_cpp* cg, int cn_idx, Network* net, int thr_no) {
  NetworkState_cpp* net_state = net->net_state;
  ConSpec_cpp* cs = cg->GetConSpec(net_state);
  float& wt_val = cg->Cn(cn_idx,ConState_cpp::WT,net_state);
  int eff_thr_no = net->HasNetFlag(Network::INIT_WTS_1_THREAD) ? 0 : thr_no;
  cs->C_Init_Weight_Rnd(wt_val, eff_thr_no); // std rnd wts
  cs->C_Init_dWt(cg->Cn(cn_idx,ConState_cpp::DWT,net_state));
}

void ProjectionSpec::SetCnScale(float sc_val, ConState_cpp* cg, int cn_idx, Network* net, int thr_no) {
  NetworkState_cpp* net_state = net->net_state;
  ConSpec_cpp* cs = cg->GetConSpec(net_state);
  cs->SetConScale(sc_val, cg, cn_idx, net_state, thr_no);
}

void ProjectionSpec::Init_Weights_Prjn(Projection* prjn, ConState_cpp* cg,
                                       Network* net, int thr_no) {
  if(!init_wts) return;         // shouldn't happen
  NetworkState_cpp* net_state = net->net_state;
  ConSpec* cs = prjn->GetConSpec();
  if(set_scale) {
    cs->Init_Weights_scale(cg, net_state, thr_no, init_wt_val);
  }
  else {
    cs->Init_Weights(cg, net_state, thr_no);
  }
}

void ProjectionSpec::Init_Weights_renorm(Projection* prjn, ConState_cpp* cg,
                                         Network* net, int thr_no) {
  if(!renorm_wts.on) return;
  NetworkState_cpp* net_state = net->net_state;
  ConSpec* cs = prjn->GetConSpec();
  if(set_scale) {
    cs->RenormScales(cg, net_state, thr_no, renorm_wts.mult_norm, renorm_wts.avg_wt);
  }
  else {
    cs->RenormWeights(cg, net_state, thr_no, renorm_wts.mult_norm, renorm_wts.avg_wt);
  }
}

bool ProjectionSpec::HasRandomScale() {
  return init_wts && set_scale;
}

bool ProjectionSpec::CheckConnect(Projection* prjn, bool quiet) {
  if(prjn->off) return true;
  bool rval;
  if(prjn->CheckError(!prjn->from, quiet, rval, "from is null -- must set from!")) {
    return false;
  }
  if(prjn->from->lesioned()) return true;
  if(prjn->CheckError(!prjn->projected, quiet, rval, "not connected!")) {
    return false;
  }
  if(prjn->CheckError(!prjn->con_spec.spec, quiet, rval, "has null con_spec")) {
    return false;
  }
  if(prjn->CheckError(!prjn->con_spec->CheckObjectType(prjn), quiet, rval,
                "does not have correct spec/object type")) {
    return false;
  }
  return true;
}

