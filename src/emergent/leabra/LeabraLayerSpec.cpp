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

eTypeDef_Of(MarkerConSpec);

void LeabraInhibSpec::Initialize() {
  type = KWTA_AVG_INHIB;
  gi = 2.0f;
  ff = 1.0f;
  ff0 = 0.1f;
  fb = 0.5f;
  fbx = 0.0f;
  infl = 0.3f;
  dt = 0.7f;
  min_i = 0.0f;
  Defaults_init();
}

void LeabraInhibSpec::Defaults_init() {
  switch(type) {
  case KWTA_INHIB:
    kwta_pt = .2f;
    break;
  case KWTA_AVG_INHIB:
    kwta_pt = .5f;
    break;
  default:
    break;
  }
}

void KWTASpec::Initialize() {
  k_from = USE_PCT;
  k = 12;
  pct = .25f;
  pat_q = .2f;
  diff_act_pct = false;
  act_pct = .1f;

  // note: legacy obsolete remove later (9/19/12, v.6.0.2)
  gp_i = false;
  gp_g = 0.0f;
}

void KWTASpec::Defaults_init() {
  pat_q = .5f;
}

void GpInhibSpec::Initialize() {
  on = false;
  fffb = false;
  lay_gi = 2.0f;
  gp_g = 0.5f;
  diff_act_pct = false;
  pct_fm_frac = true;
  act_denom = 3.0f;
  if(pct_fm_frac)
    act_pct_mult = 1.0f / act_denom;
}

void GpInhibSpec::Defaults_init() {
  // no sensible defaults
}

void GpInhibSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(pct_fm_frac)
    act_pct_mult = 1.0f / act_denom;
}
  

void KwtaTieBreak::Initialize() {
  on = false;
  Defaults_init();
}

void KwtaTieBreak::Defaults_init() {
  k_thr = 1.0f;
  diff_thr = 0.2f;
  thr_gain = 0.005f;
  loser_gain = 1.0f;
}

void AdaptISpec::Initialize() {
  type = NONE;
  Defaults_init();
}

void AdaptISpec::Defaults_init() {
  tol = .02f;                   // allow to be off by this amount before taking action
  p_dt = .1f;                   // take reasonably quick action..
  mx_d = .9f;                   // move this far in either direction
  l = .2f;                      // proportion to assign to leak..
  a_dt = .005f;                 // time averaging
}

void ClampSpec::Initialize() {
  hard = true;
  Defaults_init();
}

void ClampSpec::Defaults_init() {
  max_plus = false;             // was true -- but may not work in some cases -- false safer?
  plus = 0.01f;
  min_clamp = 0.5f;
  if(taMisc::is_loading) {
    taVersion v511(5, 1, 1);
    if(taMisc::loading_version < v511) { // default prior to 511 is max-plus off
      max_plus = false;
    }
  }
  gain = .2f;
  minus_targ_gain = 0.0f;
}

void DecaySpec::Initialize() {
  event = 1.0f;
  phase = 1.0f;
  phase2 = 0.0f;
}

void CosDiffLrateSpec::Initialize() {
  on = false;
  lo_diff = 0.0f;
  lo_lrate = 0.01f;
  hi_diff = 1.0f;
  hi_lrate = 0.01f;
  avg_dt = 0.002f;
}

void CosDiffLrateSpec::Defaults_init() {
}

float CosDiffLrateSpec::LrateMod(const float diff_avg, const float cos_diff) {
  if(cos_diff <= lo_diff) return lo_lrate;
  if(cos_diff >= hi_diff) return hi_lrate;
  if(cos_diff < diff_avg) {
    float lrm = 1.0f - ((diff_avg - cos_diff) / (diff_avg - lo_diff));  // avg..lo -> 1..0
    lrm = lo_lrate + (1.0f - lo_lrate) * lrm;
    return lrm;
  }
  else {
    float lrm = 1.0f - ((cos_diff - diff_avg) / (hi_diff - diff_avg));  // avg..hi -> 1..0
    lrm = hi_lrate + (1.0f - hi_lrate) * lrm;
    return lrm;
  }
}

void CosDiffLrateSpec::UpdtDiffAvg(float& diff_avg, const float cos_diff) {
  float eff_diff = cos_diff;
  float drange = hi_diff - lo_diff;
  float margin = 0.01f * drange;
  if(cos_diff <= (lo_diff + margin)) eff_diff = (lo_diff + margin);
  if(cos_diff >= (hi_diff - margin)) eff_diff = (hi_diff - margin);

  if(diff_avg == 0.0f) {        // first time -- set
    diff_avg = eff_diff;
    return;
  }
  
  diff_avg += avg_dt * (eff_diff - diff_avg);
}


void CtLayerInhibMod::Initialize() {
  manual_sravg = false;
  sravg_delay = 0;
  use_sin = false;
  burst_i = 0.02f;
  trough_i = 0.02f;
  use_fin = false;
  inhib_i = 0.0f;
}

void LayAbsNetAdaptSpec::Initialize() {
  on = false;
  Defaults_init();
}

void LayAbsNetAdaptSpec::Defaults_init() {
  trg_net = .5f;
  tol = .1f;
  abs_lrate = .2f;
}

void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  inhib_group = ENTIRE_LAYER;
}

void LeabraLayerSpec::Defaults_init() {
  // nothing at this level..
}

void LeabraLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  unit_gp_inhib.UpdateAfterEdit_NoGui();

  if(kwta.gp_i) {		// update from obsolete
    SetUnique("lay_gp_inhib", true);
    lay_gp_inhib.on = true;
    lay_gp_inhib.gp_g = kwta.gp_g;
    kwta.gp_i = false;
    kwta.gp_g = 0.0f;
  }

  if(gp_kwta.gp_i) {		// update from obsolete
    SetUnique("unit_gp_inhib", true);
    unit_gp_inhib.on = true;
    unit_gp_inhib.gp_g = gp_kwta.gp_g;
    gp_kwta.gp_i = false;
    gp_kwta.gp_g = 0.0f;
  }
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
  if(net && net->learn_rule != LeabraNetwork::LEABRA_CHL) {
    if(TestWarning(decay.phase == 1.0f,
                   "LeabraLayerSpec decay.phase should be 0 or small for for CTLEABRA_X/CAL -- I just set it to 0 for you in spec:", name)) {
      SetUnique("decay", true);
      decay.phase = 0.0f;
      decay.phase2 = 0.0f;
    }
  }

  if(lay->CheckError(!lay->projections.el_base->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
                "does not have LeabraPrjn projection base type!",
                "project must be updated and projections remade"))
    return false;
  bool has_rel_net_conspec = false;
  for(int i=0;i<lay->projections.size;i++) {
    Projection* prjn = (Projection*)lay->projections[i];
    lay->CheckError(!prjn->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
               "does not have LeabraPrjn projection base type!",
               "Projection must be re-made");
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.GetSpec();
    if(cs && cs->rel_net_adapt.on) has_rel_net_conspec = true;
  }
  if(has_rel_net_conspec) {
    // check for total trg_netin_rel
    float sum_trg_netin_rel = 0.0f;
    for(int i=0;i<lay->projections.size;i++) {
      LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
      sum_trg_netin_rel += prjn->trg_netin_rel;
    }
    lay->CheckError((fabsf(sum_trg_netin_rel - 1.0f) > .001f), quiet, rval,
                    "sum of trg_netin_rel values for layer:",String(sum_trg_netin_rel),
                    "!= 1.0 -- must fix!");
  }

  lay->CheckError((inhib_group == UNIT_GROUPS && !lay->unit_groups), quiet, rval,
                  "inhib_group is UNIT_GROUPS but layer is not set to use unit_groups!  will not work.");

  return rval;
}

void LeabraLayerSpec::HelpConfig() {
  String help = "LeabraLayerSpec Configuration:\n\
The layer spec sets the target level of activity, k, for each layer.  \
Therefore, you must have a different layer spec with an appropriate activity \
level for layers that have different activity levels.  Note that if you set \
the activity level by percent this will work for different sized layers that \
have the same percent activity level.";
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
  lay->BuildKwtaBuffs();               // also make sure kwta buffs are rebuilt!
}

void LeabraLayer::CheckInhibCons(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, projections) {
    p->CheckInhibCons(net);
  }
}

void LeabraLayerSpec::SetLearnRule(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) {
    decay.phase = 1.0f;         // all phase decay
  }
  else {
    decay.phase = 0.0f;         // no phase decay -- these are not even called
    decay.phase2 = 0.0f;
  }

  if(lay->unit_spec.SPtr()) {
    ((LeabraUnitSpec*)lay->unit_spec.SPtr())->SetLearnRule(net);
  }

  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, lay->projections) {
    p->SetLearnRule(net);
  }
}

void LeabraLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  lay->BuildKwtaBuffs();        // make sure kwta buffs are rebuilt!
  Compute_Active_K(lay, net);   // need kwta.pct for init
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    u->Init_Weights(net);
  }
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Init_State();
    }
  }
  Init_Inhib(lay, net);         // initialize inhibition at start..
  Init_Stats(lay, net);
  lay->sravg_vals.InitVals();
}

void LeabraLayerSpec::Init_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  lay->adapt_i.avg_avg = lay->kwta.pct;
  lay->adapt_i.i_kwta_pt = inhib.kwta_pt;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  lay->adapt_i.g_bar_i = us->g_bar.i;
  lay->adapt_i.g_bar_l = us->g_bar.l;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->adapt_i.avg_avg = gpd->kwta.pct;
      gpd->adapt_i.i_kwta_pt = inhib.kwta_pt;
      gpd->adapt_i.g_bar_i = us->g_bar.i;
      gpd->adapt_i.g_bar_l = us->g_bar.l;
    }
  }
}

void LeabraLayerSpec::Init_Stats(LeabraLayer* lay, LeabraNetwork* net) {
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
  lay->cos_diff_lrate = 1.0f;

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
  lay->ResetSortBuf();
  if(lay->units.leaves == 0) return; // may not be built yet!

  Compute_Active_K(lay, net);   // need kwta.pct for init
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

void LeabraLayerSpec::SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch) {
  if(lay->unit_spec.SPtr()) {
    ((LeabraUnitSpec*)lay->unit_spec.SPtr())->SetCurLrate(net, epoch);
  }
  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, lay->projections) {
    p->SetCurLrate(net, epoch);
  }
  if(net->cos_diff_survey) cos_diff_lrate.on = true; // turn it on everywhere
  else if(cos_diff_lrate.on) net->cos_diff_survey = true;
}

void LeabraLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  lay->sravg_vals.InitVals();
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

void LeabraLayerSpec::Trial_NoiseInit_KPos_ugp(LeabraLayer* lay,
                                               Layer::AccessMode acc_md, int gpidx,
                                               LeabraInhib* thr, LeabraNetwork* net) {
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(lay->unit_idxs.size != nunits) {
    lay->unit_idxs.SetSize(nunits);
    lay->unit_idxs.FillSeq();
  }
  lay->unit_idxs.Permute();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, lay->unit_idxs[i], gpidx);
    if(u->lesioned()) continue;
    if(i < thr->kwta.k)
      u->noise = us->noise.var;
    else
      u->noise = 0.0f;
  }
}


void LeabraLayerSpec::Trial_NoiseInit(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(!us->noise_adapt.trial_fixed) return;

  if(us->noise_adapt.k_pos_noise) {
    if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
        Trial_NoiseInit_KPos_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      }
    }
    else {
      Trial_NoiseInit_KPos_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    }
  }
  // don't do this b/c already done at unit level for this case..
//   else {
//     FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
//      if(u->lesioned()) continue;
//       u->Trial_NoiseInit(net);
//     }
//   }
}

void LeabraLayerSpec::Trial_Init_SRAvg(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->Trial_Init_SRAvg(net);
  }
}

///////////////////////////////////////////////////////////////////////
//      SettleInit -- at start of settling

