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
#include <Projection>
#include <Connection>
#include <UnitSpec>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(ConSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(ConSpec_SPtr);
TA_BASEFUNS_CTORS_DEFN(WeightLimits);
SMARTREF_OF_CPP(ConSpec);

void WeightLimits::Initialize() {
  type = NONE;
  min = -1.0f;
  max = 1.0f;
  sym = false;
  sym_fm_top = false;
}

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

void ConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(TestWarning(rnd.var == 0.0f && wt_limits.sym, "UAE",
                 "random variance rnd.var is 0, so wt_limits.sym is now turned off to prevent other weights from symmetrizing against fixed non-random weights")) {
    SetUnique("wt_limits", true);
    wt_limits.sym = false;
  }
}

bool ConSpec::CheckConfig_RecvCons(ConGroup* cg, bool quiet) {
  return true;
}

bool ConSpec::SaveConVarToWeights(MemberDef* md) {
  return md->HasOption("SAVE");
}

void ConSpec::Init_Weights_Net() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->Init_Weights();
  net->UpdateAllViews();
}

bool ConSpec::CheckObjectType_impl(taBase* obj) {
  TypeDef* con_tp = &TA_Connection;
  if(obj->InheritsFrom(&TA_ConGroup)) {
    con_tp = ((ConGroup*)obj)->ConType();
  }
  else if(obj->InheritsFrom(&TA_Projection)) {
    con_tp = ((Projection*)obj)->con_type;
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

void ConSpec::ApplySymmetry_r(ConGroup* cg, Network* net, int thr_no) {
  Unit* ru = cg->ThrOwnUn(net, thr_no);
  if(!wt_limits.sym) return;
  for(int i=0; i<cg->size;i++) {
    int con_idx = -1;
    ConGroup* rrcg = ConGroup::FindRecipRecvCon(con_idx, cg->Un(i,net), ru,
                                                cg->prjn->layer);
    if(rrcg && con_idx >= 0) {
      ConSpec* rrcs = rrcg->GetConSpec();
      if(rrcs && rrcs->wt_limits.sym) {
        if(wt_limits.sym_fm_top)
          rrcg->OwnCn(con_idx, WT) = cg->OwnCn(i, WT); // todo: not sure this order is right
        else
          cg->OwnCn(i, WT) = rrcg->OwnCn(con_idx, WT); // theoretically should be opp of s
      }
    }
  }
}

void ConSpec::ApplySymmetry_s(ConGroup* cg, Network* net, int thr_no) {
  Unit* su = cg->ThrOwnUn(net, thr_no);
  if(!wt_limits.sym) return;
  for(int i=0; i<cg->size;i++) {
    int con_idx = -1;
    ConGroup* rscg = ConGroup::FindRecipSendCon(con_idx, cg->Un(i,net), su,
                                                cg->prjn->from.ptr());
    if(rscg && con_idx >= 0) {
      ConSpec* rscs = rscg->GetConSpec();
      if(rscs && rscs->wt_limits.sym) {
        if(wt_limits.sym_fm_top)
          cg->OwnCn(i, WT) = rscg->OwnCn(con_idx, WT);
        else
          rscg->OwnCn(con_idx, WT) = cg->OwnCn(i, WT);
      }
    }
  }
}

void ConSpec::RenormWeights(ConGroup* cg, Network* net, int thr_no,
                            bool mult_norm, float avg_wt) {
  if(cg->size < 2) return;
  float avg = 0.0f;
  for(int i=0; i<cg->size; i++) {
    avg += cg->Cn(i, WT, net);
  }
  avg /= (float)cg->size;
  if(mult_norm) {
    float adj = avg_wt / avg;
    for(int i=0; i<cg->size; i++) {
      cg->Cn(i, WT, net) *= adj;
    }
  }
  else {
    float adj = avg_wt - avg;
    for(int i=0; i<cg->size; i++) {
      cg->Cn(i, WT, net) += adj;
    }
  }
}


void ConSpec::LoadWeightVal(float wtval, ConGroup* cg, int cidx, Network* net) {
  cg->Cn(cidx,WT,net) = wtval;
}
