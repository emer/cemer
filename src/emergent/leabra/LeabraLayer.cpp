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

#include "LeabraLayer.h"
#include <LeabraNetwork>
#include <TwoDValLayerSpec>

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
  margin_low_thr = 0.495f;
  margin_med_thr = 0.503f;
  margin_hi_thr = 0.508f;
  margin_low_avg = .15f;
  margin_med_avg = 0.0375f;
  margin_hi_avg = 0.075f;
  
  hard_clamped = false;
  da_p = 0.0f;
  da_n = 0.0f;
  sev = 0.0f;
  bin_err = 0.0f;
  max_err = 0.0f;
  norm_err = 0.0f;
  cos_err = 0.0f;
  cos_err_prv = 0.0f;
  cos_err_vs_prv = 0.0f;
  cos_diff = 0.0f;
  cos_diff_avg = 0.0f;
  cos_diff_var = 0.0f;
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
  taBase::Own(netin_m, this);
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
  taBase::Own(laygp_data, this);
  taBase::Own(twod_vals, this);

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
  ungp_data.CutLinks();
  laygp_data.CutLinks();
  twod_vals.CutLinks();
}

void LeabraLayer::Copy_(const LeabraLayer& cp) {
  Inhib_Copy_(cp);
  spec = cp.spec;
  hard_clamped = cp.hard_clamped;
  da_p = cp.da_p;
  da_n = cp.da_n;
  sev = cp.sev;
  bin_err = cp.bin_err;
  max_err = cp.max_err;
  norm_err = cp.norm_err;
  cos_err = cp.cos_err;
  cos_err_prv = cp.cos_err_prv;
  cos_err_vs_prv = cp.cos_err_vs_prv;
  cos_diff = cp.cos_diff;
  cos_diff_avg = cp.cos_diff_avg;
  cos_diff_var = cp.cos_diff_var;
  cos_diff_avg_lrn = cp.cos_diff_avg_lrn;
  lrate_mod = cp.lrate_mod;
  avg_act_diff = cp.avg_act_diff;
  trial_cos_diff = cp.trial_cos_diff;
  net_sd = cp.net_sd;
  hog_pct = cp.hog_pct;
  dead_pct = cp.dead_pct;
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
  UpdateTwoDValsGeom();
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

bool LeabraLayer::TwoDValMode() {
  LeabraLayerSpec* ls = (LeabraLayerSpec*)GetLayerSpec();
  if(!ls) return false;
  return (ls->InheritsFrom(&TA_TwoDValLayerSpec));
}

void LeabraLayer::UpdateTwoDValsGeom() {
  if(!TwoDValMode()) return;
  TwoDValLayerSpec* ls = (TwoDValLayerSpec*)GetLayerSpec();
  if(unit_groups)
    twod_vals.SetGeom(5, 2, TWOD_N, ls->twod.n_vals, gp_geom.x, gp_geom.y);
  else
    twod_vals.SetGeom(5, 2, TWOD_N, ls->twod.n_vals, 1, 1);
}

void LeabraLayer::ApplyInputData_2d(taMatrix* data, UnitVars::ExtFlags ext_flags,
                              Random* ran, const taVector2i& offs, bool na_by_range) {
  if(TwoDValMode()) {
    // only no unit_group supported!
    if(TestError(unit_groups, "ApplyInputData_2d",
                 "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
      return;
    }
    for(int d_y = 0; d_y < data->dim(1); d_y++) {
      int val_idx = offs.y + d_y;
      for(int d_x = 0; d_x < data->dim(0); d_x++) {
        int xy_idx = offs.x + d_x;
        Variant val = data->SafeElAsVar(d_x, d_y);
        if(ext_flags & UnitVars::EXT)
          twod_vals.SetFmVar(val, xy_idx, TWOD_EXT, val_idx, 0, 0);
        else
          twod_vals.SetFmVar(val, xy_idx, TWOD_TARG, val_idx, 0, 0);
      }
    }
  }
  else {
    inherited::ApplyInputData_2d(data, ext_flags, ran, offs, na_by_range);
  }
}

void LeabraLayer::ApplyInputData_Flat4d(taMatrix* data, UnitVars::ExtFlags ext_flags,
                                  Random* ran, const taVector2i& offs, bool na_by_range) {
  if(TwoDValMode()) {
    // outer-loop is data-group (groups of x-y data items)
    if(TestError(!unit_groups, "ApplyInputData_Flat4d",
                 "input data must be 2d for layers without unit_groups: x,y vals and n_vals")) {
      return;
    }
    for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
      for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

        for(int d_y = 0; d_y < data->dim(1); d_y++) {
          int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
          for(int d_x = 0; d_x < data->dim(0); d_x++) {
            int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
            Unit* un = UnitAtCoord(u_x, u_y);
            if(un) {
              float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
              un->ApplyInputData(val, ext_flags, ran, na_by_range);
            }
          }
        }
      }
    }
  }
  else {
    inherited::ApplyInputData_Flat4d(data, ext_flags, ran, offs, na_by_range);
  }
}

void LeabraLayer::ApplyInputData_Gp4d(taMatrix* data, UnitVars::ExtFlags ext_flags, Random* ran,
                                bool na_by_range) {
  if(TwoDValMode()) {
    // outer-loop is data-group (groups of x-y data items)
    for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
      for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

        for(int d_y = 0; d_y < data->dim(1); d_y++) {
          int val_idx = d_y;
          for(int d_x = 0; d_x < data->dim(0); d_x++) {
            int xy_idx = d_x;
            Variant val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y);
            if(ext_flags & UnitVars::EXT)
              twod_vals.SetFmVar(val, xy_idx, TWOD_EXT, val_idx, dg_x, dg_y);
            else
              twod_vals.SetFmVar(val, xy_idx, TWOD_TARG, val_idx, dg_x, dg_y);
          }
        }
      }
    }
  }
  else {
    inherited::ApplyInputData_Gp4d(data, ext_flags, ran, na_by_range);
  }
}
