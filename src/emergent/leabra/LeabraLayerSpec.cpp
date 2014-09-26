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

#include "LeabraLayerSpec.h"
#include <LeabraNetwork>
#include <MemberDef>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraInhibSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraGabaTaus);
TA_BASEFUNS_CTORS_DEFN(LeabraGabaPcts);
TA_BASEFUNS_CTORS_DEFN(LeabraInhibMisc);
TA_BASEFUNS_CTORS_DEFN(LayerAvgActSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraClampSpec);
TA_BASEFUNS_CTORS_DEFN(LayerDecaySpec);
TA_BASEFUNS_CTORS_DEFN(LayGpInhibSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraLayerSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(LeabraLayerSpec_SPtr);
SMARTREF_OF_CPP(LeabraLayerSpec);

eTypeDef_Of(MarkerConSpec);

void LeabraInhibSpec::Initialize() {
  on = true;
  gi = 2.0f;
  Defaults_init();
}

void LeabraInhibSpec::Defaults_init() {
  ff = 1.0f;
  fb = 0.5f;
  fb_tau = 1.4f;
  ff0 = 0.1f;
  
  fb_dt = 1.0f / fb_tau;
  fb_dt_c = 1.0f - fb_dt;
}

void LeabraInhibSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  fb_dt = 1.0f / fb_tau;
  fb_dt_c = 1.0f - fb_dt;
}

void LeabraGabaTaus::Initialize() {
  on = false;
  Defaults_init();
}

void LeabraGabaTaus::Defaults_init() {
  dk_drv = 0.0f;

  as_tau = 10.0f;
  am_tau = 50.0f;
  al_tau = 150.0f;
  b_tau = 300.0f;

  as_dt = 1.0f / as_tau;
  am_dt = 1.0f / am_tau;
  al_dt = 1.0f / al_tau;
  b_dt = 1.0f / b_tau;
}

void LeabraGabaTaus::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  as_dt = 1.0f / as_tau;
  am_dt = 1.0f / am_tau;
  al_dt = 1.0f / al_tau;
  b_dt = 1.0f / b_tau;
}

void LeabraGabaPcts::Initialize() {
  Defaults_init();
}

void LeabraGabaPcts::Defaults_init() {
  as_pct = 0.5f;
  am_pct = 0.50f;
  al_pct = 0.00f;
  b_pct = 0.00f;
  // as_pct = 0.20f;
  // am_pct = 0.40f;
  // al_pct = 0.20f;
  // b_pct = 0.20f;
}

void LeabraGabaPcts::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  float sum_pct = as_pct + am_pct + al_pct + b_pct;
  TestWarning(sum_pct != 1.0f, "UAE",
              "sum across all percents of inhibitory subtypes != 1 -- is:",
              String(sum_pct));
}

void LeabraInhibMisc::Initialize() {
  self_fb = 0.0f;
  Ei_dyn = false;

  Defaults_init();
}

void LeabraInhibMisc::Defaults_init() {
  Ei_gain = 0.001f;
  Ei_tau = 50.0f;

  Ei_dt = 1.0f / Ei_tau;
}

void LeabraInhibMisc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Ei_dt = 1.0f / Ei_tau;
}

void LayerAvgActSpec::Initialize() {
  init = 0.20f;
  
  Defaults_init();
}

void LayerAvgActSpec::Defaults_init() {
  fixed = false;
  tau = 100.0f;
  adjust = 1.0f;
  dt = 1.0f / tau;
}

void LayerAvgActSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  dt = 1.0f / tau;
}

void LayGpInhibSpec::Initialize() {
  on = false;
  gp_g = 0.5f;
}

void LayGpInhibSpec::Defaults_init() {
  
}

void LeabraClampSpec::Initialize() {
  hard = true;
  Defaults_init();
}

void LeabraClampSpec::Defaults_init() {
  gain = .2f;
}

void LayerDecaySpec::Initialize() {
  event = 1.0f;

  Defaults_init();
}

void LayerDecaySpec::Defaults_init() {
  phase = 0.0f;
  cos_diff_avg_tau = 100.0f;

  cos_diff_avg_dt = 1.0f / cos_diff_avg_tau;
}

void LayerDecaySpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cos_diff_avg_dt = 1.0f / cos_diff_avg_tau;
}

void LayerDecaySpec::UpdtDiffAvg(float& diff_avg, const float cos_diff) {
  if(diff_avg == 0.0f) {        // first time -- set
    diff_avg = cos_diff;
  }
  else {
    diff_avg += cos_diff_avg_dt * (cos_diff - diff_avg);
  }
}


void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  unit_gp_inhib.on = false;
}

void LeabraLayerSpec::Defaults_init() {
  // nothing at this level..
}

void LeabraLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  lay_inhib.UpdateAfterEdit_NoGui();
  unit_gp_inhib.UpdateAfterEdit_NoGui();
  avg_act.UpdateAfterEdit_NoGui();
}

void LeabraLayerSpec::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraLayerSpec);
  children.SetBaseType(&TA_LeabraLayerSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

bool LeabraLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = true;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->CheckError(!lay->projections.el_base->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
                "does not have LeabraPrjn projection base type!",
                "project must be updated and projections remade"))
    return false;

  return rval;
}

void LeabraLayerSpec::HelpConfig() {
  String help = "LeabraLayerSpec Configuration:\n\
The layer spec sets the inhibitory parameters for each layer, along with \
the expected initial average activation per layer, which is used for \
netinput rescaling.  Therefore, you must have a different layer spec \
with an appropriate activity level for layers that have different activity levels.";
  taMisc::Confirm(help);
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  FOREACH_ELEM_IN_GROUP(Projection, p, lay->projections) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    if(fmlay->spec.SPtr()->InheritsFrom(layer_spec)) {  // inherits - not excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  FOREACH_ELEM_IN_GROUP(Projection, p, lay->projections) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from.ptr();
    if(fmlay->spec.SPtr()->GetTypeDef() == layer_spec) {        // not inherits - excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecNet(Network* net, TypeDef* layer_spec) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr()->InheritsFrom(layer_spec)) {    // inherits - not excact match!
      return lay;
    }
  }
  return NULL;
}


///////////////////////////////////////////////////////////////////////
//      General Init functions


void LeabraLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  lay->Layer::BuildUnits_Threads(net); // call default
  // now can be extended in derived classes..
}

void LeabraLayer::CheckInhibCons(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, projections) {
    p->CheckInhibCons(net);
  }
}

void LeabraLayerSpec::Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m_avg = avg_act.init;
  lay->acts_p_avg = avg_act.init;
  if(avg_act.fixed) {
    lay->acts_m_avg_eff = avg_act.init;
  }
  else {
    lay->acts_m_avg_eff = avg_act.adjust * lay->acts_m_avg;
  }
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Init_State();
      gpd->acts_m_avg = avg_act.init;
      gpd->acts_p_avg = avg_act.init;
      if(avg_act.fixed) {
        gpd->acts_m_avg_eff = avg_act.init;
      }
      else {
        gpd->acts_m_avg_eff = avg_act.adjust * gpd->acts_m_avg;
      }
    }
  }
  Init_Inhib(lay, net);         // initialize inhibition at start..
  Init_Stats(lay, net);
}

void LeabraLayerSpec::Init_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
}

void LeabraLayerSpec::Init_Stats(LeabraLayer* lay, LeabraNetwork* net) {
  lay->minus_output_name = "";
  lay->avg_netin.avg = 0.0f;
  lay->avg_netin.max = 0.0f;

  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;

  lay->norm_err = 0.0f;
  lay->cos_err = 0.0f;
  lay->cos_err_prv = 0.0f;
  lay->cos_err_vs_prv = 0.0f;

  lay->cos_diff = 0.0f;
  lay->cos_diff_avg = 0.0f;
  lay->cos_diff_avg_lmix = 0.0f;
  lay->avg_act_diff = 0.0f;
  lay->trial_cos_diff = 0.0f;

  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    prjn->Init_Stats();
  }
}

void LeabraLayerSpec::Init_ActAvg(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    u->Init_ActAvg(net);
  }
}