void LeabraLayerSpec::Compute_Active_K(LeabraLayer* lay, LeabraNetwork* net) {
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    int totk = 0;
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_Active_K_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, gp_kwta);
      totk += gpd->kwta.k;
    }
    if(inhib_group == UNIT_GROUPS) {
      if(lay->kwta.k != totk) {
        lay->lay_kbuffs.ResetAllBuffs();
        lay->Inhib_ResetSortBuf();
      }
      lay->kwta.k = totk;
      lay->kwta.Compute_Pct(lay->units.leaves);
      if(gp_kwta.diff_act_pct)
        lay->kwta.pct = gp_kwta.act_pct;        // override!!
      if(unit_gp_inhib.on && unit_gp_inhib.diff_act_pct)
        lay->kwta.pct *= unit_gp_inhib.act_pct_mult;
    }
  }
  if(inhib_group != UNIT_GROUPS) {
    Compute_Active_K_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, kwta);
    if(kwta.diff_act_pct)
      lay->kwta.pct = kwta.act_pct;     // override!!
  }
  if(lay_gp_inhib.on && lay_gp_inhib.diff_act_pct)
    lay->kwta.pct *= lay_gp_inhib.act_pct_mult;
}

void LeabraLayerSpec::Compute_Active_K_ugp(LeabraLayer* lay,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraInhib* thr, KWTASpec& kwtspec)
{
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int new_k = 0;
  if(kwtspec.k_from == KWTASpec::USE_PCT)
    new_k = (int)(kwtspec.pct * (float)nunits);
  else if((kwtspec.k_from == KWTASpec::USE_PAT_K) &&
          (lay->HasExtFlag(Unit::TARG | Unit::COMP | Unit::EXT)))
    new_k = Compute_Pat_K(lay, acc_md, gpidx, thr);
  else
    new_k = kwtspec.k;

  if(inhib.type == LeabraInhibSpec::KWTA_INHIB)
    new_k = MIN(nunits - 1, new_k);
  else
    new_k = MIN(nunits, new_k);
  new_k = MAX(1, new_k);

  if(thr->kwta.k != new_k) {
    KwtaSortBuff_List* srtbuff = lay->SortBuffList(acc_md);
    srtbuff->ResetAllBuffs();
    thr->Inhib_ResetSortBuf();
  }

  thr->kwta.k = new_k;
  thr->kwta.Compute_Pct(nunits);
}

int LeabraLayerSpec::Compute_Pat_K(LeabraLayer* lay,
                                   Layer::AccessMode acc_md, int gpidx,
                                   LeabraInhib* thr) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  bool use_comp = false;
  if(lay->HasExtFlag(Unit::COMP)) // only use comparison vals if entire lay is COMP!
    use_comp = true;
  int pat_k = 0;
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    // use either EXT or TARG information...
    if(u->HasExtFlag(Unit::EXT)) {
      if(u->ext >= kwta.pat_q)
        pat_k++;
    }
    else if(u->HasExtFlag(Unit::TARG)) {
      if(u->targ >= kwta.pat_q)
        pat_k++;
    }
    else if(use_comp && u->HasExtFlag(Unit::COMP)) {
      if(u->targ >= kwta.pat_q)
        pat_k++;
    }
  }
  return pat_k;
}

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
  lay->Inhib_SetVals(inhib.kwta_pt);            // assume 0 - 1 clamped inputs

  if(clamp.max_plus && net->phase == LeabraNetwork::PLUS_PHASE && net->phase_no > 0 &&
     lay->HasExtFlag(Unit::TARG)) {
    float min_max = lay->acts_m.max;
    float clmp = min_max + clamp.plus;
    clmp = MAX(clmp, clamp.min_clamp);

    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      if(u->lesioned()) continue;
      u->ext *= clmp;           // modify!
    }
  }

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
//      Cycle Step 1: Netinput

void LeabraLayerSpec::Compute_NetinStats_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraInhib* thr,  LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int sti = LayerStatsStartUnitIdx();
  thr->netin.InitVals();  thr->i_thrs.InitVals();
  for(int i=sti; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    thr->netin.UpdtVals(u->net, i);    thr->i_thrs.UpdtVals(u->i_thr, i);
  }
  thr->netin.CalcAvg(nunits-sti);  thr->i_thrs.CalcAvg(nunits-sti);

  if(thr->netin_top_k.cmpt) {
    thr->netin_top_k.InitVals();
    int k_eff = thr->kwta.k;    // keep cutoff at k
    KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
    if(k_eff <= 0 || act_buff->Size(gpidx) != k_eff)
      return; // no can do

    for(int j=0; j < k_eff; j++) {
      thr->netin_top_k.UpdtVals(act_buff->Un(j, gpidx)->net, j);
    }
    thr->netin_top_k.CalcAvg(k_eff);
  }
}

void LeabraLayerSpec::Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    lay->netin.InitVals();    lay->i_thrs.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_NetinStats_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      lay->netin.UpdtFmAvgMax(gpd->netin, nunits-sti, g);
      lay->i_thrs.UpdtFmAvgMax(gpd->i_thrs, nunits-sti, g);
      lay->netin_top_k.UpdtFmAvgMax(gpd->netin_top_k, 1, g); // only compute gp-wise avg for avg top k (n=1 per group)
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    lay->netin.CalcAvg(lvs_eff);
    lay->i_thrs.CalcAvg(lvs_eff);
    lay->netin_top_k.CalcAvg(lay->gp_geom.n);
  }
  else {
    Compute_NetinStats_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition, Basic computation

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return; // say no more..

  if(inhib_group != UNIT_GROUPS) {
    Compute_Inhib_impl(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net, inhib);
  }
  if(lay->unit_groups) {
    if(inhib_group == UNIT_GROUPS) {
      lay->Inhib_SetVals(0.0f);
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
        Compute_Inhib_impl(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net, inhib);
        float gp_g_i = gpd->i_val.g_i;
        if(unit_gp_inhib.on)
          gp_g_i *= unit_gp_inhib.gp_g;
        lay->i_val.g_i = MAX(lay->i_val.g_i, gp_g_i);
      }
    }
    else if(inhib_group == LAY_AND_GPS) {
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
        Compute_Inhib_impl(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net, inhib);
      }
    }
    Compute_LayInhibToGps(lay, net);
  }

  Compute_CtDynamicInhib(lay, net);
}

