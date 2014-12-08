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

#include "UnitSpec.h"
#include <Network>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(UnitSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(UnitSpec_SPtr);
SMARTREF_OF_CPP(UnitSpec);

void UnitSpec::Initialize() {
  min_obj_type = &TA_Unit;
  act_range.max = 1.0f; act_range.min = 0.0f;
  act_range.range = 1.0f; act_range.scale = 1.0f;
  sse_tol = 0.0f;
}

void UnitSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_UnitSpec); // allow all of this general spec type to be created under here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(act_range, this);
  taBase::Own(bias_spec, this);
  // don't do this if loading -- creates specs in all the wrong places..
  // specs that own specs have this problem
  if(!taMisc::is_loading) {
    Network* net = (Network *) GET_MY_OWNER(Network);
    if(net && !net->HasBaseFlag(COPYING))
      bias_spec.SetDefaultSpec(this);
  }
}

void UnitSpec::CutLinks() {
  bias_spec.CutLinks();
  inherited::CutLinks();
}

void UnitSpec::Copy_(const UnitSpec& cp) {
  act_range = cp.act_range;
  bias_spec = cp.bias_spec;
  sse_tol = cp.sse_tol;
}

bool UnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  return true;
}

void UnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
}

bool UnitSpec::CheckType_impl(TypeDef* td) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (td->InheritsFrom(TA_Layer))
    return true;
  return inherited::CheckType_impl(td);
}

bool UnitSpec::CheckObjectType_impl(taBase* obj) {
  // other specs are allowed to own any kind of other spec,
  // and layers and projections also contain specs..
  if (obj->InheritsFrom(TA_Layer))
    return true;
  return inherited::CheckObjectType_impl(obj);
}

void UnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_range.UpdateAfterEdit_NoGui();
}


////////////////////////////////////////////////////////////////////////////////
//  Below are the primary computational interface to the Network Objects
//  for performing algorithm-specific activation and learning
//  Most functions operate directly on the units via threads, and then
//  call through to the layers for any layer-level subsequent processing
//  ConGroup level processing is all done as a separate pass at network level
//  so DON'T do that here!

void UnitSpec::Init_Vars(UnitVars* u, Network* net, int thr_no) {
  u->ext_flag = UnitVars::NO_EXTERNAL;
  u->ext = 0.0f;
  u->targ = 0.0f;
  u->net = 0.0f;
  u->act = 0.0f;
  u->bias_wt = 0.0f;
  u->bias_dwt = 0.0f;
}

void UnitSpec::Init_InputData(UnitVars* u, Network* net, int thr_no) {
  u->ClearExtFlag(UnitVars::COMP_TARG_EXT);
  u->ext = 0.0f;
  u->targ = 0.0f;
}

void UnitSpec::Init_Acts(UnitVars* u, Network* net, int thr_no) {
  u->ClearExtFlag(UnitVars::COMP_TARG_EXT);
  u->ext = 0.0f;
  u->targ = 0.0f;
  u->net = 0.0f;
  u->act = 0.0f;
}

void UnitSpec::Init_dWt(UnitVars* u, Network* net, int thr_no) {
  if(bias_spec) {
    bias_spec->B_Init_dWt(u, net, thr_no);
  }
}

void UnitSpec::Init_Weights(UnitVars* u, Network* net, int thr_no) {
  if(bias_spec) {
    bias_spec->B_Init_Weights(u, net, thr_no);
  }
}

void UnitSpec::Init_Weights_post(UnitVars* u, Network* net, int thr_no) {
  if(bias_spec) {
    bias_spec->B_Init_Weights_post(u, net, thr_no);
  }
}

void UnitSpec::Compute_Netin(UnitVars* u, Network* net, int thr_no) {
  float new_net = 0.0f;
  const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
  for(int g=0; g<nrcg; g++) {
    ConGroup* rgp = net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, g);
    if(rgp->NotActive()) continue;
    new_net += rgp->con_spec->Compute_Netin(rgp, net, thr_no);
  }
  if(bias_spec) {
    new_net += u->bias_wt;
  }
  u->net = new_net;
}

void UnitSpec::Compute_SentNetin(UnitVars* u, Network* net, float sent_netin) {
  // called by network-level Send_Netin function to integrate sent netin value
  // with current net input value -- default is just to set to net val + bias wt if avail
  u->net = sent_netin;
  if(bias_spec) {
    u->net += u->bias_wt;
  }
}

void UnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->HasExtFlag(UnitVars::EXT))
    u->act = u->ext;
  else
    u->act = u->net;
}

void UnitSpec::Compute_NetinAct(UnitVars* u, Network* net, int thr_no) {
  Compute_Netin(u, net, thr_no);
  Compute_Act(u, net, thr_no);
}

void UnitSpec::Compute_dWt(UnitVars* u, Network* net, int thr_no) {
  if(bias_spec) {
    bias_spec->B_Compute_dWt(u, net, thr_no);
  }
}

void UnitSpec::Compute_Weights(UnitVars* u, Network* net, int thr_no) {
  if(bias_spec) {
    bias_spec->B_Compute_Weights(u, net, thr_no);
  }
}

float UnitSpec::Compute_SSE(UnitVars* u, Network* net, int thr_no, bool& has_targ) {
  float sse = 0.0f;
  has_targ = false;
  if(u->HasExtFlag(UnitVars::COMP_TARG)) {
    has_targ = true;
    float uerr = u->targ - u->act;
    if(fabsf(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}

bool UnitSpec::Compute_PRerr
(UnitVars* u, Network* net, int thr_no,
 float& true_pos, float& false_pos, float& false_neg, float& true_neg) {
  true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f; true_neg = 0.0f;
  bool has_targ = false;
  if(u->HasExtFlag(UnitVars::COMP_TARG)) {
    has_targ = true;
    if(u->targ > u->act) {
      true_pos = u->act;
      true_neg = 1.0 - u->targ;
      false_neg = u->targ - u->act;
    }
    else {
      true_pos = u->targ;
      false_pos = u->act - u->targ;
      true_neg = 1.0 - u->act;
    }
  }
  return has_targ;
}