void LeabraLayerSpec::Init_Netins(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    u->Init_Netins(net);
  }
}

void LeabraLayerSpec::Init_Acts(LeabraLayer* lay, LeabraNetwork* net) {
  lay->ext_flag = Unit::NO_EXTERNAL;
  lay->hard_clamped = false;
  if(lay->units.leaves == 0) return; // may not be built yet!

  lay->Inhib_Init_Acts(this);
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Inhib_Init_Acts(this);
    }
  }
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Init_Acts(net);
  }
}

void LeabraLayerSpec::DecayState(LeabraLayer* lay, LeabraNetwork* net, float decay) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->DecayState(net, decay);
  }
}

///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraLayerSpec::Trial_Init_Specs(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->unit_spec.SPtr()) {
    ((LeabraUnitSpec*)lay->unit_spec.SPtr())->Trial_Init_Specs(net);
  }

  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, lay->projections) {
    p->Trial_Init_Specs(net);
  }

  if(lay_gp_inhib.on) 
    net->net_misc.lay_gp_inhib = true;
}

void LeabraLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(decay.event > 0.0f) {
    lay->i_val.ffi -= decay.event * lay->i_val.ffi;
    lay->i_val.fbi -= decay.event * lay->i_val.fbi;
    if(lay->unit_groups) {
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
        gpd->i_val.ffi -= decay.event * gpd->i_val.ffi;
        gpd->i_val.fbi -= decay.event * gpd->i_val.fbi;
      }
    }
  }
}

// NOTE: the following are not typically used, as the Trial_Init_Units calls directly
// to the unit level -- if anything is added to the layer-level then a _Layers call is
// needed instead

void LeabraLayerSpec::Trial_DecayState(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Trial_DecayState(net);
  }
}

void LeabraLayerSpec::Trial_Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Trial_Init_SRAvg(net);
  }
}

///////////////////////////////////////////////////////////////////////
//      SettleInit -- at start of settling

void LeabraLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  Settle_Init_TargFlags_Layer(lay, net);
}

void LeabraLayerSpec::Settle_Init_TargFlags(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Settle_Init_TargFlags(net);
  }

  Settle_Init_TargFlags_Layer(lay, net);
}

void LeabraLayerSpec::Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->HasExtFlag(Unit::TARG)) {     // only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      lay->SetExtFlag(Unit::EXT);
  }
}

void LeabraLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(clamp.hard && lay->HasExtFlag(Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }
  lay->hard_clamped = true;     // cache this flag
  lay->Inhib_SetVals(0.5f);            // assume 0 - 1 clamped inputs

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Compute_HardClamp(net);
  }
  Compute_CycleStats(lay, net); // compute once only
}

void LeabraLayerSpec::ExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(Unit::EXT))       // only process ext
    return;
  lay->ext_flag = Unit::COMP;   // totally reset to comparison

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->ExtToComp(net);
  }
}

void LeabraLayerSpec::TargExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(Unit::TARG_EXT))  // only process w/ external input
    return;
  lay->ext_flag = Unit::COMP;   // totally reset to comparison

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->TargExtToComp(net);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition -- netin stats

void LeabraLayerSpec::Compute_NetinStats_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraInhib* thr,  LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int sti = LayerStatsStartUnitIdx();
  thr->netin.InitVals();
  for(int i=sti; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    thr->netin.UpdtVals(u->net, i);
  }
  thr->netin.CalcAvg(nunits-sti);
}

void LeabraLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  if(HasUnitGpInhib(lay)) {
    lay->netin.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_NetinStats_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      lay->netin.UpdtFmAvgMax(gpd->netin, nunits-sti, g);
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    lay->netin.CalcAvg(lvs_eff);
  }
  else {
    Compute_NetinStats_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition, Basic computation

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  if(lay->hard_clamped) return; // say no more..

  Compute_NetinStats(lay, net); // for all cases, do this!

  if(HasUnitGpInhib(lay)) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_Inhib_impl(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net, unit_gp_inhib);
    }
  }
  if(HasLayerInhib(lay)) {
    Compute_Inhib_impl(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net, lay_inhib);
  }

  Compute_LayInhibToGps(lay, net); // sync it all up..
}