void LeabraLayerSpec::Compute_Inhib_impl(LeabraLayer* lay,
                         Layer::AccessMode acc_md, int gpidx,
                         LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  if(ispec.type == LeabraInhibSpec::UNIT_INHIB) {
    thr->i_val.g_i = 0.0f;      // make sure it's zero, cuz this gets added to units..
  }
  else {
    switch(ispec.type) {
    case LeabraInhibSpec::KWTA_INHIB:
      Compute_Inhib_kWTA(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::KWTA_AVG_INHIB:
      Compute_Inhib_kWTA_Avg(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::FF_FB_INHIB:
      Compute_Inhib_FfFb(lay, acc_md, gpidx, thr, net, ispec);
      break;
    case LeabraInhibSpec::UNIT_INHIB:
      break;
    }
    thr->i_val.g_i = thr->i_val.kwta;
  }

  thr->i_val.g_i_orig = thr->i_val.g_i; // retain original values..
}

// basic sorting function:

void LeabraLayerSpec::Compute_Inhib_kWTA_Sort(LeabraLayer* lay, Layer::AccessMode acc_md,
                                      int gpidx, int nunits, LeabraInhib* thr,
                                      KwtaSortBuff& act_buff, KwtaSortBuff& inact_buff,
                                      int& k_eff, float& k_net, int& k_idx) {
  LeabraUnit* u;
  int j;
  if(act_buff.Size(gpidx) != k_eff) { // need to fill the sort buf..
    act_buff.ResetGp(gpidx);
    int end_j = k_eff;
    for(j=0; j < end_j; j++) {
      u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
      if(u->lesioned()) {
	end_j++;
	if(end_j >= nunits) {
	  k_eff = act_buff.Size(gpidx); // this may not end well really..
	  break;
	}
	continue;
      }
      act_buff.Add(u, gpidx); // add to list
      if(u->i_thr < k_net) {
        k_net = u->i_thr;       k_idx = act_buff.Size(gpidx)-1;
      }
    }
    inact_buff.ResetGp(gpidx);
    // now, use the "replace-the-lowest" sorting technique
    for(; j < nunits; j++) {
      u = (LeabraUnit*)lay->UnitAccess(acc_md, j, gpidx);
      if(u->lesioned()) continue;
      if(u->i_thr <= k_net) {   // not bigger than smallest one in sort buffer
        inact_buff.Add(u, gpidx);
        continue;
      }
      inact_buff.Add(act_buff.Un(k_idx, gpidx), gpidx); // now inactive
      act_buff.Set(u, k_idx, gpidx); // replace the smallest with it
      k_net = u->i_thr;         // assume its the smallest
      for(int b=0; b < k_eff; b++) {    // and recompute the actual smallest
        float tmp = act_buff.Un(b, gpidx)->i_thr;
        if(tmp < k_net) {
          k_net = tmp;          k_idx = b;
        }
      }
    }
  }
  else {                                // keep the ones around from last time, find k_net
    for(j=0; j < k_eff; j++) {  // these should be the top ones, very fast!!
      float tmp = act_buff.Un(j, gpidx)->i_thr;
      if(tmp < k_net) {
        k_net = tmp;            k_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    int iabsz = inact_buff.Size(gpidx);
    for(j=0; j < iabsz; j++) {
      u = inact_buff.Un(j, gpidx);
      if(u->i_thr <= k_net)             // not bigger than smallest one in sort buffer
        continue;
      inact_buff.Set(act_buff.Un(k_idx, gpidx), j, gpidx);      // now inactive
      act_buff.Set(u, k_idx, gpidx); // replace the smallest with it
      k_net = u->i_thr;         // assume its the smallest
      for(int b=0; b < k_eff; b++) {    // and recompute the actual smallest
        float tmp = act_buff.Un(b, gpidx)->i_thr;
        if(tmp < k_net) {
          k_net = tmp;          k_idx = b;
        }
      }
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_BreakTie(LeabraInhib* thr) {
  if(thr->kwta.k_ithr > 0.0f)
    thr->kwta.ithr_diff = (thr->kwta.k_ithr - thr->kwta.k1_ithr) / thr->kwta.k_ithr;
  else
    thr->kwta.ithr_diff = 0.0f;
  thr->kwta.tie_brk = 0;
  if(tie_brk.on && (thr->kwta.k_ithr > tie_brk.k_thr)) {
    if(thr->kwta.ithr_diff < tie_brk.diff_thr) {
      // we now have an official tie: break it by reducing firing of "others"
      thr->kwta.tie_brk = 1;
      thr->kwta.tie_brk_gain = (tie_brk.diff_thr - thr->kwta.ithr_diff) / tie_brk.diff_thr;
      thr->kwta.k1_ithr *= (1.0f - thr->kwta.tie_brk_gain * tie_brk.thr_gain);
      thr->kwta.eff_loser_gain = 1.0f + (tie_brk.loser_gain * thr->kwta.tie_brk_gain);
    }
  }
}

// actual kwta impls:

void LeabraLayerSpec::Compute_Inhib_kWTA(LeabraLayer* lay,
                         Layer::AccessMode acc_md, int gpidx,
                         LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits <= 1) {     // this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k + 1;       // expand cutoff to include N+1th one
  k_plus_1 = MIN(nunits, k_plus_1);
  float k1_net = FLT_MAX;
  int k1_idx = 0;

  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  KwtaSortBuff* inact_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT);

  Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
                          *act_buff, *inact_buff, k_plus_1, k1_net, k1_idx);

  if(k_plus_1 <= 1) {		// something bad happened
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  // active_buf now has k+1 most active units, get the next-highest one
  int k_idx = -1;
  float net_k = FLT_MAX;
  for(int j=0; j < k_plus_1; j++) {
    float tmp = act_buff->Un(j, gpidx)->i_thr;
    if((tmp < net_k) && (j != k1_idx)) {
      net_k = tmp;              k_idx = j;
    }
  }
  if(k_idx == -1) {             // we didn't find the next one
    k_idx = k1_idx;
    net_k = k1_net;
  }

  LeabraUnit* k1_u = act_buff->Un(k1_idx, gpidx);
  LeabraUnit* k_u =  act_buff->Un(k_idx, gpidx);

  float k1_i = k1_u->i_thr;
  float k_i = k_u->i_thr;
  thr->kwta.k_ithr = k_i;
  thr->kwta.k1_ithr = k1_i;

  Compute_Inhib_BreakTie(thr);

  // place kwta inhibition between k and k+1
  float nw_gi = thr->kwta.k1_ithr + ispec.kwta_pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Avg(LeabraLayer* lay,
                         Layer::AccessMode acc_md, int gpidx,
                         LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits <=  1) {    // this is undefined
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  int k_eff = thr->kwta.k;      // keep cutoff at k
  float k_net = FLT_MAX;
  int k_idx = 0;

  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  KwtaSortBuff* inact_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::INACT);

  Compute_Inhib_kWTA_Sort(lay, acc_md, gpidx, nunits, thr,
                          *act_buff, *inact_buff, k_eff, k_net, k_idx);

  if(k_eff <= 0) {		// something bad happened
    thr->Inhib_SetVals(ispec.kwta_pt);
    return;
  }

  // active_buf now has k most active units, get averages of both groups
  int j;
  float k_avg = 0.0f;
  for(j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->i_thr;
  k_avg /= (float)k_eff;

  float oth_avg = 0.0f;
  int iabsz = inact_buff->Size(gpidx);
  for(j=0; j < iabsz; j++)
    oth_avg += inact_buff->Un(j, gpidx)->i_thr;
  if(iabsz > 0)
    oth_avg /= (float)iabsz;

  // place kwta inhibition between two averages
  // this uses the adapting point!
  float pt = ispec.kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;

  Compute_Inhib_BreakTie(thr);

  float nw_gi = thr->kwta.k1_ithr + pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);

  nw_gi = MAX(nw_gi, ispec.min_i);
  thr->i_val.kwta = nw_gi;
}

void LeabraLayerSpec::Compute_Inhib_FfFb(LeabraLayer* lay,
                         Layer::AccessMode acc_md, int gpidx,
                         LeabraInhib* thr, LeabraNetwork* net, LeabraInhibSpec& ispec) {

  float nw_ffi = ispec.FFInhib(thr->netin.avg);
  float fbi_x;
  float nw_fbi = ispec.FBInhib(thr->acts.avg, fbi_x);

  thr->kwta.ffi = nw_ffi;
  thr->kwta.fbi_x = fbi_x;
  // dt only on fbi part
  thr->kwta.fbi = ispec.dt * nw_fbi + (1.0f - ispec.dt) * thr->kwta.fbi;

  thr->i_val.kwta = ispec.gi * (thr->kwta.ffi + thr->kwta.fbi); // combine
  thr->kwta.k_ithr = 0.0f;
  thr->kwta.k1_ithr = 0.0f;
}

void LeabraLayerSpec::Compute_CtDynamicInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule == LeabraNetwork::LEABRA_CHL) return;

  float bi = net->ct_sin_i.burst_i;
  float ti = net->ct_sin_i.trough_i;
  if(ct_inhib_mod.use_sin) {
    bi = ct_inhib_mod.burst_i;
    ti = ct_inhib_mod.trough_i;
  }
  float ii = net->ct_fin_i.inhib_i;
  if(ct_inhib_mod.use_fin) {
    ii = ct_inhib_mod.inhib_i;
  }
  float imod = net->ct_sin_i.GetInhibMod(net->ct_cycle, bi, ti) +
    net->ct_fin_i.GetInhibMod(net->ct_cycle - net->ct_time.inhib_start, ii);

  // only one is going to be in effect at a time..
  lay->i_val.g_i += imod * lay->i_val.g_i;

  if(inhib_group != ENTIRE_LAYER) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i += imod * gpd->i_val.g_i;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
//      Inhibition Stage 2.2: LayInhibToGps

void LeabraLayerSpec::Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork*) {
  if(!lay->unit_groups) return;

  if(inhib_group == ENTIRE_LAYER) {
    // propagate g_i to all subgroups even if doing ENTIRE_LAYER
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i = lay->i_val.g_i;
    }
  }
  else if(inhib_group == UNIT_GROUPS) {
    if(unit_gp_inhib.on) {  // linking groups: get max from layer
      if(inhib.type == LeabraInhibSpec::FF_FB_INHIB && unit_gp_inhib.fffb) {
        float nw_ffi = inhib.FFInhib(lay->netin.avg);
        float fbi_x;
        float nw_fbi = inhib.FBInhib(lay->acts.avg, fbi_x);

        lay->kwta.ffi = nw_ffi;
        lay->kwta.fbi_x = fbi_x;
        // dt only on fbi part
        lay->kwta.fbi = inhib.dt * nw_fbi + (1.0f - inhib.dt) * lay->kwta.fbi;

        lay->i_val.kwta = unit_gp_inhib.lay_gi * (lay->kwta.ffi + lay->kwta.fbi);
        lay->i_val.g_i = lay->i_val.kwta;
        
        for(int g=0; g < lay->gp_geom.n; g++) {
          LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
          gpd->i_val.gp_g_i = lay->i_val.g_i;
          gpd->i_val.g_i = MAX(gpd->i_val.g_i, lay->i_val.g_i);
        }        
      }
      else {
        for(int g=0; g < lay->gp_geom.n; g++) {
          LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
          gpd->i_val.gp_g_i = lay->i_val.g_i;
          gpd->i_val.g_i = MAX(gpd->i_val.g_i, lay->i_val.g_i);
        }
      }
    }
  }
  else if(inhib_group == LAY_AND_GPS) {
    // actual inhibition is max of layer and group
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->i_val.g_i = MAX(gpd->i_val.g_i, lay->i_val.g_i);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
//      Inhibition Stage 2.3: Apply Inhib

void LeabraLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;                     // don't do this during normal processing

  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_ApplyInhib_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  else {
    Compute_ApplyInhib_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ApplyInhib_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraInhib* thr, LeabraNetwork* net)
{
  int nunits = lay->UnitAccess_NUnits(acc_md);
  float inhib_val = thr->i_val.g_i;
  if(thr->kwta.tie_brk == 1) {
    float inhib_thr = thr->kwta.k_ithr;
    float inhib_loser = thr->kwta.eff_loser_gain * thr->i_val.g_i;
    for(int i=0; i<nunits; i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      u->Compute_ApplyInhib_LoserGain(net, inhib_thr, inhib_val, inhib_loser);
    }
  }
  else {
    for(int i=0; i<nunits; i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      u->Compute_ApplyInhib(net, inhib_val);
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation

///////////////////////////////////////////////////////////////////////
//      Cycle Stats

void LeabraLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped) {
    Compute_OutputName(lay, net); // need to keep doing this because network clears it
    return;
  }

  Compute_Acts_AvgMax(lay, net);
  Compute_MaxDa(lay, net);
  Compute_OutputName(lay, net);

  if(lay->un_g_i.cmpt)
    Compute_UnitInhib_AvgMax(lay, net);

  if(lay->HasExtFlag(Unit::TARG)) {
    net->trg_max_act = MAX(net->trg_max_act, lay->acts.max);
  }

  if(lay->Iconified()) {
    lay->icon_value = lay->acts.avg;
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
  int nunits = lay->UnitAccess_NUnits(acc_md);
  int sti = LayerStatsStartUnitIdx();
  for(int i=sti; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    thr->acts.UpdtVals(u->act_eq, i);
  }
  thr->acts.CalcAvg(nunits-sti);

  if(thr->acts_top_k.cmpt) {
    thr->acts_top_k.InitVals();
    int k_eff = thr->kwta.k;    // keep cutoff at k
    KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
    if(k_eff <= 0 || act_buff->Size(gpidx) != k_eff)
      return; // no can do

    for(int j=0; j < k_eff; j++) {
      thr->acts_top_k.UpdtVals(act_buff->Un(j, gpidx)->act_eq, j);
    }
    thr->acts_top_k.CalcAvg(k_eff);
  }
}

void LeabraLayerSpec::Compute_Acts_AvgMax(LeabraLayer* lay, LeabraNetwork* net) {
  AvgMaxVals& vals = lay->acts;
//   static ta_memb_ptr mb_off = 0;
//   if(mb_off == 0) {
//     TypeDef* td = &TA_LeabraUnit; int net_base_off = 0;
//     TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "act_eq");
//   }
  if(lay->unit_groups) {
    vals.InitVals();
    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    int sti = LayerStatsStartUnitIdx();
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_AvgMaxActs_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd);
      vals.UpdtFmAvgMax(gpd->acts, nunits-sti, g);
      lay->acts_top_k.UpdtFmAvgMax(gpd->acts_top_k, 1, g); // only compute gp-wise avg for avg top k (n=1 per group)
    }
    int lvs_eff = lay->units.leaves-sti*lay->gp_geom.n;
    vals.CalcAvg(lvs_eff);
    lay->acts_top_k.CalcAvg(lay->gp_geom.n);
  }
  else {
    Compute_AvgMaxActs_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay);
  }
}

void LeabraLayerSpec::Compute_MaxDa_ugp(LeabraLayer* lay,
                                        Layer::AccessMode acc_md, int gpidx,
                                        LeabraInhib* thr, LeabraNetwork* net) {
  thr->maxda = 0.0f;
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0; i<nunits; i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float fda = u->Compute_MaxDa(net);
    lay->maxda = MAX(fda, lay->maxda);
    thr->maxda = MAX(fda, thr->maxda);
    net->maxda = MAX(fda, net->maxda);
  }
}

void LeabraLayerSpec::Compute_MaxDa(LeabraLayer* lay, LeabraNetwork* net) {
  lay->maxda = 0.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      Compute_MaxDa_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  else {
    Compute_MaxDa_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
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

  if(thr->acts.max_i < 0) {
    *onm = "n/a";
    return;
  }
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, thr->acts.max_i, gpidx);
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
    TypeDef::FindMemberPathStatic(td, net_base_off, mb_off, "gc.i");
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
//      Cycle Stats -- optional non-default guys

float LeabraLayerSpec::Compute_TopKAvgAct_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraInhib* thr, LeabraNetwork*) {
  int k_eff = thr->kwta.k;      // keep cutoff at k
  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  if(TestError(k_eff <= 0 || act_buff->Size(gpidx) != k_eff, "Compute_TopKAvgAct_ugp",
               "Only usable when using a kwta function -- kwta sort buff was not set properly!")) {
    return -1;
  }

  float k_avg = 0.0f;
  for(int j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->act_eq;
  k_avg /= (float)k_eff;

  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgAct(LeabraLayer* lay, LeabraNetwork* net) {
  float k_avg = 0.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      k_avg += Compute_TopKAvgAct_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
    if(lay->unit_groups)
      k_avg /= (float)lay->gp_geom.n;
  }
  else {
    k_avg = Compute_TopKAvgAct_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgNetin_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraInhib* thr, LeabraNetwork*) {
  int k_eff = thr->kwta.k;      // keep cutoff at k
  KwtaSortBuff* act_buff = lay->SortBuff(acc_md, KwtaSortBuff_List::ACTIVE);
  if(TestError(k_eff <= 0 || act_buff->Size(gpidx) != k_eff, "Compute_TopKAvgNetin_ugp",
               "Only usable when using a kwta function -- kwta sort buff was not set properly!")) {
    return -1;
  }

  float k_avg = 0.0f;
  for(int j=0; j < k_eff; j++)
    k_avg += act_buff->Un(j, gpidx)->net;
  k_avg /= (float)k_eff;

  return k_avg;
}

float LeabraLayerSpec::Compute_TopKAvgNetin(LeabraLayer* lay, LeabraNetwork* net) {
  float k_avg = 0.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      k_avg += Compute_TopKAvgNetin_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
    if(lay->unit_groups)
      k_avg /= (float)lay->gp_geom.n;
  }
  else {
    k_avg = Compute_TopKAvgNetin_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
  }
  return k_avg;
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
  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  switch(net->phase_order) {
  case LeabraNetwork::MINUS_PLUS:
    if(no_plus_testing) {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPlus(lay, net);
      lay->phase_dif_ratio = 1.0f;
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE)
        PostSettle_GetMinus(lay, net);
      else {
        PostSettle_GetPlus(lay, net);
        PostSettle_GetPhaseDifRatio(lay, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_MINUS:
    if(no_plus_testing) {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPlus(lay, net);
      lay->phase_dif_ratio = 1.0f;
    }
    else {
      if(net->phase == LeabraNetwork::MINUS_PHASE) {
        PostSettle_GetMinus(lay, net);
        PostSettle_GetPhaseDifRatio(lay, net);
      }
      else {
        PostSettle_GetPlus(lay, net);
      }
    }
    break;
  case LeabraNetwork::PLUS_ONLY:
    PostSettle_GetMinus(lay, net);
    PostSettle_GetPlus(lay, net);
    lay->phase_dif_ratio = 1.0f;
    break;
  case LeabraNetwork::MINUS_PLUS_NOTHING:
  case LeabraNetwork::MINUS_PLUS_MINUS:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      PostSettle_GetMinus(lay, net);
    }
    else if(net->phase_no == 1) {
      PostSettle_GetPlus(lay, net);
      PostSettle_GetPhaseDifRatio(lay, net);
    }
    else {
      PostSettle_GetMinus2(lay, net);
    }
    break;
  case LeabraNetwork::PLUS_NOTHING:
    // don't use actual phase values because pluses might be minuses with testing
    if(net->phase_no == 0) {
      PostSettle_GetPlus(lay, net);
    }
    else {
      PostSettle_GetMinus(lay, net);
      PostSettle_GetPhaseDifRatio(lay, net);
    }
    break;
  }

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->PostSettle(net);
  }

  if((adapt_i.type == AdaptISpec::G_BAR_I) || (adapt_i.type == AdaptISpec::G_BAR_IL)) {
    AdaptGBarI(lay, net);
  }
}

void LeabraLayerSpec::PostSettle_GetMinus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_m = gpd->acts;
    }
  }
}

void LeabraLayerSpec::PostSettle_GetPlus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_p = gpd->acts;
    }
  }
}

