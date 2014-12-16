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
TA_BASEFUNS_CTORS_DEFN(LayerAvgActSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraInhibMisc);
TA_BASEFUNS_CTORS_DEFN(LeabraClampSpec);
TA_BASEFUNS_CTORS_DEFN(LayerDecaySpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDelInhib);
TA_BASEFUNS_CTORS_DEFN(LeabraLayStats);
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
}

void LeabraInhibSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  fb_dt = 1.0f / fb_tau;
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

void LeabraInhibMisc::Initialize() {
  self_fb = 0.0f;
  fb_up_immed = false;

  Defaults_init();
}

void LeabraInhibMisc::Defaults_init() {
  self_tau = 1.4f;
  self_dt = 1.0f / self_tau;
}

void LeabraInhibMisc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  self_dt = 1.0f / self_tau;
}

void LeabraClampSpec::Initialize() {
  hard = true;
  Defaults_init();
}

void LeabraClampSpec::Defaults_init() {
  gain = .2f;
}

void LayerDecaySpec::Initialize() {
  trial = 1.0f;

  Defaults_init();
}

void LayerDecaySpec::Defaults_init() {
}

void LeabraDelInhib::Initialize() {
  on = false;
  prv_trl = 0.0f;
  prv_q = 0.0f;

  Defaults_init();
}

void LeabraDelInhib::Defaults_init() {
}

void LeabraDelInhib::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void LeabraLayStats::Initialize() {
  Defaults_init();
}

void LeabraLayStats::Defaults_init() {
  cos_diff_avg_tau = 100.0f;
  hog_thr = 0.3f;

  cos_diff_avg_dt = 1.0f / cos_diff_avg_tau;
}

void LeabraLayStats::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cos_diff_avg_dt = 1.0f / cos_diff_avg_tau;
}

void LayGpInhibSpec::Initialize() {
  on = false;
  gp_g = 0.5f;
}

void LayGpInhibSpec::Defaults_init() {
  
}


//////////////////////////////////////////////////////////


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
  lstats.UpdateAfterEdit_NoGui();
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


void LeabraLayer::CheckInhibCons(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraPrjn, p, projections) {
    p->CheckInhibCons(net);
  }
}

void LeabraLayerSpec::Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m_avg = avg_act.init;
  lay->acts_p_avg = avg_act.init;
  if(avg_act.fixed) {
    lay->acts_p_avg_eff = avg_act.init;
  }
  else {
    lay->acts_p_avg_eff = avg_act.adjust * lay->acts_p_avg;
  }
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Init_State();
      gpd->acts_m_avg = avg_act.init;
      gpd->acts_p_avg = avg_act.init;
      if(avg_act.fixed) {
        gpd->acts_p_avg_eff = avg_act.init;
      }
      else {
        gpd->acts_p_avg_eff = avg_act.adjust * gpd->acts_p_avg;
      }
    }
  }
  Init_Inhib(lay, net);         // initialize inhibition at start..
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
  lay->avg_norm_err.ResetAvg();
  lay->cos_err = 0.0f;
  lay->cos_err_prv = 0.0f;
  lay->cos_err_vs_prv = 0.0f;
  lay->avg_cos_err.ResetAvg();
  lay->avg_cos_err_prv.ResetAvg();
  lay->avg_cos_err_vs_prv.ResetAvg();

  lay->cos_diff = 0.0f;
  lay->avg_cos_diff.ResetAvg();
  lay->cos_diff_avg = 0.0f;
  lay->cos_diff_avg_lmix = 0.0f;
  lay->avg_act_diff = 0.0f;
  lay->avg_avg_act_diff.ResetAvg();
  lay->trial_cos_diff = 0.0f;
  lay->avg_trial_cos_diff.ResetAvg();

  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    prjn->Init_Stats();
  }
}

void LeabraLayerSpec::Init_Acts_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  lay->ext_flag = UnitVars::NO_EXTERNAL;
  lay->hard_clamped = false;
  if(lay->units.leaves == 0) return; // may not be built yet!

  lay->Inhib_Init_Acts(this);
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->Inhib_Init_Acts(this);
    }
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

  if(decay.trial > 0.0f)
    net->net_misc.trial_decay = true;
}