void LeabraLayerSpec::Compute_Inhib_impl
(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  Compute_Inhib_FfFb(lay, acc_md, gpidx, thr, net, ispec); // only one option right now..
  thr->i_val.g_i_orig = thr->i_val.g_i; // retain original values..
}

void LeabraLayerSpec::Compute_Inhib_FfFb
(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
 LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {

  if(!ispec.on) {
    thr->i_val.ffi = 0.0f;
    thr->i_val.fbi = 0.0f;
    thr->i_val.g_i = 0.0f;
    return;
  }

  float nw_ffi = ispec.FFInhib(thr->netin.avg);
  float nw_fbi = ispec.FBInhib(thr->acts.avg);

  thr->i_val.ffi = nw_ffi;

  // dt only on fbi part
  if(nw_fbi > thr->i_val.fbi) { // up_immed case --- useful for all
    thr->i_val.fbi = nw_fbi;
  }
  else {
    thr->i_val.fbi += ispec.fb_dt * (nw_fbi - thr->i_val.fbi);
  }

  thr->i_val.g_i = ispec.gi * (nw_ffi + thr->i_val.fbi); // combine
}

////////////////////////////////////////////////////////////////////////////////////
//      Inhibition Stage 2.2: LayInhibToGps

void LeabraLayerSpec::Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->unit_groups) return;

  if(HasUnitGpInhib(lay)) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.lay_g_i = lay->i_val.g_i;
      gpd->i_val.g_i = MAX(gpd->i_val.g_i, lay->i_val.g_i);
    }
  }
  else {
    // propagate layer-level g_i to all subgroups 
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i = lay->i_val.g_i;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, 
                                         int thread_no) {
  // note: Compute_OutputName now done in a network-level post step
  Compute_Acts_AvgMax(lay, net);

  if(lay->un_g_i.cmpt)
    Compute_UnitInhib_AvgMax(lay, net);

  if(lay->HasExtFlag(Unit::TARG)) {
    net->trg_max_act = MAX(net->trg_max_act, lay->acts_eq.max);
  }

  if(lay->Iconified()) {
    lay->icon_value = lay->acts_eq.avg;
  }
}

void LeabraLayerSpec::Compute_AvgMaxVals_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             AvgMaxVals& vals, ta_memb_ptr mb_off) {
  vals.InitVals();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int sti = LayerStatsStartUnitIdx();
  for(int i=sti; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float val = *((float*)MemberDef::GetOff_static((void*)u, 0, mb_off));
    vals.UpdtVals(val, i);
  }
  vals.CalcAvg(nunits-sti);
}

void LeabraLayerSpec::Compute_AvgMaxActs_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraInhib* thr) {
  thr->acts.InitVals();
  thr->acts_eq.InitVals();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int sti = LayerStatsStartUnitIdx();
  for(int i=sti; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    thr->acts.UpdtVals(u->act, i);
    thr->acts_eq.UpdtVals(u->act_eq, i);
  }
  thr->acts.CalcAvg(nunits-sti);
  thr->acts_eq.CalcAvg(nunits-sti);
}

void LeabraLayerSpec::Compute_Acts_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts;
  AvgMaxVals& vals_eq = lay->acts_eq;
//   static ta_memb_ptr mb_off = 0;
//   if(mb_off == 0) {
//     TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
//     TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_eq");
//   }
  if(lay->unit_groups) {
    vals.InitVals();
    vals_eq.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxActs_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd);
      vals.UpdtFmAvgMax(gpd->acts, nunits-sti, g);
      vals_eq.UpdtFmAvgMax(gpd->acts_eq, nunits-sti, g);
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    vals.CalcAvg(lvs_eff);
    vals_eq.CalcAvg(lvs_eff);
  }
  else {
    Compute_AvgMaxActs_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay);
  }
}