void LeabraLayerSpec::PostSettle_GetMinus2(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m2 = lay->acts;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_m2 = gpd->acts;
    }
  }
}

void LeabraLayerSpec::PostSettle_GetPhaseDifRatio(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->acts_p.avg > 0.0f)
    lay->phase_dif_ratio = lay->acts_m.avg / lay->acts_p.avg;
  else
    lay->phase_dif_ratio = 1.0f;
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      if(gpd->acts_p.avg > 0.0f)
        gpd->phase_dif_ratio = gpd->acts_m.avg / gpd->acts_p.avg;
      else
        gpd->phase_dif_ratio = 1.0f;
    }
  }
}

void LeabraLayerSpec::AdaptGBarI(LeabraLayer* lay, LeabraNetwork*) {
  float diff = lay->kwta.pct - lay->acts.avg;
  if(fabsf(diff) > adapt_i.tol) {
    float p_i = 1.0f;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      p_i = 1.0f - adapt_i.l;
    }
    lay->adapt_i.g_bar_i -= p_i * adapt_i.p_dt * diff;
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    float minv = us->g_bar.i * (1.0 - adapt_i.mx_d);
    float maxv = us->g_bar.i * (1.0 + adapt_i.mx_d);
    if(lay->adapt_i.g_bar_i < minv) lay->adapt_i.g_bar_i = minv;
    if(lay->adapt_i.g_bar_i > maxv) lay->adapt_i.g_bar_i = maxv;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      lay->adapt_i.g_bar_l -= adapt_i.l * adapt_i.p_dt * diff;
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
      float minv = us->g_bar.l * (1.0 - adapt_i.mx_d);
      float maxv = us->g_bar.l * (1.0 + adapt_i.mx_d);
      if(lay->adapt_i.g_bar_l < minv) lay->adapt_i.g_bar_l = minv;
      if(lay->adapt_i.g_bar_l > maxv) lay->adapt_i.g_bar_l = maxv;
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

void LeabraLayerSpec::Compute_SRAvg_State(LeabraLayer* lay, LeabraNetwork* net) {
  if(!ct_inhib_mod.manual_sravg) {
    lay->sravg_vals.state = net->sravg_vals.state; // default is to just copy

    if(lay->sravg_vals.state != CtSRAvgVals::NO_SRAVG) {
      if(ct_inhib_mod.sravg_delay > 0) {
	if(net->phase != LeabraNetwork::PLUS_PHASE) {
	  int into = net->ct_cycle - net->ct_sravg.start;
	  if(into <= ct_inhib_mod.sravg_delay) // override!
	    lay->sravg_vals.state = CtSRAvgVals::NO_SRAVG;
	}
      }
    }
  }
}

void LeabraLayerSpec::Compute_SRAvg_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->sravg_vals.state == CtSRAvgVals::NO_SRAVG) return; // don't

  if(lay->sravg_vals.state == CtSRAvgVals::SRAVG_M ||
     lay->sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    lay->sravg_vals.m_sum += 1.0f;
    lay->sravg_vals.m_nrm = 1.0f / lay->sravg_vals.m_sum;
  }

  if(lay->sravg_vals.state == CtSRAvgVals::SRAVG_S ||
     lay->sravg_vals.state == CtSRAvgVals::SRAVG_SM) {
    lay->sravg_vals.s_sum += 1.0f;
    lay->sravg_vals.s_nrm = 1.0f / lay->sravg_vals.s_sum;
  }
}