void LeabraLayerSpec::Trial_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(decay.trial > 0.0f) {
    lay->i_val.ffi -= decay.trial * lay->i_val.ffi;
    lay->i_val.fbi -= decay.trial * lay->i_val.fbi;
    if(lay->unit_groups) {
      for(int g=0; g < lay->gp_geom.n; g++) {
        LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
        gpd->i_val.ffi -= decay.trial * gpd->i_val.ffi;
        gpd->i_val.fbi -= decay.trial * gpd->i_val.fbi;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      QuarterInit -- at start of settling

void LeabraLayerSpec::Quarter_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  Quarter_Init_TargFlags_Layer(lay, net);
}

void LeabraLayerSpec::Quarter_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->HasExtFlag(UnitVars::TARG)) {     // only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      lay->SetExtFlag(UnitVars::EXT);
  }
}

void LeabraLayerSpec::Compute_HardClamp_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(clamp.hard && lay->HasExtFlag(UnitVars::EXT))) {
    lay->hard_clamped = false;
    return;
  }
  lay->hard_clamped = true;     // cache this flag
  lay->Inhib_SetVals(0.5f);            // assume 0 - 1 clamped inputs
}

void LeabraLayerSpec::ExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(UnitVars::EXT))       // only process ext
    return;
  lay->ext_flag = UnitVars::COMP;   // totally reset to comparison
}

void LeabraLayerSpec::TargExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->HasExtFlag(UnitVars::TARG_EXT))  // only process w/ external input
    return;
  lay->ext_flag = UnitVars::COMP;   // totally reset to comparison
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 2: Inhibition, Basic computation

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  if(lay->hard_clamped) return; // say no more..

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
  if(inhib_misc.fb_up_immed) {
    if(nw_fbi > thr->i_val.fbi) { // up_immed case --- best for spiking
      thr->i_val.fbi = nw_fbi;
    }
    else {
      thr->i_val.fbi += ispec.fb_dt * (nw_fbi - thr->i_val.fbi);
    }
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
  LeabraUnit* u = (LeabraUnit*)net->UnFmIdx(thr->acts_eq.max_i); // max_i = flat now
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

///////////////////////////////////////////////////////////////////////
//      Quarter_Final

void LeabraLayerSpec::Quarter_Final_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->quarter == 2) {       // quarter still pre-increment?
    Quarter_Final_GetMinus(lay, net);
  }
  else if(net->quarter == 3) {
    Quarter_Final_GetPlus(lay, net);
  }
}

void LeabraLayerSpec::Quarter_Final_GetMinus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_m = lay->acts_eq;
  lay->acts_m_avg += avg_act.dt * (lay->acts_m.avg - lay->acts_m_avg);
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_m = gpd->acts_eq;
      gpd->acts_m_avg += avg_act.dt * (gpd->acts_m.avg - gpd->acts_m_avg);
    }
  }
}

void LeabraLayerSpec::Quarter_Final_GetPlus(LeabraLayer* lay, LeabraNetwork* net) {
  lay->acts_p = lay->acts_eq;
  lay->acts_p_avg += avg_act.dt * (lay->acts_p.avg - lay->acts_p_avg); 
  if(avg_act.fixed) {
    lay->acts_p_avg_eff = avg_act.init;
  }
  else {
    lay->acts_p_avg_eff = avg_act.adjust * lay->acts_p_avg;
  }
  if(lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      gpd->acts_p = gpd->acts_eq;
      gpd->acts_p_avg += avg_act.dt * (gpd->acts_p.avg - gpd->acts_p_avg);
      if(avg_act.fixed) {
        gpd->acts_p_avg_eff = avg_act.fixed;
      }
      else {
        gpd->acts_p_avg_eff = avg_act.adjust * gpd->acts_p_avg;
      }
    }
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
  // use default, but allow subclasses to override in layerspec
  return lay->Layer::Compute_SSE(net, n_vals, unit_avg, sqrt);
}

float LeabraLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;        // assume not contributing
  if(!lay->HasExtFlag(UnitVars::COMP_TARG)) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  int ntrg_act = 0;

  const int li = lay->active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& lay_nerr = net->ThrLayStats(thr_no, li, 0, LeabraNetwork::NORMERR);
    float& lay_trg_n = net->ThrLayStats(thr_no, li, 1, LeabraNetwork::NORMERR);

    nerr += lay_nerr;
    ntrg_act += (int)lay_trg_n;
  }

  if(net->lstats.on_errs && net->lstats.off_errs)
    ntrg_act *= 2;              // double count

  if(ntrg_act == 0) return -1.0f;

  lay->norm_err = nerr / (float)ntrg_act;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f; // shouldn't happen...

  lay->avg_norm_err.Increment(lay->norm_err);
  
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(UnitVars::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

float LeabraLayerSpec::Compute_CosErr(LeabraLayer* lay, LeabraNetwork* net,
				     int& n_vals) {
  lay->cos_err = 0.0f;
  lay->cos_err_prv = 0.0f;
  lay->cos_err_vs_prv = 0.0f;
  n_vals = 0;
  if(!lay->HasExtFlag(UnitVars::COMP_TARG)) return 0.0f;
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(UnitVars::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    return 0.0f;
  }
  float cosv = 0.0f;
  float cosvp = 0.0f;
  float ssm = 0.0f;
  float ssp = 0.0f;
  float sst = 0.0f;
  const int li = lay->active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& lcosv = net->ThrLayStats(thr_no, li, 0, LeabraNetwork::COSERR);
    float& lcosvp = net->ThrLayStats(thr_no, li, 1, LeabraNetwork::COSERR);
    float& lssm = net->ThrLayStats(thr_no, li, 2, LeabraNetwork::COSERR);
    float& lssp = net->ThrLayStats(thr_no, li, 3, LeabraNetwork::COSERR);
    float& lsst = net->ThrLayStats(thr_no, li, 4, LeabraNetwork::COSERR);
    float& lnvals = net->ThrLayStats(thr_no, li, 5, LeabraNetwork::COSERR);

    n_vals += lnvals;
    cosv += lcosv;
    ssm += lssm;
    sst += lsst;
    if(net->net_misc.ti) {
      cosvp += lcosvp;
      ssp += lssp;
    }
  }
  if(n_vals == 0) return 0.0f;
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->cos_err = cosv;

  lay->avg_cos_err.Increment(lay->cos_err);

  if(net->net_misc.ti) {
    float pdist = sqrtf(ssp * sst);
    if(pdist != 0.0f) {
      cosvp /= pdist;
    }
    lay->cos_err_prv = cosvp;
    lay->cos_err_vs_prv = lay->cos_err - lay->cos_err_prv;

    lay->avg_cos_err_prv.Increment(lay->cos_err_prv);
    lay->avg_cos_err_vs_prv.Increment(lay->cos_err_vs_prv);
  }
  return cosv;
}

float LeabraLayerSpec::Compute_CosDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->cos_diff = 0.0f;
  float cosv = 0.0f;
  float ssm = 0.0f;
  float sst = 0.0f;

  const int li = lay->active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& lcosv = net->ThrLayStats(thr_no, li, 0, LeabraNetwork::COSDIFF);
    float& lssm = net->ThrLayStats(thr_no, li, 1, LeabraNetwork::COSDIFF);
    float& lsst = net->ThrLayStats(thr_no, li, 2, LeabraNetwork::COSDIFF);

    cosv += lcosv;
    ssm += lssm;
    sst += lsst;
  }
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->cos_diff = cosv;

  lstats.UpdtDiffAvg(lay->cos_diff_avg, lay->cos_diff);
  if(lay->layer_type == Layer::HIDDEN) {
    lay->cos_diff_avg_lmix = 1.0f - lay->cos_diff_avg;
  }
  else {
    lay->cos_diff_avg_lmix = 0.0f; // no mix for TARGET layers; irrelevant for INPUT
  }

  lay->avg_cos_diff.Increment(lay->cos_diff);
  
  return cosv;
}