void LeabraLayerSpec::Compute_OutputName_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraInhib* thr, LeabraNetwork* net) {
  String* onm;
  if(lay->unit_groups)
    onm = &(lay->gp_output_names.FastEl_Flat(gpidx));
  else
    onm = &(lay->output_name);

  if(thr->acts_eq.max_i < 0) {
    *onm = "n/a";
    return;
  }
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, thr->acts_eq.max_i, gpidx);
  if(!u || u->lesioned()) {
    *onm = "n/a";
    return;
  }
  // for target/output layers, if we set something, set network name!
  if(u->name.empty()) return;
  *onm = u->name;       // if it is something..

  if(lay->unit_groups) {        // also aggregate the layer name
    if(lay->output_name.nonempty())
      lay->output_name += "_";
    lay->output_name += u->name;
  }

  if((lay->layer_type != Layer::OUTPUT) && (lay->layer_type != Layer::TARGET)) return;
  if(net->output_name.nonempty())
    net->output_name += "_";
  net->output_name += u->name;
}

void LeabraLayerSpec::Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->unit_groups) {
    lay->output_name = "";
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_OutputName_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  else {
    Compute_OutputName_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_UnitInhib_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->un_g_i;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "gc_i");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->un_g_i, mb_off);
      vals.UpdtFmAvgMax(gpd->un_g_i, nunits-sti, g);
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    vals.CalcAvg(lvs_eff);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Optional Misc

void LeabraLayerSpec::Compute_MidMinus(LeabraLayer* lay, LeabraNetwork* net) {
  // just snapshot the activation state
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Compute_MidMinus(net);
  }
}

///////////////////////////////////////////////////////////////////////
//      SettleFinal

void LeabraLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  bool no_plus_testing = net->IsNoPlusTesting();

  if(net->phases.minus > 0) {
    if(no_plus_testing) {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPlus(lay, net);
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE)
        PostSettle_GetMinus(lay, net);
      else {
        PostSettle_GetPlus(lay, net);
      }
    }
  }
  else {                        // plus only
    PostSettle_GetMinus(lay, net);
    PostSettle_GetPlus(lay, net);
  }

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->PostSettle(net);
  }
}

void LeabraLayerSpec::PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m = lay->acts_eq;
  lay->acts_m_avg += avg_act.dt * (lay->acts_m.avg - lay->acts_m_avg);
  if(avg_act.fixed) {
    lay->acts_m_avg_eff = avg_act.init;
  }
  else {
    lay->acts_m_avg_eff = avg_act.adjust * lay->acts_m_avg;
  }
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_m = gpd->acts_eq;
      gpd->acts_m_avg += avg_act.dt * (gpd->acts_m.avg - gpd->acts_m_avg);
      if(avg_act.fixed) {
        gpd->acts_m_avg_eff = avg_act.fixed;
      }
      else {
        gpd->acts_m_avg_eff = avg_act.adjust * gpd->acts_m_avg;
      }
    }
  }
}

void LeabraLayerSpec::PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p = lay->acts_eq;
  lay->acts_p_avg += avg_act.dt * (lay->acts_p.avg - lay->acts_p_avg); 
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_p = gpd->acts_eq;
      gpd->acts_p_avg += avg_act.dt * (gpd->acts_p.avg - gpd->acts_p_avg);
    }
  }
}

/////////////////////////////////////////////////////////
// NOTE: the following are not currently used..

void LeabraLayerSpec::Compute_ActM_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts_m;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_m");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_m, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_m, nunits-sti, g);
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    vals.CalcAvg(lvs_eff);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

void LeabraLayerSpec::Compute_ActP_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts_p;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_p");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_p, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_p, nunits-sti, g);
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    vals.CalcAvg(lvs_eff);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

void LeabraLayerSpec::Compute_ActCtxt_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts_ctxt;
  static ta_memb_ptr mb_off = 0;
  if(mb_off == 0) {
    TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_ctxt");
  }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxVals_ugp(lay, Layer::ACC_GP, g, gpd->acts_ctxt, mb_off);
      vals.UpdtFmAvgMax(gpd->acts_ctxt, nunits-sti, g);
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    vals.CalcAvg(lvs_eff);
  }
  else {
    Compute_AvgMaxVals_ugp(lay, Layer::ACC_LAY, 0, vals, mb_off);
  }
}