bool LeabraLayerSpec::Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return true;
}

bool LeabraLayerSpec::Compute_dWt_FirstMinus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  return false;			// standard layers never do it
}

bool LeabraLayerSpec::Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->phase_order == LeabraNetwork::MINUS_PLUS_NOTHING ||
      net->phase_order == LeabraNetwork::MINUS_PLUS_MINUS) &&
     (net->learn_rule != LeabraNetwork::LEABRA_CHL &&
      net->learn_rule != LeabraNetwork::CTLEABRA_XCAL_C)) { // xcal_c learns on 1st plus!
    return false;
  }
  return true;
}

bool LeabraLayerSpec::Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) {
  // shouldn't happen, but just in case..
  if(net->learn_rule == LeabraNetwork::CTLEABRA_XCAL_C) return false; // only 1st plus
  return true;          // all types learn here..
}

///////////////////////////////////////////////////////////////////////
//      Trial-level Stats

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
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      if(net->on_errs && net->off_errs)
        ntot += 2 * gpd->kwta.k;
      else
        ntot += gpd->kwta.k;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    if(net->on_errs && net->off_errs)
      ntot += 2 * lay->kwta.k;
    else
      ntot += lay->kwta.k;
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
    if(net->ti_mode) {
      cosvp += u->targ * u->p_act_p;
      ssp += u->p_act_p * u->p_act_p;
    }
  }
  if(n_vals == 0) return 0.0f;
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->cos_err = cosv;
  if(net->ti_mode) {
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

  if(cos_diff_lrate.on) {
    cos_diff_lrate.UpdtDiffAvg(lay->cos_diff_avg, lay->cos_diff);
    lay->cos_diff_lrate = cos_diff_lrate.LrateMod(lay->cos_diff_avg, lay->cos_diff);
  }

  return cosv;
}

