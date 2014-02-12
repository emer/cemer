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


void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  units.SetBaseType(&TA_LeabraUnit);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Inhib_Initialize();
  hard_clamped = false;
  dav = 0.0f;
  sev = 0.0f;
  norm_err = 0.0f;
  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;
  cos_diff = 0.0f;
  cos_diff_avg = 0.0f;
  cos_diff_lrate = 1.0f;
  da_updt = false;

  avg_netin_n = 0;
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(spec, this);
  taBase::Own(netin, this);
  taBase::Own(netin_top_k, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);
  taBase::Own(acts_top_k, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);
  taBase::Own(acts_m_avg, this);
  taBase::Own(acts_ctxt, this);
  taBase::Own(acts_m2, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);

  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

  taBase::Own(sravg_vals, this);

  taBase::Own(ungp_data, this);

  taBase::Own(unit_idxs, this);

  taBase::Own(lay_kbuffs, this);
  taBase::Own(gp_kbuffs, this);

  lay_kbuffs.CreateStdBuffs();
  gp_kbuffs.CreateStdBuffs();

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif
  spec.SetDefaultSpec(this);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
}

void LeabraLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();

  lay_kbuffs.CutLinks();
  gp_kbuffs.CutLinks();
}

void LeabraLayer::Copy_(const LeabraLayer& cp) {
  Inhib_Copy_(cp);
  spec = cp.spec;
  hard_clamped = cp.hard_clamped;
  dav = cp.dav;
  sev = cp.sev;
  norm_err = cp.norm_err;
  cos_err = cp.cos_err;
  cos_err_prv = cp.cos_err_prv;
  cos_err_vs_prv = cp.cos_err_vs_prv;
  cos_diff = cp.cos_diff;
  cos_diff_avg = cp.cos_diff_avg;
  cos_diff_lrate = cp.cos_diff_lrate;

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

void LeabraLayer::ResetSortBuf() {
  lay_kbuffs.ResetAllBuffs();
  gp_kbuffs.ResetAllBuffs();
}

void LeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ResetSortBuf();
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

void LeabraLayer::BuildKwtaBuffs() {
  ResetSortBuf();
  lay_kbuffs.AllocAllBuffs(flat_geom.n, 1);
  gp_kbuffs.AllocAllBuffs(un_geom.n, gp_geom.n);

  ungp_data.Reset();
  ungp_data.SetBaseType(((LeabraLayerSpec*)GetLayerSpec())->UnGpDataType());
  ungp_data.SetSize(gp_geom.n);
}

void LeabraLayer::BuildUnits() {
  ResetSortBuf();
  inherited::BuildUnits();
  BuildKwtaBuffs();
}

// void LeabraLayer::TriggerContextUpdate() -- in leabra_extra.cpp

#ifdef DMEM_COMPILE
void LeabraLayer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraLayer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