///////////////////////////////////////////////////////////////////////
//      TI

void LeabraLayerSpec::TI_Compute_Deep5bAct(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->TI_Compute_Deep5bAct(net);
  }
  //  Compute_Deep_AvgMax(lay, net);
}

void LeabraLayerSpec::TI_Compute_CtxtAct(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->TI_Compute_CtxtAct(net);
  }
  Compute_ActCtxt_AvgMax(lay, net);
}

void LeabraLayerSpec::TI_ClearContext(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->TI_ClearContext(net);
  }
}

///////////////////////////////////////////////////////////////////////
//      TrialFinal

// todo: could probably package these at unit level...

void LeabraLayerSpec::EncodeState(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->EncodeState(net);
  }
}

void LeabraLayerSpec::Compute_SelfReg_Trial(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Compute_SelfReg_Trial(net);
  }
}


///////////////////////////////////////////////////////////////////////
//      Learning

bool LeabraLayerSpec::Compute_dWt_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return true;
}

void LeabraLayerSpec::SetUnitLearnFlags(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->SetLearnFlag();
  }
}


///////////////////////////////////////////////////////////////////////
//      Trial-level Stats

void LeabraLayerSpec::LayerAvgAct(DataTable* report_table) {
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(!net) return;
  net->LayerAvgAct(report_table, this);
}


float LeabraLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork* net,
                                   int& n_vals, bool unit_avg, bool sqrt) {
  return lay->Layer::Compute_SSE(net, n_vals, unit_avg, sqrt);
}

float LeabraLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraInhib* thr, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  float nerr = 0.0f;
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    nerr += u->Compute_NormErr(net);
  }
  return nerr;
}

float LeabraLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!lay->HasExtFlag(Unit::TARG | Unit::COMP)) return -1.0f; // indicates not applicable


  float nerr = 0.0f;
  int ntot = 0;
  if(HasUnitGpInhib(lay)) {
    int gp_nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      if(net->lstats.on_errs && net->lstats.off_errs)
        ntot += 2 * (int)(gpd->acts_m_avg * (float)gp_nunits);
      else
        ntot += (int)(gpd->acts_m_avg * (float)gp_nunits);
    }
  }
  else {
    int lay_nunits = lay->UnitAccess_NUnits(Layer::ACC_LAY);
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    if(net->lstats.on_errs && net->lstats.off_errs)
      ntot += 2 * (int)(lay->acts_m_avg * (float)lay_nunits);
    else
      ntot += (int)(lay->acts_m_avg * (float)lay_nunits);
  }
  if(ntot == 0) return -1.0f;

  lay->norm_err = nerr / (float)ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

float LeabraLayerSpec::Compute_CosErr(LeabraLayer* lay, LeabraNetwork* net,
				     int& n_vals) {
  lay->cos_err = 0.0f;
  lay->cos_err_prv = 0.0f;
  lay->cos_err_vs_prv = 0.0f;
  n_vals = 0;
  if(!lay->HasExtFlag(Unit::TARG | Unit::COMP)) return 0.0f;
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    return 0.0f;
  }
  float cosv = 0.0f;
  float cosvp = 0.0f;
  float ssm = 0.0f;
  float ssp = 0.0f;
  float sst = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    //    if(!u->HasExtFlag(Unit::TARG | Unit::COMP)) continue;
    n_vals++;
    cosv += u->targ * u->act_m;
    ssm += u->act_m * u->act_m;
    sst += u->targ * u->targ;
    if(net->net_misc.ti) {
      cosvp += u->targ * u->p_act_p;
      ssp += u->p_act_p * u->p_act_p;
    }
  }
  if(n_vals == 0) return 0.0f;
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->cos_err = cosv;
  if(net->net_misc.ti) {
    float pdist = sqrtf(ssp * sst);
    if(pdist != 0.0f) {
      cosvp /= pdist;
    }
    lay->cos_err_prv = cosvp;
    lay->cos_err_vs_prv = lay->cos_err - lay->cos_err_prv;
  }
  return cosv;
}