float LeabraLayerSpec::Compute_AvgActDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_act_diff = 0.0f;
  float adiff = 0.0f;
  int nd = 0;

  const int li = lay->active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& ladiff = net->ThrLayStats(thr_no, li, 0, LeabraNetwork::AVGACTDIFF);
    float& lnd = net->ThrLayStats(thr_no, li, 1, LeabraNetwork::AVGACTDIFF);
    adiff += ladiff;
    nd += (int)lnd;
  }
  
  if(nd > 0)
    adiff /= (float)nd;
  lay->avg_act_diff = adiff;

  lay->avg_avg_act_diff.Increment(lay->avg_act_diff);
  
  return adiff;
}

float LeabraLayerSpec::Compute_TrialCosDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->trial_cos_diff = 0.0f;
  float cosv = 0.0f;
  float ssm = 0.0f;
  float sst = 0.0f;

  const int li = lay->active_lay_idx;
  for(int thr_no=0; thr_no < net->n_thrs_built; thr_no++) {
    // integrate over thread raw data
    float& lcosv = net->ThrLayStats(thr_no, li, 0, LeabraNetwork::TRIALCOSDIFF);
    float& lssm = net->ThrLayStats(thr_no, li, 1, LeabraNetwork::TRIALCOSDIFF);
    float& lsst = net->ThrLayStats(thr_no, li, 2, LeabraNetwork::TRIALCOSDIFF);

    cosv += lcosv;
    ssm += lssm;
    sst += lsst;
  }
  
  float dist = sqrtf(ssm * sst);
  if(dist != 0.0f)
    cosv /= dist;
  lay->trial_cos_diff = cosv;

  lay->avg_trial_cos_diff.Increment(lay->trial_cos_diff);
  
  return cosv;
}

void LeabraLayerSpec::Compute_AvgNormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_norm_err.GetAvg_Reset();
}

void LeabraLayerSpec::Compute_AvgCosErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_cos_err.GetAvg_Reset();

  if(net->net_misc.ti) {
    lay->avg_cos_err_prv.GetAvg_Reset();
    lay->avg_cos_err_vs_prv.GetAvg_Reset();
  }
}

void LeabraLayerSpec::Compute_AvgCosDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_cos_diff.GetAvg_Reset();
}

void LeabraLayerSpec::Compute_AvgAvgActDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_avg_act_diff.GetAvg_Reset();
}

void LeabraLayerSpec::Compute_AvgTrialCosDiff(LeabraLayer* lay, LeabraNetwork* net) {
  lay->avg_trial_cos_diff.GetAvg_Reset();
}

void LeabraLayerSpec::Compute_EpochStats(LeabraLayer* lay, LeabraNetwork* net) {
  lay->Layer::Compute_EpochStats(net);
  Compute_AvgNormErr(lay, net);
  Compute_AvgCosErr(lay, net);
  Compute_AvgCosDiff(lay, net);
  Compute_AvgAvgActDiff(lay, net);
  Compute_AvgTrialCosDiff(lay, net);
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
        if(u->act_eq() < us->opt_thresh.send) continue; // ignore if not above sending thr
        LeabraConGroup* cg = (LeabraConGroup*)u->RecvConGroupPrjn(prjn);
        if(!cg) continue;
        float netin;
        if(net->NetinPerPrjn()) {
          netin = cg->net_raw;
        }
        else {
          netin = ((LeabraConSpec*)cg->con_spec)->Compute_Netin(cg, net, u->ThrNo());
          // otherwise have to compute it
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

void LeabraLayerSpec::ClearTICtxt(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    ((LeabraUnitSpec*)u->GetUnitSpec())->ClearTICtxt
      ((LeabraUnitVars*)u->GetUnitVars(), net, u->ThrNo());
  }
}

