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
  bias_con_type = NULL;
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
  bias_con_type = cp.bias_con_type;
  bias_spec = cp.bias_spec;
  sse_tol = cp.sse_tol;
}

bool UnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  return true;
}

void UnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!bias_con_type) return;
  if(CheckError(!bias_spec.spec, quiet, rval,
                 "Bias con type of:", bias_con_type->name,
                 "does not have a spec set!"))
    return;
  CheckError((!bias_con_type->InheritsFrom(bias_spec.spec->min_obj_type)), quiet, rval,
             "Bias con type of:", bias_con_type->name,
             "is not of the correct type for the bias con spec,"
             "which needs at least a:", bias_spec.spec->min_obj_type->name);
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

void UnitSpec::BuildBiasCons() {
  Network* net = (Network *) GET_MY_OWNER(Network);
  if (!net) return;
  net->BuildUnits();
}

////////////////////////////////////////////////////////////////////////////////
//      Below are the primary computational interface to the Network Objects
//      for performing algorithm-specific activation and learning
//      Many functions operate directly on the units via threads, and then
//      call through to the layers for any layer-level subsequent processing
//      units typically call spec versions except for basic stuff

//      Init functions are NOT threaded, while Compute functions are

void UnitSpec::Init_Acts(Unit* u, Network* net) {
  u->Init_InputData();
  u->net = 0.0f;
  u->act = 0.0f;
}

void UnitSpec::Init_dWt(Unit* u, Network* net) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->size) continue;
    recv_gp->Init_dWt(u, net);
  }
  if(u->bias.size > 0) {
    bias_spec->C_Init_dWt(&u->bias, 0, u, NULL, net);  // this is a virtual fun
  }
}

void UnitSpec::Init_Weights(Unit* u, Network* net) {
  u->snap = 0.0f;

#ifdef DMEM_COMPILE
  if(!u->DMem_IsLocal()) {
    // make up for random numbers not being used for non-local connections.
    for(int i=0; i<u->n_recv_cons; i++) Random::ZeroOne();
  }
  else
#endif
    {
      for(int g = 0; g < u->recv.size; g++) {
        RecvCons* recv_gp = u->recv.FastEl(g);
        // ignore lesion here because n_recv_cons does not take into account lesioned layers, so dmem would get out of sync
        //    if(!recv_gp->prjn->from->lesioned())
        recv_gp->Init_Weights(u, net);
      }
    }

  if(u->bias.size > 0) {
    bias_spec->C_Init_Weights(&u->bias, 0, u, NULL, net);
    // this is a virtual fun
    bias_spec->C_Init_dWt(&u->bias, 0, u, NULL, net);
    // don't forget delta too!!
  }
}

void UnitSpec::Init_Weights_post(Unit* u, Network* net) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->size) continue;
    recv_gp->Init_Weights_post(u, net);
  }
  if(u->bias.size > 0) {
    bias_spec->C_Init_Weights_post(&u->bias, 0, u, NULL, net); // this is a virtual fun
  }
}

void UnitSpec::Compute_Netin(Unit* u, Network* net, int thread_no) {
  u->net = 0.0f;
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->size) continue;
    u->net += recv_gp->Compute_Netin(u, net);
  }
  if(u->bias.size > 0)
    u->net += u->bias.OwnCn(0,BaseCons::WT);
}

void UnitSpec::Send_Netin(Unit* u, Network* net, int thread_no) {
  // typically the whole point of using sender based net input is that you want to check
  // here if the sending unit's activation (i.e., this one) is above some threshold
  // so you don't send if it isn't above that threshold..  this isn't implemented here though.
  if(thread_no < 0) thread_no = 0; // use 0 thread in tmp matrix in this case
  if(net->NetinPerPrjn()) {
    for(int g = 0; g < u->send.size; g++) {
      SendCons* send_gp = u->send.FastEl(g);
      Layer* tol = send_gp->prjn->layer;
      if(tol->lesioned() || !send_gp->size) continue;
      send_gp->Send_Netin_PerPrjn(net, thread_no, u);
    }
  }
  else {
    for(int g = 0; g < u->send.size; g++) {
      SendCons* send_gp = u->send.FastEl(g);
      Layer* tol = send_gp->prjn->layer;
      if(tol->lesioned() || !send_gp->size) continue;
      send_gp->Send_Netin(net, thread_no, u);
    }
  }
}

void UnitSpec::Compute_SentNetin(Unit* u, Network* net, float sent_netin) {
  // called by network-level Send_Netin function to integrate sent netin value
  // with current net input value -- default is just to set to net val + bias wt if avail
  u->net = sent_netin;
  if(u->bias.size)
    u->net += u->bias.OwnCn(0,BaseCons::WT);
}

void UnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->HasExtFlag(Unit::EXT))
    u->act = u->ext;
  else
    u->act = u->net;
}

void UnitSpec::Compute_dWt(Unit* u, Network* net, int thread_no) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->size) continue;
    recv_gp->Compute_dWt(u, net);
  }
  // NOTE: derived classes must supply bias.OwnCn(0)->Compute_dWt call because C_Compute_dWt
  // is not virtual, so if called here, only ConSpec version would be called.
  // This is not true of Init_Weights and Init_dWt, which are virtual.
}

void UnitSpec::Compute_Weights(Unit* u, Network* net, int thread_no) {
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->size) continue;
    recv_gp->Compute_Weights(u, net);
  }
  // NOTE: derived classes must supply bias.OwnCn(0)->Compute_Weights call because C_Compute_Weights
  // is not virtual, so if called here, only ConSpec version would be called.
  // This is not true of Init_Weights and Init_dWt, which are virtual.
}

float UnitSpec::Compute_SSE(Unit* u, Network* net, bool& has_targ) {
  float sse = 0.0f;
  has_targ = false;
  if(u->HasExtFlag(Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = u->targ - u->act;
    if(fabsf(uerr) >= sse_tol)
      sse = uerr * uerr;
  }
  return sse;
}

bool UnitSpec::Compute_PRerr(Unit* u, Network* net, float& true_pos, float& false_pos, float& false_neg) {
  true_pos = 0.0f; false_pos = 0.0f; false_neg = 0.0f;
  bool has_targ = false;
  if(u->HasExtFlag(Unit::TARG | Unit::COMP)) {
    has_targ = true;
    float uerr = u->targ - u->act;
    if(fabsf(uerr) < sse_tol) {
      true_pos = u->targ;
    }
    else {
      if(u->targ > u->act) {
        true_pos = u->act;
        false_neg = u->targ - u->act;
      }
      else {
        true_pos = u->targ;
        false_pos = u->act - u->targ;
      }
    }
  }
  return has_targ;
}