float LeabraLayerSpec::Compute_CosDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->cos_diff = 0.0f;
  float cosv = 0.0f;
  float ssm = 0.0f;
  float sst = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    cosv += u->act_p * u->act_m;
    ssm += u->act_m * u->act_m;
    sst += u->act_p * u->act_p;
  }
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->cos_diff = cosv;

  decay.UpdtDiffAvg(lay->cos_diff_avg, lay->cos_diff);
  if(lay->layer_type == Layer::HIDDEN) {
    lay->cos_diff_avg_lmix = 1.0f - lay->cos_diff_avg;
  }
  else {
    lay->cos_diff_avg_lmix = 0.0f; // no mix for TARGET layers; irrelevant for INPUT
  }

  return cosv;
}

float LeabraLayerSpec::Compute_AvgActDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_act_diff = 0.0f;
  float adiff = 0.0f;
  int nd = 0;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    adiff += u->act_p - u->act_m;
    nd++;
  }
  if(nd > 0)
    adiff /= (float)nd;
  lay->avg_act_diff = adiff;
  return adiff;
}

float LeabraLayerSpec::Compute_TrialCosDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->trial_cos_diff = 0.0f;
  float cosv = 0.0f;
  float ssm = 0.0f;
  float sst = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    cosv += u->act_p * u->p_act_p;
    ssm += u->p_act_p * u->p_act_p;
    sst += u->act_p * u->act_p;
  }
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->trial_cos_diff = cosv;

  return cosv;
}


void LeabraLayerSpec::Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->netin.max < 0.01f) return; // not getting enough activation to count!

  // layer is automatic
  lay->avg_netin_sum.avg += lay->netin.avg;
  lay->avg_netin_sum.max += lay->netin.max;
  lay->avg_netin_n++;

  // but projection level is not
  if(net->NetinPerPrjn() || net->rel_netin.ComputeNow(net->epoch, net->trial)) {
    float sum_net = 0.0f;
    for(int i=0;i<lay->projections.size;i++) {
      LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
      if(prjn->NotActive()) continue;
      prjn->netin_avg = 0.0f;
      int netin_avg_n = 0;
      FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
        if(u->lesioned()) continue;
        LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
        if(u->act_eq < us->opt_thresh.send) continue; // ignore if not above sending thr
        LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(prjn->recv_idx);
        if(!cg) continue;
        float netin;
        if(net->NetinPerPrjn()) {
          netin = cg->net_raw;
        }
        else {
          netin = cg->Compute_Netin(u,net); // otherwise have to compute it
        }
        cg->net = netin;
        prjn->netin_avg += netin;
        netin_avg_n++;
      }
      if(netin_avg_n > 0)
        prjn->netin_avg /= (float)netin_avg_n;
      sum_net += prjn->netin_avg;
    }

    for(int i=0;i<lay->projections.size;i++) {
      LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
      if(sum_net > 0.0f)
        prjn->netin_rel = prjn->netin_avg / sum_net;
      // increment epoch-level
      prjn->avg_netin_avg_sum += prjn->netin_avg;
      prjn->avg_netin_rel_sum += prjn->netin_rel;
      prjn->avg_netin_n++;
    }
  }
}

void LeabraLayerSpec::Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
#ifdef DMEM_COMPILE
  lay->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
  if(lay->avg_netin_n > 0) {
    lay->avg_netin.avg = lay->avg_netin_sum.avg / (float)lay->avg_netin_n;
    lay->avg_netin.max = lay->avg_netin_sum.max / (float)lay->avg_netin_n;
  }
  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->NotActive()) continue;
#ifdef DMEM_COMPILE
    prjn->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
    if(prjn->avg_netin_n > 0) {
      prjn->avg_netin_avg = prjn->avg_netin_avg_sum / (float)prjn->avg_netin_n;
      prjn->avg_netin_rel = prjn->avg_netin_rel_sum / (float)prjn->avg_netin_n;
    }
    prjn->avg_netin_n = 0;
    prjn->avg_netin_avg_sum = 0.0f;
    prjn->avg_netin_rel_sum = 0.0f;
  }
}

