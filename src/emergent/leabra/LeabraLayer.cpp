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

#include "LeabraLayer.h"
#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(LeabraLayer);
SMARTREF_OF_CPP(LeabraLayer);

void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  units.SetBaseType(&TA_LeabraUnit);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Inhib_Initialize();
  adapt_gi = 1.0f;
  hard_clamped = false;
  da_p = 0.0f;
  da_n = 0.0f;
  sev = 0.0f;
  bin_err = 0.0f;
  norm_err = 0.0f;
  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;
  cos_diff = 0.0f;
  cos_diff_avg = 0.0f;
  cos_diff_avg_lrn = 0.0f;
  lrate_mod = 1.0f;
  avg_act_diff = 0.0f;
  trial_cos_diff = 0.0f;
  net_sd = 0.0f;
  hog_pct = 0.0f;
  dead_pct = 0.0f;

  avg_netin_n = 0;
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(spec, this);

  // own all the LeabraInhib guys! -- also update in LeabraUnGpData::InitLinks()
  taBase::Own(i_val, this);
  
  taBase::Own(netin, this);
  taBase::Own(netin_raw, this);
  taBase::Own(acts, this);
  taBase::Own(acts_eq, this);
  taBase::Own(acts_raw, this);
  taBase::Own(acts_prvq, this);

  taBase::Own(acts_m, this);
  taBase::Own(acts_p, this);
  taBase::Own(acts_q0, this);

  taBase::Own(un_g_i, this);
  
  taBase::Own(am_deep_raw, this);
  taBase::Own(am_deep_raw_net, this);
  taBase::Own(am_deep_mod_net, this);
  taBase::Own(am_deep_ctxt, this);

  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

  taBase::Own(ungp_data, this);
  taBase::Own(multigp_data, this);
  taBase::Own(laygp_data, this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif
  spec.SetDefaultSpec(this);

  AutoNameMyMembers();
}

void LeabraLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
}

void LeabraLayer::Copy_(const LeabraLayer& cp) {
  Inhib_Copy_(cp);
  spec = cp.spec;
  hard_clamped = cp.hard_clamped;
  da_p = cp.da_p;
  da_n = cp.da_n;
  sev = cp.sev;
  bin_err = cp.bin_err;
  norm_err = cp.norm_err;
  cos_err = cp.cos_err;
  cos_err_prv = cp.cos_err_prv;
  cos_err_vs_prv = cp.cos_err_vs_prv;
  cos_diff = cp.cos_diff;
  cos_diff_avg = cp.cos_diff_avg;
  cos_diff_avg_lrn = cp.cos_diff_avg_lrn;
  lrate_mod = cp.lrate_mod;
  avg_act_diff = cp.avg_act_diff;
  trial_cos_diff = cp.trial_cos_diff;
  net_sd = cp.net_sd;
  hog_pct = cp.hog_pct;
  dead_pct = cp.dead_pct;

  // this will update spec pointer to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  // (other pointers are already dealt with in Layer)
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);
}

void LeabraLayer::CheckSpecs() {
  spec.CheckSpec();
  inherited::CheckSpecs();
}

void LeabraLayer::CheckThisConfig_impl(bool quiet, bool& rval) {
  if(lesioned()) return;
  inherited::CheckThisConfig_impl(quiet, rval);

  if(!spec->CheckConfig_Layer(this, quiet)) {
    rval = false;
  }
}

void LeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void LeabraLayer::Init_InputData(Network* net) {
  inherited::Init_InputData(net);
  hard_clamped = false;		// reset our flag
}

bool LeabraLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)        return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((LeabraLayerSpec*)sp);
  else
    return false;
  return true;
}

void LeabraLayer::BuildUnits() {
  inherited::BuildUnits();
  ungp_data.Reset();
  ungp_data.SetBaseType(((LeabraLayerSpec*)GetLayerSpec())->UnGpDataType());
  ungp_data.SetSize(gp_geom.n);

  multigp_data.Reset();
  multigp_data.SetBaseType(((LeabraLayerSpec*)GetLayerSpec())->UnGpDataType());
  multigp_data.SetSize(gp_geom.n);
}

LeabraUnit* LeabraLayer::GetMostActiveUnit() {
  if(acts.max_i < 0 || !own_net) return NULL;
  return (LeabraUnit*)own_net->UnFmIdx(acts.max_i);
}

#ifdef DMEM_COMPILE
void LeabraLayer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraLayer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