float LeabraLayerSpec::Compute_CosDiff2(LeabraLayer* lay, LeabraNetwork* net) {
  float cosv = 0.0f;
  float ssm = 0.0f;
  float sst = 0.0f;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    cosv += u->act_p * u->act_m2;
    ssm += u->act_m2 * u->act_m2;
    sst += u->act_p * u->act_p;
  }
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  return cosv;
}

////////////////////////////////////////////////////////////////////////////////
//      Parameter Adaptation over longer timesales

void LeabraLayerSpec::AdaptKWTAPt_ugp(LeabraLayer* lay,
                                      Layer::AccessMode acc_md, int gpidx,
                                      LeabraInhib* thr, LeabraNetwork* net) {
  thr->adapt_i.avg_avg += adapt_i.a_dt * (thr->acts_m.avg - thr->adapt_i.avg_avg);
  float dif = thr->adapt_i.avg_avg - thr->kwta.pct;
  if(dif < -adapt_i.tol) {      // average is less than target
    // so reduce the point towards lower value
    thr->adapt_i.i_kwta_pt += adapt_i.p_dt *
      ((inhib.kwta_pt - adapt_i.mx_d) - thr->adapt_i.i_kwta_pt);
  }
  else if(dif > adapt_i.tol) {  // average is more than target
    // so increase point towards higher value
    thr->adapt_i.i_kwta_pt += adapt_i.p_dt *
      ((inhib.kwta_pt + adapt_i.mx_d) - thr->adapt_i.i_kwta_pt);
  }
}

