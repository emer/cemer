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

#include "ProjectionSpec.h"
#include <Network>

TA_BASEFUNS_CTORS_DEFN(ProjectionSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(ProjectionSpec_SPtr);
SMARTREF_OF_CPP(ProjectionSpec);


void ProjectionSpec::Initialize() {
  min_obj_type = &TA_Projection;
  self_con = false;
  init_wts = false;
  set_scale = false;
  add_rnd_var = false;
}

void ProjectionSpec::InitLinks() {
  BaseSpec::InitLinks();
  children.SetBaseType(&TA_ProjectionSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
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

void ProjectionSpec::SetCnWt(float wt_val, ConGroup* cg, int cn_idx, Network* net, int thr_no) {
  ConSpec* cs = cg->GetConSpec();
  if(add_rnd_var) {
    cs->C_Init_Weight_AddRndVar(wt_val, thr_no);
  }
  cs->C_ApplyLimits(wt_val);
  cg->Cn(cn_idx,ConGroup::WT,net) = wt_val;
}

void ProjectionSpec::SetCnWtRnd(ConGroup* cg, int cn_idx, Network* net, int thr_no) {
  ConSpec* cs = cg->GetConSpec();
  float& wt_val = cg->Cn(cn_idx,ConGroup::WT,net);
  cs->C_Init_Weight_Rnd(wt_val, thr_no); // std rnd wts
}

void ProjectionSpec::SetCnScale(float sc_val, ConGroup* cg, int cn_idx, Network* net, int thr_no) {
  ConSpec* cs = cg->GetConSpec();
  cs->SetConScale(sc_val, cg, cn_idx, net, thr_no);
}

void ProjectionSpec::Init_Weights_Prjn(Projection* prjn, ConGroup* cg,
                                       Network* net, int thr_no) {
  if(!init_wts) return;         // shouldn't happen
  ConSpec* cs = prjn->GetConSpec();
  cs->Init_Weights(cg, net, thr_no);
}

bool ProjectionSpec::CheckConnect(Projection* prjn, bool quiet) {
  if(prjn->off) return true;
  bool rval;
  if(CheckError(!prjn->from, quiet, rval, "from is null -- must set from!")) {
    return false;
  }
  if(prjn->from->lesioned()) return true;
  if(CheckError(!prjn->projected, quiet, rval, "not connected!")) {
    return false;
  }
  if(CheckError(!prjn->con_spec.spec, quiet, rval, "has null con_spec")) {
    return false;
  }
  if(CheckError(!prjn->con_spec->CheckObjectType(prjn), quiet, rval,
                "does not have correct spec/object type")) {
    return false;
  }
  return true;
}