void LeabraLayerSpec::AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->HasExtFlag(Unit::EXT) && !lay->HasExtFlag(Unit::TARG))
    return;                     // don't adapt points for input-only layers
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      AdaptKWTAPt_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
    }
  }
  AdaptKWTAPt_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
}

void LeabraLayerSpec::Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->netin.max < 0.01f) return; // not getting enough activation to count!

  lay->avg_netin_sum.avg += lay->netin.avg;
  lay->avg_netin_sum.max += lay->netin.max;
  lay->avg_netin_n++;

  float sum_net = 0.0f;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
    prjn->netin_avg = 0.0f;
    int netin_avg_n = 0;
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      if(u->lesioned()) continue;
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      if(u->act_eq < us->opt_thresh.send) continue; // ignore if not above sending thr
      LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(prjn->recv_idx);
      if(!cg) continue;
      float netin = cg->Compute_Netin(u,net);
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
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
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

void LeabraLayerSpec::Compute_TrgRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  int n_in = 0;
  int n_out = 0;
  int n_lat = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->off || !prjn->from || prjn->from->lesioned()) {
      prjn->trg_netin_rel = 0.0f;
      continue;
    }
    if(prjn->con_spec->InheritsFrom(&TA_MarkerConSpec)) { // fix these guys
      prjn->trg_netin_rel = 0.0f;
      prjn->direction = Projection::DIR_UNKNOWN;
      continue;
    }
    if(prjn->direction == Projection::FM_INPUT) n_in++;
    else if(prjn->direction == Projection::FM_OUTPUT) n_out++;
    else if(prjn->direction == Projection::LATERAL) n_lat++;
  }
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    float in_trg = cs->rel_net_adapt.trg_fm_input;
    float out_trg = cs->rel_net_adapt.trg_fm_output;
    float lat_trg = cs->rel_net_adapt.trg_lateral;
    if(prjn->direction == Projection::FM_INPUT) {
      if(n_out == 0 && n_lat == 0) in_trg = 1.0;
      else if(n_out == 0) in_trg = in_trg / (in_trg + lat_trg);
      else if(n_lat == 0) in_trg = in_trg / (in_trg + out_trg);
      prjn->trg_netin_rel = in_trg / (float)n_in;
    }
    else if(prjn->direction == Projection::FM_OUTPUT) {
      if(n_in == 0 && n_lat == 0) out_trg = 1.0;
      else if(n_in == 0) out_trg = out_trg / (out_trg + lat_trg);
      else if(n_lat == 0) out_trg = out_trg / (out_trg + in_trg);
      prjn->trg_netin_rel = out_trg / (float)n_out;
    }
    else if(prjn->direction == Projection::LATERAL) {
      if(n_in == 0 && n_out == 0) lat_trg = 1.0;
      else if(n_in == 0) lat_trg = lat_trg / (lat_trg + out_trg);
      else if(n_out == 0) lat_trg = lat_trg / (lat_trg + in_trg);
      prjn->trg_netin_rel = lat_trg / (float)n_lat;
    }
  }
}

void LeabraLayerSpec::Compute_AdaptRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    if(prjn->trg_netin_rel <= 0.0f) continue; // not set
    if(!cs->rel_net_adapt.on) continue;
    if(cs->rel_net_adapt.CheckInTolerance(prjn->trg_netin_rel, prjn->avg_netin_rel))
      continue;
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.rel += cs->rel_net_adapt.rel_lrate *
      (prjn->trg_netin_rel - prjn->avg_netin_rel);
    if(cs->wt_scale.rel <= 0.0f) cs->wt_scale.rel = 0.0f;
  }
}

void LeabraLayerSpec::Compute_AdaptAbsNetin(LeabraLayer* lay, LeabraNetwork*) {
  if(!abs_net_adapt.on) return;
  float dst = abs_net_adapt.trg_net - lay->avg_netin.max;
  if(fabsf(dst) < abs_net_adapt.tol) return;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(prjn->off || !prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.abs += abs_net_adapt.abs_lrate * dst;
  }
}

