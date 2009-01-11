// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_extra.h"

#include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////
//  	MarkerConSpec   	//
//////////////////////////////////

void MarkerConSpec::Initialize() {
  SetUnique("rnd", true);
  rnd.mean = 0.0f; rnd.var = 0.0f;
  SetUnique("wt_limits", true);
  wt_limits.sym = false;
  SetUnique("wt_scale", true);
  wt_scale.rel = 0.0f;
  SetUnique("lrate", true);
  lrate = 0.0f;
  cur_lrate = 0.0f;
}

//////////////////////////////////
//  	ContextLayerSpec	//
//////////////////////////////////

void CtxtUpdateSpec::Initialize() {
  fm_hid = 1.0f;
  fm_prv = 0.0f;
  to_out = 1.0f;
}

void LeabraContextLayerSpec::Initialize() {
  updt.fm_prv = 0.0f;
  updt.fm_hid = 1.0f;
  updt.to_out = 1.0f;
  SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
}

// void LeabraContextLayerSpec::UpdateAfterEdit-impl() {
//   inherited::UpdateAfterEdit_impl();
//   hysteresis_c = 1.0f - hysteresis;
// }

bool LeabraContextLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  return rval;
}

void LeabraContextLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
  Initialize();
}

void LeabraContextLayerSpec::Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net) {
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    u->ext = u->act_m;		// just use previous minus phase value!
  }
  else {
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv[0];
    if(TestError(!cg, "Compute_Context", "requires one recv projection!")) {
      return;
    }
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(TestError(!su, "Compute_Context", "requires one unit in recv projection!")) {
      return;
    }
    u->ext = updt.fm_prv * u->act_p + updt.fm_hid * su->act_p; // compute new value
  }
  u->SetExtFlag(Unit::EXT);
  u->Compute_HardClamp(lay, net);
}

void LeabraContextLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = true;	// cache this flag
  lay->SetExtFlag(Unit::EXT);
  lay->Inhib_SetVals(inhib.kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    Compute_Context(lay, u, net);
  }
  Compute_ActAvg(lay, net);
}

//////////////////////////////////////////
// 	Misc Special Objects		//
//////////////////////////////////////////

//////////////////////////////////
// 	Linear Unit		//
//////////////////////////////////

void LeabraLinUnitSpec::Initialize() {
  SetUnique("act_fun", true);
  SetUnique("act_range", true);
  SetUnique("clamp_range", true);
  SetUnique("act", true);
  act_fun = LINEAR;
  act_range.max = 20;
  act_range.min = 0;
  act_range.UpdateAfterEdit();
  clamp_range.max = 1.0f;
  clamp_range.UpdateAfterEdit();
  act.gain = 2;
}

void LeabraLinUnitSpec::Defaults() {
  LeabraUnitSpec::Defaults();
  Initialize();
}

void LeabraLinUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork* net) {
  float new_act = u->net * act.gain; // use linear netin as act

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += noise_sched.GetVal(net->cycle) * noise.Gen();
  }
  u->act = u->act_nd = u->act_eq = act_range.Clip(new_act);
}

//////////////////////////
// 	NegBias		//
//////////////////////////

void LeabraNegBiasSpec::Initialize() {
  decay = 0.0f;
  updt_immed = false;
}

//////////////////////////////////
// 	TrialSynDepConSpec	//
//////////////////////////////////

void TrialSynDepSpec::Initialize() {
  rec = 1.0f;
  depl = 1.1f;
}

void TrialSynDepConSpec::Initialize() {
  min_obj_type = &TA_TrialSynDepCon;
}

void TrialSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(syn_dep.rec <= 0.0f)	// can't go to zero!
    syn_dep.rec = 1.0f;
}

//////////////////////////////////
// 	CycleSynDepConSpec	//
//////////////////////////////////

void CycleSynDepSpec::Initialize() {
  rec = 0.002f;
  asymp_act = 0.4f;
  depl = rec * (1.0f - asymp_act); // here the drive is constant
}

void CycleSynDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(rec < .00001f) rec = .00001f;
  // chg = rec * (1 - cur) - dep * drive = 0; // equilibrium point
  // rec * (1 - cur) = dep * drive
  // dep = rec * (1 - cur) / drive
  depl = rec * (1.0f - asymp_act); // here the drive is constant
  depl = MAX(depl, 0.0f);
}

void CycleSynDepConSpec::Initialize() {
  min_obj_type = &TA_CycleSynDepCon;
}

void CycleSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  syn_dep.UpdateAfterEdit();
}

//////////////////////////////////
// 	CaiSynDepCon
//////////////////////////////////

void CaiSynDepSpec::Initialize() {
  ca_inc = .2f;			// base per-cycle is .01
  ca_dec = .2f;			// base per-cycle is .01
  sd_ca_thr = 0.2f;
  sd_ca_gain = 0.3f;
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CaiSynDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CaiSynDepConSpec::Initialize() {
  min_obj_type = &TA_CaiSynDepCon;
}

void CaiSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ca_dep.UpdateAfterEdit();
}


//////////////////////////////////
// 	FastWtConSpec		//
//////////////////////////////////

void FastWtSpec::Initialize() {
  lrate = .05f;
  use_lrs = false;
  cur_lrate = .05f;
  decay = 1.0f;
  slw_sat = true;
  dk_mode = SU_THR;
}

void FastWtConSpec::Initialize() {
  min_obj_type = &TA_FastWtCon;
}

void FastWtConSpec::SetCurLrate(LeabraNetwork* net, int epoch) {
  LeabraConSpec::SetCurLrate(net, epoch);
  if(fast_wt.use_lrs)
    fast_wt.cur_lrate = fast_wt.lrate * lrate_sched.GetVal(epoch);
  else
    fast_wt.cur_lrate = fast_wt.lrate;
}

///////////////////////////////////////////////////////////////
//   ActAvgHebbConSpec

void ActAvgHebbMixSpec::Initialize() {
  act_avg = .5f;
  cur_act = .5f;
}

void ActAvgHebbMixSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cur_act = 1.0f - act_avg;
}

void ActAvgHebbConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_avg_hebb.UpdateAfterEdit();
}

void ActAvgHebbConSpec::Initialize() {
}


void LeabraLimPrecConSpec::Initialize() {
  prec_levels = 1024;
}

void LeabraDwtNorm::Initialize() {
  on = false;
  norm_pct = 1.0f;
}

void LeabraDwtNormConSpec::Initialize() {
}

void LeabraDaNoise::Initialize() {
  da_noise = 1.0f;
  std_leabra = 1.0f;
}

void LeabraDaNoiseConSpec::Initialize() {
}

//////////////////////////////////
// 	Scalar Value Layer	//
//////////////////////////////////

void ScalarValSpec::Initialize() {
  rep = LOCALIST;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  min_sum_act = 0.2f;
  val_mult_lrn = false;
  clip_val = true;
  send_thr = false;
  init_nms = true;

  min = val = 0.0f;
  range = incr = 1.0f;
  un_width_eff = un_width;
}

void ScalarValSpec::InitRange(float umin, float urng) {
  min = umin; range = urng;
  un_width_eff = un_width;
  if(norm_width)
    un_width_eff *= range;
}

void ScalarValSpec::InitVal(float sval, int ugp_size, float umin, float urng) {
  InitRange(umin, urng);
  val = sval;
  incr = range / (float)(ugp_size - 2); // skip 1st unit, and count end..
  //  incr -= .000001f;		// round-off tolerance..
}

// rep 1.5.  ugp_size = 4, incr = 1.5 / 3 = .5
// 0  .5   1
// oooo111122222 = val / incr

// 0 .5  1  val = .8, incr = .5
// 0 .4 .6
// (.4 * .5 + .6 * 1) / (.6 + .4) = .8

// act = 1.0 - (fabs(val - cur) / incr)


float ScalarValSpec::GetUnitAct(int unit_idx) {
  int eff_idx = unit_idx - 1;
  if(rep == GAUSSIAN) {
    float cur = min + incr * (float)eff_idx;
    float dist = (cur - val) / un_width_eff;
    return taMath_float::exp_fast(-(dist * dist));
  }
  else if(rep == LOCALIST) {
    float cur = min + incr * (float)eff_idx;
    if(fabs(val - cur) > incr) return 0.0f;
    return 1.0f - (fabs(val - cur) / incr);
  }
  return 0.0f;			// compiler food
}

float ScalarValSpec::GetUnitVal(int unit_idx) {
  int eff_idx = unit_idx - 1;
  float cur = min + incr * (float)eff_idx;
  return cur;
}

void ScalarValBias::Initialize() {
  un = NO_UN;
  un_shp = VAL;
  un_gain = 1.0f;
  wt = NO_WT;
  val = 0.0f;
  wt_gain = 1.0f;
}

void ScalarValLayerSpec::Initialize() {
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .9f;

  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    unit_range.min = -0.5f;   unit_range.max = 1.5f;
    unit_range.UpdateAfterEdit();
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else if(scalar.rep == ScalarValSpec::LOCALIST) {
    unit_range.min = 0.0f;  unit_range.max = 1.0f;
    unit_range.UpdateAfterEdit();
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit();
}

void ScalarValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  unit_range.UpdateAfterEdit();
  scalar.UpdateAfterEdit();
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else {
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit();
}

void ScalarValLayerSpec::HelpConfig() {
  String help = "ScalarValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent a single scalar value.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current scalar value is displayed in the first unit in the layer, which can be clamped\
 and compared, etc (i.e., set the environment patterns to have just one unit and provide\
 the actual scalar value and it will automatically establish the appropriate distributed\
 representation in the rest of the units).  This first unit is only viewable as act_eq,\
 not act, because it must not send activation to other units.\n\
 \nScalarValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the ScalarValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool ScalarValLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
		"coarse-coded scalar representation requires at least 3 units, I just set un_geom.n")) {
    if(scalar.rep == ScalarValSpec::LOCALIST) {
      lay->un_geom.n = 4;
      lay->un_geom.x = 4;
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      lay->un_geom.n = 12;
      lay->un_geom.x = 12;
    }
  }

  if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k = 1;		// localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == ScalarValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
		  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
		  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
		"scalar val layer doesn't have any units:", lay->name)) {
    return false;		// fatal
  }
  
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
		    "scalar val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
	cs->SetUnique("wt_scale", true);
	cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
		    "scalar val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
	cs->SetUnique("lrate", true);
	cs->lrate = 0.0f;
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void ScalarValLayerSpec::ReConfig(Network* net, int n_units) {
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if(lay->spec.SPtr() != this) continue;
    
    if(n_units > 0) {
      lay->un_geom.n = n_units;
      lay->un_geom.x = n_units;
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
    
    if(scalar.rep == ScalarValSpec::LOCALIST) {
      scalar.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = 0.0f; unit_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	   cs->InheritsFrom(TA_MarkerConSpec)) {
	  continue;
	}
	cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
	cs->rnd.mean = 0.1f;
	cs->rnd.var = 0.0f;
	cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0; 
      }
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = -.5f; unit_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	   cs->InheritsFrom(TA_MarkerConSpec)) {
	  continue;
	}
	cs->lmix.err_sb = true;
	cs->rnd.mean = 0.1f;
	cs->rnd.var = 0.0f;
	cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0; 
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void ScalarValLayerSpec::Compute_WtBias_Val(Unit_Group* ugp, float val) {
  if(ugp->size < 3) return;	// must be at least a few units..
  scalar.InitVal(val, ugp->size, unit_range.min, unit_range.range);
  int i;
  for(i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = .03f * bias_val.wt_gain * scalar.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	 cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->cons.size;ci++) {
	LeabraCon* cn = (LeabraCon*)recv_gp->Cn(ci);
	cn->wt += act;
	if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
	if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void ScalarValLayerSpec::Compute_UnBias_Val(Unit_Group* ugp, float val) {
  if(ugp->size < 3) return;	// must be at least a few units..
  scalar.InitVal(val, ugp->size, unit_range.min, unit_range.range);
  int i;
  for(i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = bias_val.un_gain * scalar.GetUnitAct(i);
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.Cn(0)->wt = act;
  }
}

void ScalarValLayerSpec::Compute_UnBias_NegSlp(Unit_Group* ugp) {
  if(ugp->size < 3) return;	// must be at least a few units..
  float val = 0.0f;
  float incr = bias_val.un_gain / (float)(ugp->size - 2);
  int i;
  for(i=1;i<ugp->size;i++, val += incr) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_a = val;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.Cn(0)->wt = -val;
  }
}

void ScalarValLayerSpec::Compute_UnBias_PosSlp(Unit_Group* ugp) {
  if(ugp->size < 3) return;	// must be at least a few units..
  float val = bias_val.un_gain;
  float incr = bias_val.un_gain / (float)(ugp->size - 2);
  int i;
  for(i=1;i<ugp->size;i++, val -= incr) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_h = val;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.Cn(0)->wt = val;
  }
}

void ScalarValLayerSpec::Compute_BiasVal(LeabraLayer* lay) {
  if(bias_val.un != ScalarValBias::NO_UN) {
    if(bias_val.un_shp == ScalarValBias::VAL) {
      UNIT_GP_ITR(lay, Compute_UnBias_Val(ugp, bias_val.val););
    }
    else if(bias_val.un_shp == ScalarValBias::NEG_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_NegSlp(ugp););
    }
    else if(bias_val.un_shp == ScalarValBias::POS_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_PosSlp(ugp););
    }
  }
  if(bias_val.wt == ScalarValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(ugp, bias_val.val););
  }
}

void ScalarValLayerSpec::Init_Weights(LeabraLayer* lay) {
  LeabraLayerSpec::Init_Weights(lay);
  Compute_BiasVal(lay);
  if(scalar.init_nms)
    LabelUnits(lay);
}

void ScalarValLayerSpec::Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_NetinScale(lay, net);
  if(lay->hard_clamped) return;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
    u->clmp_net -= u->bias_scale * u->bias.Cn(0)->wt;

    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    u->bias_scale /= 100.0f; 		  // keep a constant scaling so it doesn't depend on network size!
    u->clmp_net += u->bias_scale * u->bias.Cn(0)->wt;
  }
}

void ScalarValLayerSpec::Compute_ActAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts.avg = 0.0f;
  thr->acts.max = -FLT_MAX;
  thr->acts.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf == 0) { lf++; continue; }
    thr->acts.avg += u->act_eq;
    if(u->act_eq > thr->acts.max) {
      thr->acts.max = u->act_eq;  thr->acts.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 1) thr->acts.avg /= (float)(ug->leaves - 1);
}

void ScalarValLayerSpec::Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf == 0) { lf++; continue; }
    thr->acts_m.avg += u->act_m;
    if(u->act_m > thr->acts_m.max) {
      thr->acts_m.max = u->act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 1) thr->acts_m.avg /= (float)(ug->leaves - 1);
}

void ScalarValLayerSpec::Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf == 0) { lf++; continue; }
    thr->acts_p.avg += u->act_p;
    if(u->act_p > thr->acts_p.max) {
      thr->acts_p.max = u->act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 1) thr->acts_p.avg /= (float)(ug->leaves - 1);
}

void ScalarValLayerSpec::ClampValue(Unit_Group* ugp, LeabraNetwork*, float rescale) {
  if(ugp->size < 3) return;	// must be at least a few units..
  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
  LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
  if(!clamp.hard)
    u->UnSetExtFlag(Unit::EXT);
  else
    u->SetExtFlag(Unit::EXT);
  float val = u->ext;
  if(scalar.clip_val)
    val = val_range.Clip(val);		// first unit has the value to clamp
  scalar.InitVal(val, ugp->size, unit_range.min, unit_range.range);
  int i;
  for(i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = rescale * scalar.GetUnitAct(i);
    if(act < us->opt_thresh.send)
      act = 0.0f;
    u->SetExtFlag(Unit::EXT);
    u->ext = act;
  }
}

float ScalarValLayerSpec::ClampAvgAct(int ugp_size) {
  if(ugp_size < 3) return 0.0f;
  float val = val_range.min + .5f * val_range.Range(); // half way
  scalar.InitVal(val, ugp_size, unit_range.min, unit_range.range);
  float sum = 0.0f;
  int i;
  for(i=1;i<ugp_size;i++) {
    float act = scalar.GetUnitAct(i);
    sum += act;
  }
  sum /= (float)(ugp_size - 1);
  return sum;
}

float ScalarValLayerSpec::ReadValue(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return 0.0f;	// must be at least a few units..

  scalar.InitVal(0.0f, ugp->size, unit_range.min, unit_range.range);
  float avg = 0.0f;
  float sum_act = 0.0f;
  int i;
  for(i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float cur = scalar.GetUnitVal(i);
    float act_val = 0.0f;
    if(!scalar.send_thr || (u->act_eq >= us->opt_thresh.send)) // only if over sending thresh!
      act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
    avg += cur * act_val;
    sum_act += act_val;
  }
  sum_act = MAX(sum_act, scalar.min_sum_act);
  if(sum_act > 0.0f)
    avg /= sum_act;
  // set the first unit in the group to represent the value
  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
  u->act_eq = u->act_nd = avg;
  u->act = 0.0f;		// very important to clamp act to 0: don't send!
  u->da = 0.0f;			// don't contribute to change in act
  return u->act_eq;
}

void ScalarValLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  if(ugp->size < 3) return;	// must be at least a few units..
  scalar.InitVal(0.0f, ugp->size, unit_range.min, unit_range.range);
  for(int i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float cur = scalar.GetUnitVal(i);
    u->name = (String)cur;
  }
  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
  u->name = "val";		// overall value
}

void ScalarValLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(ugp); );
}

void ScalarValLayerSpec::LabelUnitsNet(Network* net) {
  LeabraLayer* l;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, l, net->layers., li) {
    if(l->spec.SPtr() == this)
      LabelUnits(l);
  }
}

void ScalarValLayerSpec::ResetAfterClamp(LeabraLayer* lay, LeabraNetwork*) {
  UNIT_GP_ITR(lay, 
	      if(ugp->size > 2) {
		LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
		u->act = 0.0f;		// must reset so it doesn't contribute!
		u->act_eq = u->act_nd = u->ext;	// avoid clamp_range!
	      }
	      );
}

void ScalarValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_HardClamp(lay, net);
  ResetAfterClamp(lay, net);
}

void ScalarValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(scalar.clamp_pat) {
    LeabraLayerSpec::Compute_HardClamp(lay, net);
    return;
  }
  if(!(lay->ext_flag & Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, if(ugp->size > 2) { ClampValue(ugp, net); } );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void ScalarValLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_Act_impl(lay, ug, thr, net);
  ReadValue(ug, net);		// always read out the value
}

void ScalarValLayerSpec::Compute_dWt_Ugp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net) {
  if(scalar.val_mult_lrn) {
    // because it is very hard to actually change the lrate, we are just moving
    // the act_m value toward act_p to decrease the effective lrate: this is equivalent
    // for the delta-rule..
    LeabraUnit* scu = (LeabraUnit*)ugp->FastEl(0);
    float val_dif_mult = 1.0f - fabs(scu->act_p - scu->act_m);
    if(val_dif_mult < 0.0f) val_dif_mult = 0.0f;
    for(int i=1;i<ugp->size;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      // if dif = 0, val_dif_mult = 1 = move all the way to act_p; else move just a bit..
      u->act_m += val_dif_mult * (u->act_p - u->act_m);
      u->Compute_dWt(lay, net);
    }
  }
  else {
    for(int i=1;i<ugp->size;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      u->Compute_dWt(lay, net);
    }
  }
}

void ScalarValLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule != LeabraNetwork::LEABRA_CHL) {
    if(lay->sravg_m_sum == 0.0f) return; // if nothing, nothing!
    lay->sravg_m_nrm = 1.0f / lay->sravg_m_sum;
    if(lay->sravg_s_sum > 0.0f) 
      lay->sravg_s_nrm = 1.0f / lay->sravg_s_sum;
    else
      lay->sravg_s_nrm = 1.0f;	// whatever
  }
  UNIT_GP_ITR(lay, 
	      Compute_dWt_Ugp(ugp, lay, net);
	      );
  AdaptKWTAPt(lay, net);
}

float ScalarValLayerSpec::Compute_SSE_Ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals) {
  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(u->ext_flag & (Unit::TARG | Unit::COMP) && val_range.RangeTestEq(u->targ)) {
    n_vals++;
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_SSE(LeabraLayer* lay, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay, 
	      lay->sse += Compute_SSE_Ugp(ugp, lay, n_vals);
	      );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float ScalarValLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ug,
					   LeabraInhib* thr, LeabraNetwork* net) {
  LeabraUnit* u = (LeabraUnit*)ug->FastEl(0);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(u->ext_flag & (Unit::TARG | Unit::COMP) && val_range.RangeTestEq(u->targ)) {
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return fabsf(uerr);
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;					 // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      nerr += Compute_NormErr_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      ntot += unit_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
    ntot += unit_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;		// no contributarse

  return lay->norm_err;
}


//////////////////////////////////
// 	Scalar Value Self Prjn	//
//////////////////////////////////

void ScalarValSelfPrjnSpec::Initialize() {
  init_wts = true;
  width = 3;
  wt_width = 2.0f;
  wt_max = 1.0f;
}

void ScalarValSelfPrjnSpec::Connect_UnitGroup(Unit_Group* gp, Projection* prjn) {
  float neigh1 = 1.0f / wt_width;
  float val1 = expf(-(neigh1 * neigh1));
  float scale_val = wt_max / val1;


  int n_cons = 2*width + 1;

  int i;
  for(i=0;i<gp->size;i++) {
    Unit* ru = (Unit*)gp->FastEl(i);

    ru->ConnectAlloc(n_cons, prjn);

    int j;
    for(j=-width;j<=width;j++) {
      int sidx = i+j;
      if((sidx < 0) || (sidx >= gp->size)) continue;
      Unit* su = (Unit*)gp->FastEl(sidx);
      if(!self_con && (ru == su)) continue;
      Connection* cn = ru->ConnectFromCk(su, prjn);
      if(cn != NULL) {
	float dist = (float)j / wt_width;
	float wtval = scale_val * expf(-(dist * dist));
	cn->wt = wtval;
      }
    }
  }
}

void ScalarValSelfPrjnSpec::Connect_impl(Projection* prjn) {
  if(!prjn->from)	return;
  if(TestError(prjn->from.ptr() != prjn->layer, "Connect_impl", "must be used as a self-projection!")) {
    return;
  }

  Layer* lay = prjn->layer;
  UNIT_GP_ITR(lay, Connect_UnitGroup(ugp, prjn); );
}

void ScalarValSelfPrjnSpec::C_Init_Weights(Projection*, RecvCons* cg, Unit* ru) {
  float neigh1 = 1.0f / wt_width;
  float val1 = expf(-(neigh1 * neigh1));
  float scale_val = wt_max / val1;

  int ru_idx = ((Unit_Group*)ru->owner)->FindEl(ru);

  for(int i=0; i<cg->cons.size; i++) {
    Unit* su = cg->Un(i);
    int su_idx = ((Unit_Group*)su->owner)->FindEl(su);
    float dist = (float)(ru_idx - su_idx) / wt_width;
    float wtval = scale_val * expf(-(dist * dist));
    cg->Cn(i)->wt = wtval;
  }
}

//////////////////////////////////
// 	MotorForceLayerSpec	//
//////////////////////////////////

void MotorForceSpec::Initialize() {
  pos_width = .2f;
  vel_width = .2f;
  norm_width = true;
  clip_vals = true;

  cur_pos = cur_vel = 0.0f;
  pos_min = vel_min = 0.0f;
  pos_range = vel_range = 1.0f;

  pos_width_eff = pos_width;
  vel_width_eff = vel_width;
}

void MotorForceSpec::InitRanges(float pos_min_, float pos_range_, float vel_min_, float vel_range_) {
  pos_min = pos_min_;
  pos_range = pos_range_;
  vel_min = vel_min_;
  vel_range = vel_range_;
  pos_width_eff = pos_width;
  vel_width_eff = vel_width;
  if(norm_width) {
    pos_width_eff *= pos_range;
    vel_width_eff *= vel_range;
  }
}

void MotorForceSpec::InitVals(float pos, int pos_size, float pos_min_, float pos_range_,
			      float vel, int vel_size, float vel_min_, float vel_range_) {
  InitRanges(pos_min_, pos_range_, vel_min_, vel_range_);
  cur_pos = pos;
  pos_incr = pos_range / (float)(pos_size-1);
  cur_vel = vel;
  vel_incr = vel_range / (float)(vel_size-1);
}

float MotorForceSpec::GetWt(int pos_gp_idx, int vel_gp_idx) {
  float ug_pos = pos_min + pos_incr * (float)pos_gp_idx;
  float pos_dist = (ug_pos - cur_pos) / pos_width_eff;
  float ug_vel = vel_min + vel_incr * (float)vel_gp_idx;
  float vel_dist = (ug_vel - cur_vel) / vel_width_eff;
  return taMath_float::exp_fast(-(pos_dist * pos_dist + vel_dist * vel_dist));
}

void MotorForceLayerSpec::Initialize() {
  pos_range.min = 0.0f;
  pos_range.max = 2.0f;
  vel_range.min = -.1f;
  vel_range.max = .1f;
  add_noise = true;
  force_noise.type = Random::GAUSSIAN;
  force_noise.var = .01f;
}

void MotorForceLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  pos_range.UpdateAfterEdit();
  vel_range.UpdateAfterEdit();
  force_noise.UpdateAfterEdit();
}

bool MotorForceLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);
  if(!rval) return rval;
  
  if(lay->CheckError(!lay->unit_groups, quiet, rval,
		"requires unit groups -- I just set it for you")) {
    lay->unit_groups = true;
  }
  if(lay->CheckError(lay->gp_geom.x < 3, quiet, rval,
		"requires at least 3 unit groups in x axis -- I just set it for you")) {
    lay->gp_geom.x = 5;
  }
  if(lay->CheckError(lay->gp_geom.y < 3, quiet, rval,
		"requires at least 3 unit groups in y axis -- I just set it for you")) {
    lay->gp_geom.y = 5;
  }
  return rval;
}

float MotorForceLayerSpec::ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel) {
  if(motor_force.clip_vals) {
    pos = pos_range.Clip(pos);
    vel = vel_range.Clip(vel);
  }
  motor_force.InitVals(pos, lay->gp_geom.x, pos_range.min, pos_range.range,
		       vel, lay->gp_geom.y, vel_range.min, vel_range.range);

  float force = 0.0f;
  float wt_sum = 0.0f;
  for(int y=0; y<lay->gp_geom.y; y++) {
    for(int x=0; x<lay->gp_geom.x; x++) {
      float wt = motor_force.GetWt(x,y);
      Unit_Group* ug = lay->FindUnitGpFmCoord(x, y);
      if(!ug || ug->size == 0) continue;
      LeabraUnit* un0 = (LeabraUnit*)ug->FastEl(0);
      force += wt * un0->act_eq;
      wt_sum += wt;
    }
  }
  if(wt_sum > 0.0f)
    force /= wt_sum;
  if(add_noise)
    force += force_noise.Gen();
  return force;
}

void MotorForceLayerSpec::ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel) {
  if(motor_force.clip_vals) {
    pos = pos_range.Clip(pos);
    vel = vel_range.Clip(vel);
  }
  motor_force.InitVals(pos, lay->gp_geom.x, pos_range.min, pos_range.range,
		       vel, lay->gp_geom.y, vel_range.min, vel_range.range);

  for(int y=0; y<lay->gp_geom.y; y++) {
    for(int x=0; x<lay->gp_geom.x; x++) {
      float wt = motor_force.GetWt(x,y);
      Unit_Group* ugp = lay->FindUnitGpFmCoord(x, y);
      if(!ugp || ugp->size == 0) continue;
      LeabraUnit* un0 = (LeabraUnit*)ugp->FastEl(0);
      un0->ext = force;
      ClampValue(ugp, net, wt);
    }
  }
  lay->SetExtFlag(Unit::EXT);
  lay->hard_clamped = clamp.hard;
  HardClampExt(lay, net);
  scalar.clamp_pat = true;	// must have this to keep this clamped val
  UNIT_GP_ITR(lay, 
	      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
	      u->ext = 0.0f;		// must reset so it doesn't contribute!
	      );
}

void MotorForceLayerSpec::Compute_BiasVal(LeabraLayer* lay) {
  float vel_mid = .5f * (float)(lay->gp_geom.y-1);
  float pos_mid = .5f * (float)(lay->gp_geom.x-1);
  for(int y=0; y<lay->gp_geom.y; y++) {
    float vel_dist = -((float)y - vel_mid) / vel_mid;
    for(int x=0; x<lay->gp_geom.x; x++) {
      float pos_dist = -((float)x - pos_mid) / pos_mid;
      Unit_Group* ugp = lay->FindUnitGpFmCoord(x, y);
      if(!ugp || ugp->size == 0) continue;
      float sum_val = .5f * vel_dist + .5f * pos_dist;

      if(bias_val.un != ScalarValBias::NO_UN) {
	Compute_UnBias_Val(ugp, sum_val);
      }
      if(bias_val.wt == ScalarValBias::WT) {
	Compute_WtBias_Val(ugp, sum_val);
      }
    }
  }
}

//////////////////////////////////
// 	TwoD Value Layer	//
//////////////////////////////////

void TwoDValSpec::Initialize() {
  rep = GAUSSIAN;
  n_vals = 1;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  min_sum_act = 0.2f;
  mn_dst = 0.5f;
  clip_val = true;

  x_min = x_val = y_min = y_val = 0.0f;
  x_range = x_incr = y_range = y_incr = 1.0f;
  x_size = y_size = 1;
  un_width_x = un_width_y = un_width;
}

void TwoDValSpec::InitRange(float xmin, float xrng, float ymin, float yrng) {
  x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng;
  un_width_x = un_width;
  un_width_y = un_width;
  if(norm_width) {
    un_width_x *= x_range;
    un_width_y *= y_range;
  }
}

void TwoDValSpec::InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
  InitRange(xmin, xrng, ymin, yrng);
  x_val = xval; y_val = yval;
  x_size = xsize; y_size = ysize;
  x_incr = x_range / (float)(x_size - 1); // DON'T skip 1st row, and count end..
  y_incr = y_range / (float)(y_size - 2); // skip 1st row, and count end..
  //  incr -= .000001f;		// round-off tolerance..
}

float TwoDValSpec::GetUnitAct(int unit_idx) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size) - 1; // get rid of first row..
  if(rep == GAUSSIAN) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float x_dist = (x_cur - x_val) / un_width_x;
    float y_cur = y_min + y_incr * (float)y_idx;
    float y_dist = (y_cur - y_val) / un_width_y;
    float dist = x_dist * x_dist + y_dist * y_dist;
    return expf(-dist);
  }
  else if(rep == LOCALIST) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float y_cur = y_min + y_incr * (float)y_idx;
    float x_dist = fabs(x_val - x_cur);
    float y_dist = fabs(y_val - y_cur);
    if((x_dist > x_incr) && (y_dist > y_incr)) return 0.0f;
    
    return 1.0f - .5 * ((x_dist / x_incr) + (y_dist / y_incr)); // todo: no idea if this is right.
  }
  return 0.0f;
}

void TwoDValSpec::GetUnitVal(int unit_idx, float& x_cur, float& y_cur) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size) - 1; // get rid of first row..
  x_cur = x_min + x_incr * (float)x_idx;
  y_cur = y_min + y_incr * (float)y_idx;
}

void TwoDValBias::Initialize() {
  un = NO_UN;
  un_gain = 1.0f;
  wt = NO_WT;
  wt_gain = 1.0f;
  x_val = 0.0f;
  y_val = 0.0f;
}

void TwoDValLayerSpec::Initialize() {
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 9;
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 9;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;

  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit();
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else if(twod.rep == TwoDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit(); y_val_range.UpdateAfterEdit();
}

void TwoDValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  x_range.UpdateAfterEdit(); y_range.UpdateAfterEdit();
  twod.UpdateAfterEdit();
  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else {
    x_val_range.min = x_range.min;    y_val_range.min = y_range.min;
    x_val_range.max = x_range.max;    y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit(); y_val_range.UpdateAfterEdit();
}

void TwoDValLayerSpec::HelpConfig() {
  String help = "TwoDValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent two-dimensional values.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current twod value is displayed in the first row of units in the layer (x1,y1, x2,y2, etc), which can be clamped\
 and compared, etc (i.e., set the environment patterns to have just the first row of units and provide\
 the actual twod value and it will automatically establish the appropriate distributed\
 representation in the rest of the units).  This first row is only viewable as act_eq,\
 not act, because it must not send activation to other units.\n\
 \nTwoDValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the TwoDValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool TwoDValLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
		"coarse-coded twod representation requires at least 3 units, I just set un_geom.n")) {
    if(twod.rep == TwoDValSpec::LOCALIST) {
      lay->un_geom.n = 12;
      lay->un_geom.x = 3;
      lay->un_geom.y = 4;
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      lay->un_geom.n = 132;
      lay->un_geom.x = 11;
      lay->un_geom.y = 12;
    }
  }

  if(twod.rep == TwoDValSpec::LOCALIST) {
    kwta.k = 1;		// localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == TwoDValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
		  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
		  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
		"twod val layer doesn't have any units:", lay->name)) {
    return false;		// fatal
  }
    
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
		    "twod val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
	cs->SetUnique("wt_scale", true);
	cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
		    "twod val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
	cs->SetUnique("lrate", true);
	cs->lrate = 0.0f;
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void TwoDValLayerSpec::ReConfig(Network* net, int n_units) {
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if(lay->spec.SPtr() != this) continue;
    
    if(n_units > 0) {
      lay->un_geom.n = n_units;
      lay->un_geom.x = n_units;
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
    
    if(twod.rep == TwoDValSpec::LOCALIST) {
      twod.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = TwoDValBias::GC; bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	   cs->InheritsFrom(TA_MarkerConSpec)) {
	  continue;
	}
	cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
	cs->rnd.mean = 0.1f;
	cs->rnd.var = 0.0f;
	cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0; 
      }
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = TwoDValBias::GC;  bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
	LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
	if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	   cs->InheritsFrom(TA_MarkerConSpec)) {
	  continue;
	}
	cs->lmix.err_sb = true;
	cs->rnd.mean = 0.1f;
	cs->rnd.var = 0.0f;
	cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0; 
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void TwoDValLayerSpec::Compute_WtBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  int i;
  for(i=lay->un_geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = .03f * bias_val.wt_gain * twod.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	 cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->cons.size;ci++) {
	LeabraCon* cn = (LeabraCon*)recv_gp->Cn(ci);
	cn->wt += act;
	if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
	if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void TwoDValLayerSpec::Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  int i;
  for(i=lay->un_geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = bias_val.un_gain * twod.GetUnitAct(i);
    if(bias_val.un == TwoDValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == TwoDValBias::BWT)
      u->bias.Cn(0)->wt = act;
  }
}

void TwoDValLayerSpec::Compute_BiasVal(LeabraLayer* lay) {
  if(bias_val.un != TwoDValBias::NO_UN) {
    UNIT_GP_ITR(lay, Compute_UnBias_Val(ugp, bias_val.x_val, bias_val.y_val););
  }
  if(bias_val.wt == TwoDValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(ugp, bias_val.x_val, bias_val.y_val););
  }
}

void TwoDValLayerSpec::Init_Weights(LeabraLayer* lay) {
  LeabraLayerSpec::Init_Weights(lay);
  Compute_BiasVal(lay);
}

void TwoDValLayerSpec::Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_NetinScale(lay, net);
  if(lay->hard_clamped) return;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
    u->clmp_net -= u->bias_scale * u->bias.Cn(0)->wt;

    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    u->bias_scale /= 100.0f; 		  // keep a constant scaling so it doesn't depend on network size!
    u->clmp_net += u->bias_scale * u->bias.Cn(0)->wt;
  }
}

void TwoDValLayerSpec::Compute_ActAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts.avg = 0.0f;
  thr->acts.max = -FLT_MAX;
  thr->acts.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf < lay->un_geom.x) { lf++; continue; }
    thr->acts.avg += u->act_eq;
    if(u->act_eq > thr->acts.max) {
      thr->acts.max = u->act_eq;  thr->acts.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > lay->un_geom.x) thr->acts.avg /= (float)(ug->leaves - lay->un_geom.x);
}

void TwoDValLayerSpec::Compute_ActMAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf < lay->un_geom.x) { lf++; continue; }
    thr->acts_m.avg += u->act_m;
    if(u->act_m > thr->acts_m.max) {
      thr->acts_m.max = u->act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > lay->un_geom.x) thr->acts.avg /= (float)(ug->leaves - lay->un_geom.x);
}

void TwoDValLayerSpec::Compute_ActPAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf < lay->un_geom.x) { lf++; continue; }
    thr->acts_p.avg += u->act_p;
    if(u->act_p > thr->acts_p.max) {
      thr->acts_p.max = u->act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > lay->un_geom.x) thr->acts.avg /= (float)(ug->leaves - lay->un_geom.x);
}

void TwoDValLayerSpec::ClampValue(Unit_Group* ugp, LeabraNetwork*, float rescale) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  // first initialize to zero
  for(int i=lay->un_geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->SetExtFlag(Unit::EXT);
    u->ext = 0.0;
  }
  for(int k=0;k<twod.n_vals;k++) {
    LeabraUnit* x_u = (LeabraUnit*)ugp->FastEl(k*2);
    LeabraUnit* y_u = (LeabraUnit*)ugp->FastEl(k*2+1);
    LeabraUnitSpec* us = (LeabraUnitSpec*)x_u->GetUnitSpec();
    x_u->SetExtFlag(Unit::EXT); y_u->SetExtFlag(Unit::EXT);
    float x_val = x_u->ext;
    float y_val = y_u->ext;
    if(twod.clip_val) {
      x_val = x_val_range.Clip(x_u->ext);
      y_val = y_val_range.Clip(y_u->ext);
    }
    twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
    for(int i=lay->un_geom.x;i<ugp->size;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      float act = rescale * twod.GetUnitAct(i);
      if(act < us->opt_thresh.send)
	act = 0.0f;
      u->ext += act;
    }
  }
}

void TwoDValLayerSpec::ReadValue(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  twod.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  if(twod.n_vals == 1) {	// special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=lay->un_geom.x;i<ugp->size;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      float x_cur, y_cur;  twod.GetUnitVal(i, x_cur, y_cur);
      float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
      x_avg += x_cur * act_val;
      y_avg += y_cur * act_val;
      sum_act += act_val;
    }
    sum_act = MAX(sum_act, twod.min_sum_act);
    if(sum_act > 0.0f) {
      x_avg /= sum_act; y_avg /= sum_act;
    }
    // set the first units in the group to represent the value
    LeabraUnit* x_u = (LeabraUnit*)ugp->FastEl(0);
    LeabraUnit* y_u = (LeabraUnit*)ugp->FastEl(1);
    x_u->act_eq = x_u->act_nd = x_avg;  x_u->act = 0.0f;  x_u->da = 0.0f;	
    y_u->act_eq = y_u->act_nd = y_avg;  y_u->act = 0.0f;  y_u->da = 0.0f;
    for(int i=2;i<lay->un_geom.x;i++) {	// reset the rest!
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      u->act_eq = u->act_nd = u->act = 0.0f; u->da = 0.0f;
    }
  }
  else {			// multiple items
    // first find the max values, using sum of -1..+1 region
    static ValIdx_Array sort_ary;
    sort_ary.Reset();
    for(int i=lay->un_geom.x;i<ugp->size;i++) {
      float sum = 0.0f;
      float nsum = 0.0f;
      for(int x=-1;x<=1;x++) {
	for(int y=-1;y<=1;y++) {
	  int idx = i + y * lay->un_geom.x + x;
	  if(idx < lay->un_geom.x || idx >= ugp->size) continue;
	  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(idx);
	  LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
	  float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
	  nsum += 1.0f;
	  sum += act_val;
	}
      }
      if(nsum > 0.0f) sum /= nsum;
      ValIdx vi(sum, i);
      sort_ary.Add(vi);
    }
    sort_ary.Sort();
    float mn_x = twod.mn_dst * twod.un_width_x * x_range.Range();
    float mn_y = twod.mn_dst * twod.un_width_y * y_range.Range();
    float mn_dist = mn_x * mn_x + mn_y * mn_y;
    int outi = 0;  int j = 0;
    while((outi < twod.n_vals) && (j < sort_ary.size)) {
      ValIdx& vi = sort_ary[sort_ary.size - j - 1]; // going backward through sort_ary
      float x_cur, y_cur;  twod.GetUnitVal(vi.idx, x_cur, y_cur);
      // check distance from all previous!
      float my_mn = x_range.Range() + y_range.Range();
      for(int k=0; k<j; k++) {
	ValIdx& vo = sort_ary[sort_ary.size - k - 1];
	if(vo.val == -1.0f) continue; // guy we skipped over before
	float x_prv, y_prv;  twod.GetUnitVal(vo.idx, x_prv, y_prv);
	float x_d = x_cur - x_prv; float y_d = y_cur - y_prv; 
	float dist = x_d * x_d + y_d * y_d;
	my_mn = MIN(dist, my_mn);
      }
      if(my_mn < mn_dist) { vi.val = -1.0f; j++; continue; } // mark with -1 so we know we skipped it
      LeabraUnit* x_u = (LeabraUnit*)ugp->FastEl(outi*2);
      LeabraUnit* y_u = (LeabraUnit*)ugp->FastEl(outi*2 + 1);
      x_u->act_eq = x_u->act_nd = x_cur;  x_u->act = 0.0f;  x_u->da = 0.0f;	
      y_u->act_eq = y_u->act_nd = y_cur;  y_u->act = 0.0f;  y_u->da = 0.0f;
      j++; outi++;
    }
    for(int i=2 * twod.n_vals;i<lay->un_geom.x;i++) {	// reset the rest!
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      u->act_eq = u->act_nd = u->act = 0.0f; u->da = 0.0f;
    }
  }
}

void TwoDValLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  twod.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=lay->un_geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float x_cur, y_cur; twod.GetUnitVal(i, x_cur, y_cur);
    u->name = (String)x_cur + "," + String(y_cur);
  }
}

void TwoDValLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(ugp); );
}

void TwoDValLayerSpec::LabelUnitsNet(Network* net) {
  LeabraLayer* l;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, l, net->layers., li) {
    if(l->spec.SPtr() == this)
      LabelUnits(l);
  }
}

void TwoDValLayerSpec::ResetAfterClamp(LeabraLayer* lay, LeabraNetwork*) {
  UNIT_GP_ITR(lay, 
	      if(ugp->size > 2) {
		for(int i=0; i<lay->un_geom.x; i++) {
		  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
		  u->act = 0.0f;		// must reset so it doesn't contribute!
		  u->act_eq = u->act_nd = u->ext;	// avoid clamp_range!
		}
	      }
	      );
}

void TwoDValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_HardClamp(lay, net);
  ResetAfterClamp(lay, net);
}

void TwoDValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(twod.clamp_pat) {
    LeabraLayerSpec::Compute_HardClamp(lay, net);
    return;
  }
  if(!(lay->ext_flag & Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, if(ugp->size > 2) { ClampValue(ugp, net); } );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void TwoDValLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_Act_impl(lay, ug, thr, net);
  ReadValue(ug, net);		// always read out the value
}

void TwoDValLayerSpec::Compute_dWtUgp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net) {
  for(int i=lay->un_geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->Compute_dWt(lay, net);
  }
}

void TwoDValLayerSpec::Compute_dWt_impl(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->learn_rule != LeabraNetwork::LEABRA_CHL) {
    if(lay->sravg_m_sum == 0.0f) return; // if nothing, nothing!
    lay->sravg_m_nrm = 1.0f / lay->sravg_m_sum;
    if(lay->sravg_s_sum > 0.0f) 
      lay->sravg_s_nrm = 1.0f / lay->sravg_s_sum;
    else
      lay->sravg_s_nrm = 1.0f;	// whatever
  }
  UNIT_GP_ITR(lay, 
	      Compute_dWtUgp(ugp, lay, net);
	      );
  AdaptKWTAPt(lay, net);
}

float TwoDValLayerSpec::Compute_SSE_Ugp(Unit_Group* ugp, LeabraLayer* lay, int& n_vals) {
  float rval = 0.0f;
  for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
    LeabraUnit* x_tu = (LeabraUnit*)ugp->FastEl(k*2);
    LeabraUnit* y_tu = (LeabraUnit*)ugp->FastEl(k*2+1);
    LeabraUnitSpec* us = (LeabraUnitSpec*)x_tu->GetUnitSpec();
    // only count if target value is within range -- otherwise considered a non-target
    if((x_tu->ext_flag & (Unit::TARG | Unit::COMP)) && x_val_range.RangeTestEq(x_tu->targ) && 
       (y_tu->ext_flag & (Unit::TARG | Unit::COMP)) && y_val_range.RangeTestEq(y_tu->targ)) {
      n_vals++;

      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<twod.n_vals;j++) {
	LeabraUnit* x_u = (LeabraUnit*)ugp->FastEl(j*2);
	LeabraUnit* y_u = (LeabraUnit*)ugp->FastEl(j*2+1);
	float dx = x_tu->targ - x_u->act_m;
	float dy = y_tu->targ - y_u->act_m;
	if(fabsf(dx) < us->sse_tol) dx = 0.0f;
	if(fabsf(dy) < us->sse_tol) dy = 0.0f;
	float dist = dx * dx + dy * dy;
	if(dist < mn_dist)
	  mn_dist = dist;
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float TwoDValLayerSpec::Compute_SSE(LeabraLayer* lay, int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay, 
	      lay->sse += Compute_SSE_Ugp(ugp, lay, n_vals);
	      );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float TwoDValLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay, Unit_Group* ugp,
					   LeabraInhib* thr, LeabraNetwork* net) {
  float rval = 0.0f;
  for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
    LeabraUnit* x_tu = (LeabraUnit*)ugp->FastEl(k*2);
    LeabraUnit* y_tu = (LeabraUnit*)ugp->FastEl(k*2+1);
    LeabraUnitSpec* us = (LeabraUnitSpec*)x_tu->GetUnitSpec();
    // only count if target value is within range -- otherwise considered a non-target
    if((x_tu->ext_flag & (Unit::TARG | Unit::COMP)) && x_val_range.RangeTestEq(x_tu->targ) && 
       (y_tu->ext_flag & (Unit::TARG | Unit::COMP)) && y_val_range.RangeTestEq(y_tu->targ)) {
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<twod.n_vals;j++) {
	LeabraUnit* x_u = (LeabraUnit*)ugp->FastEl(j*2);
	LeabraUnit* y_u = (LeabraUnit*)ugp->FastEl(j*2+1);
	float dx = x_tu->targ - x_u->act_m;
	float dy = y_tu->targ - y_u->act_m;
	if(fabsf(dx) < us->sse_tol) dx = 0.0f;
	if(fabsf(dy) < us->sse_tol) dy = 0.0f;
	float dist = fabsf(dx) + fabsf(dy); // only diff from sse!
	if(dist < mn_dist)
	  mn_dist = dist;
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float TwoDValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;					 // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      nerr += Compute_NormErr_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      ntot += x_range.range + y_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
    ntot += x_range.range + y_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;		// no contributarse

  return lay->norm_err;
}

///////////////////////////////////////////////////////////////
//   DecodeTwoDValLayerSpec

void DecodeTwoDValLayerSpec::Initialize() {
}

void DecodeTwoDValLayerSpec::Compute_Inhib(LeabraLayer*, LeabraNetwork*) {
  return;			// do nothing!
}

void DecodeTwoDValLayerSpec::Compute_InhibAvg(LeabraLayer*, LeabraNetwork*) {
  return;
}

void DecodeTwoDValLayerSpec::Compute_Act_impl(LeabraLayer*, Unit_Group* ug, LeabraInhib*, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr ui;
  FOR_ITR_EL(LeabraUnit, u, ug->, ui) {
    if(u->recv.size == 0) continue;
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv[0];
    if(cg->cons.size == 0) continue;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    u->net = su->net;
    u->act = su->act;
    u->act_eq = su->act_eq;
    u->act_nd = su->act_nd;
  }
  ReadValue(ug, net);		// always read out the value
}

////////////////////////////////////////////////////////////
//	V1RFPrjnSpec

void V1RFPrjnSpec::Initialize() {
  init_wts = true;
  wrap = false;
  dog_surr_mult = 1.0f;
}

void V1RFPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rf_spec.name = name + "_rf_spec";
}

void V1RFPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(prjn->layer->units.gp.size == 0, "Connect_impl",
		 "requires recv layer to have unit groups!")) {
    return;
  }

  rf_spec.un_geom = prjn->layer->un_geom;
  rf_spec.InitFilters();	// this one call initializes all filter info once and for all!
  // renorm the dog net filter to 1 abs max!
  if(rf_spec.filter_type == GaborV1Spec::BLOB) {
    for(int i=0;i<rf_spec.blob_specs.size;i++) {
      DoGFilterSpec* df = (DoGFilterSpec*)rf_spec.blob_specs.FastEl(i);
      taMath_float::vec_norm_abs_max(&(df->net_filter));
    }
  }
  TestWarning(rf_spec.un_geom != prjn->layer->un_geom,
	      "number of filters from rf_spec:", (String)rf_spec.un_geom.n,
	      "does not match layer un_geom.n:", (String)prjn->layer->un_geom.n);

  TwoDCoord rf_width = rf_spec.rf_width;
  int n_cons = rf_width.Product();
  TwoDCoord rf_half_wd = rf_width / 2;
  TwoDCoord ru_geo = prjn->layer->gp_geom;

  TwoDCoord su_geo = prjn->from->un_geom;

  TwoDCoord ruc;
  for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
    for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {

      Unit_Group* ru_gp = prjn->layer->FindUnitGpFmCoord(ruc);
      if(ru_gp == NULL) continue;

      TwoDCoord su_st;
      if(wrap) {
	su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
	su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
      }
      else {
	su_st.x = (int)floor((float)ruc.x * rf_move.x);
	su_st.y = (int)floor((float)ruc.y * rf_move.y);
      }

      su_st.WrapClip(wrap, su_geo);
      TwoDCoord su_ed = su_st + rf_width;
      if(wrap) {
	su_ed.WrapClip(wrap, su_geo); // just wrap ends too
      }
      else {
	if(su_ed.x > su_geo.x) {
	  su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
	}
	if(su_ed.y > su_geo.y) {
	  su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
	}
      }

      for(int rui=0;rui<ru_gp->size;rui++) {
	Unit* ru_u = (Unit*)ru_gp->FastEl(rui);
	ru_u->ConnectAlloc(n_cons, prjn);

	TwoDCoord suc;
	TwoDCoord suc_wrp;
	for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
	  for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
	    suc_wrp = su_st + suc;
	    if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
	      continue;
	    Unit* su_u = prjn->from->FindUnitFmCoord(suc_wrp);
	    if(su_u == NULL) continue;
	    if(!self_con && (su_u == ru_u)) continue;
	    ru_u->ConnectFrom(su_u, prjn); // don't check: saves lots of time!
	  }
	}
      }
    }
  }
}

void V1RFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  Unit_Group* rugp = (Unit_Group*)ru->GetOwner();
  int recv_idx = ru->pos.y * rugp->geom.x + ru->pos.x;
  
  bool on_rf = true;
  if(prjn->from->name.contains("_off"))
    on_rf = false;
  DoGFilterSpec::ColorChannel col_chan = DoGFilterSpec::BLACK_WHITE;
  if(prjn->from->name.contains("_rg_"))
    col_chan = DoGFilterSpec::RED_GREEN;
  else if(prjn->from->name.contains("_by_"))
    col_chan = DoGFilterSpec::BLUE_YELLOW;

  int send_x = rf_spec.rf_width.x;
  if(rf_spec.filter_type == GaborV1Spec::BLOB) {
    // color is outer-most dimension, and if it doesn't match, then bail
    int clr_dx = (recv_idx / (rf_spec.blob_rf.n_sizes * 2) % 2);
    DoGFilterSpec* df = (DoGFilterSpec*)rf_spec.blob_specs.SafeEl(recv_idx);
    if(!df) return;		// oops
    if(col_chan != DoGFilterSpec::BLACK_WHITE) {
      // outer-most mod is color, after phases (2) and sizes (inner)
      if((clr_dx == 0 && col_chan == DoGFilterSpec::BLUE_YELLOW) ||
	 (clr_dx == 1 && col_chan == DoGFilterSpec::RED_GREEN)) {
	for(int i=0; i<cg->cons.size; i++)
	  cg->Cn(i)->wt = 0.0f;
	return;			// bail if not our channel.
      }
    }
    for(int i=0; i<cg->cons.size; i++) {
      int su_x = i % send_x;
      int su_y = i / send_x;
      float val = rf_spec.gabor_rf.amp * df->net_filter.SafeEl(su_x, su_y);
      if(on_rf) {
	if(df->on_sigma > df->off_sigma) val *= dog_surr_mult;
	if(val > 0.0f) cg->Cn(i)->wt = val;
	else	       cg->Cn(i)->wt = 0.0f;
      }
      else {
	if(df->off_sigma > df->on_sigma) val *= dog_surr_mult;
	if(val < 0.0f) 	cg->Cn(i)->wt = -val;
	else		cg->Cn(i)->wt = 0.0f;
      }
    }
  }
  else {			// GABOR
    GaborFilterSpec* gf = (GaborFilterSpec*)rf_spec.gabor_specs.SafeEl(recv_idx);
    if(!gf) return;		// oops
    for(int i=0; i<cg->cons.size; i++) {
      int su_x = i % send_x;
      int su_y = i / send_x;
      float val = gf->filter.SafeEl(su_x, su_y);
      if(on_rf) {
	if(val > 0.0f) cg->Cn(i)->wt = val;
	else	       cg->Cn(i)->wt = 0.0f;
      }
      else {
	if(val < 0.0f) 	cg->Cn(i)->wt = -val;
	else		cg->Cn(i)->wt = 0.0f;
      }
    }
  }
}

void V1RFPrjnSpec::GraphFilter(DataTable* graph_data, int recv_unit_no) {
  rf_spec.GraphFilter(graph_data, recv_unit_no);
}

void V1RFPrjnSpec::GridFilter(DataTable* graph_data) {
  rf_spec.GridFilter(graph_data);
}

////////////////////////////////////////////////////////////
//	SaliencyPrjnSpec

void SaliencyPrjnSpec::Initialize() {
  //  init_wts = true;
  convergence = 1;
  reciprocal = false;
  feat_only = true;
  feat_gps = 2;
  dog_wts.color_chan = DoGFilterSpec::BLACK_WHITE;
  dog_wts.filter_width = 3;
  dog_wts.filter_size = 7;
  dog_wts.on_sigma = 1;
  dog_wts.off_sigma = 2;
  wt_mult = 1.0f;
  surr_mult = 1.0f;
  units_per_feat_gp = 4;
}

void SaliencyPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(prjn->layer->units.gp.size == 0, "Connect_impl",
		 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(prjn->from->units.gp.size == 0, "Connect_impl",
		 "requires sending layer to have unit groups!")) {
    return;
  }
  if(feat_only)
    Connect_feat_only(prjn);
  else
    Connect_full_dog(prjn);
}

void SaliencyPrjnSpec::Connect_feat_only(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;

    Unit* su;
    taLeafItr su_itr;
    FOR_ITR_EL(Unit, su, send_lay->units., su_itr) {
      su->ConnectAlloc(1, prjn); // only ever have 1!
    }
  }

  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord ruu_geo = recv_lay->un_geom;
  TwoDCoord su_geo = send_lay->gp_geom;

  int fltsz = convergence;
  int sg_sz_tot = fltsz * fltsz;

  int feat_no = 0;
  TwoDCoord rug;
  for(rug.y = 0; rug.y < rug_geo.y; rug.y++) {
    for(rug.x = 0; rug.x < rug_geo.x; rug.x++, feat_no++) {
      Unit_Group* ru_gp = recv_lay->FindUnitGpFmCoord(rug);
      if(!ru_gp) continue;

      int rui = 0;
      TwoDCoord ruc;
      for(ruc.y = 0; ruc.y < ruu_geo.y; ruc.y++) {
	for(ruc.x = 0; ruc.x < ruu_geo.x; ruc.x++, rui++) {

	  TwoDCoord su_st = ruc*convergence;

	  Unit* ru_u = (Unit*)ru_gp->SafeEl(rui);
	  if(!ru_u) break;
	  if(!reciprocal)
	    ru_u->ConnectAlloc(sg_sz_tot, prjn);

	  TwoDCoord suc;
	  for(suc.y = 0; suc.y < fltsz; suc.y++) {
	    for(suc.x = 0; suc.x < fltsz; suc.x++) {
	      TwoDCoord sugc = su_st + suc;
	      Unit_Group* su_gp = send_lay->FindUnitGpFmCoord(sugc);
	      if(!su_gp) continue;

	      Unit* su_u = (Unit*)su_gp->SafeEl(feat_no);
	      if(su_u) {
		if(reciprocal)
		  su_u->ConnectFrom(ru_u, prjn);
		else
		  ru_u->ConnectFrom(su_u, prjn);
	      }
	    }
	  }
	}
      }
    }
  }
}

void SaliencyPrjnSpec::Connect_full_dog(Projection* prjn) {
  dog_wts.UpdateFilter();
  taMath_float::vec_norm_abs_max(&(dog_wts.net_filter)); // renorm to abs max = 1

  if(TestError(reciprocal, "Connect_full_dog", "full DoG connection not supported in reciprocal mode!!!")) return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord ruu_geo = recv_lay->un_geom;
  TwoDCoord su_geo = send_lay->gp_geom;

  int fltwd = dog_wts.filter_width; // no convergence..
  int fltsz = dog_wts.filter_size * convergence;

  int sg_sz_tot = fltsz * fltsz;
  Unit_Group* su_gp0 = (Unit_Group*)send_lay->units.gp[0];
  int alloc_no = sg_sz_tot * su_gp0->size;

  units_per_feat_gp = su_gp0->size / feat_gps;

  int feat_no = 0;
  TwoDCoord rug;
  for(rug.y = 0; rug.y < rug_geo.y; rug.y++) {
    for(rug.x = 0; rug.x < rug_geo.x; rug.x++, feat_no++) {
      Unit_Group* ru_gp = recv_lay->FindUnitGpFmCoord(rug);
      if(!ru_gp) continue;

      int rui = 0;
      TwoDCoord ruc;
      for(ruc.y = 0; ruc.y < ruu_geo.y; ruc.y++) {
	for(ruc.x = 0; ruc.x < ruu_geo.x; ruc.x++, rui++) {

	  TwoDCoord su_st = ruc*convergence - convergence*fltwd;

	  Unit* ru_u = (Unit*)ru_gp->SafeEl(rui);
	  if(!ru_u) break;
	  ru_u->ConnectAlloc(alloc_no, prjn);

	  TwoDCoord suc;
	  for(suc.y = 0; suc.y < fltsz; suc.y++) {
	    for(suc.x = 0; suc.x < fltsz; suc.x++) {
	      TwoDCoord sugc = su_st + suc;
	      Unit_Group* su_gp = send_lay->FindUnitGpFmCoord(sugc);
	      if(!su_gp) continue;

	      for(int sui=0;sui<su_gp->size;sui++) {
		Unit* su_u = (Unit*)su_gp->FastEl(sui);
		if(!self_con && (su_u == ru_u)) continue;
		ru_u->ConnectFrom(su_u, prjn);
	      }
	    }
	  }
	}
      }
    }
  }
}

void SaliencyPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(feat_only) {		// just use regular..
    inherited::C_Init_Weights(prjn, cg, ru);
    return;
  }
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int fltwd = dog_wts.filter_width; // no convergence.
  int fltsz = dog_wts.filter_size * convergence;

  Unit_Group* su_gp0 = (Unit_Group*)send_lay->units.gp[0];
  units_per_feat_gp = su_gp0->size / feat_gps;

  Unit_Group* rugp = (Unit_Group*)ru->GetOwner();

  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord rgp_pos = rugp->GetGpGeomPos();
  
  int feat_no = rgp_pos.y * rug_geo.x + rgp_pos.x; // unit group index
  int my_feat_gp = feat_no / units_per_feat_gp;
  int fg_st = my_feat_gp * units_per_feat_gp;
  int fg_ed = fg_st + units_per_feat_gp;

  TwoDCoord ruu_geo = recv_lay->un_geom;
  TwoDCoord su_st;		// su starting (left)
  su_st.x = (ru->idx % ruu_geo.x)*convergence - convergence*fltwd;
  su_st.y = (ru->idx / ruu_geo.x)*convergence - convergence*fltwd;

  TwoDCoord su_geo = send_lay->gp_geom;

  int su_idx = 0;
  TwoDCoord suc;
  for(suc.y = 0; suc.y < fltsz; suc.y++) {
    for(suc.x = 0; suc.x < fltsz; suc.x++) {
      TwoDCoord sugc = su_st + suc;
      Unit_Group* su_gp = send_lay->FindUnitGpFmCoord(sugc);
      if(!su_gp) continue;

      float wt = wt_mult * dog_wts.net_filter.FastEl(suc.x/convergence, 
						     suc.y/convergence);

      if(wt > 0) {
	for(int sui=0;sui<su_gp->size;sui++) {
	  if(sui == feat_no)
	    cg->Cn(su_idx++)->wt = wt; // target feature
	  else
	    cg->Cn(su_idx++)->wt = 0.0f; // everyone else
	}
      }
      else {
	for(int sui=0;sui<su_gp->size;sui++) {
	  if(sui != feat_no && sui >= fg_st && sui < fg_ed) 
	    cg->Cn(su_idx++)->wt = -surr_mult * wt;
	  else
	    cg->Cn(su_idx++)->wt = 0.0f; // not in our group or is guy itself
	}
      }
    }
  }
}

void SaliencyPrjnSpec::GraphFilter(DataTable* graph_data) {
  dog_wts.GraphFilter(graph_data);
}

void SaliencyPrjnSpec::GridFilter(DataTable* graph_data) {
  dog_wts.GridFilter(graph_data);
}

////////////////////////////////////////////////////////////
//	GpAggregatePrjnSpec

void GpAggregatePrjnSpec::Initialize() {
}

void GpAggregatePrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from)	return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(prjn->from->units.gp.size == 0, "Connect_impl",
		 "requires sending layer to have unit groups!")) {
    return;
  }
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord su_geo = send_lay->gp_geom;
  int n_su_gps = su_geo.Product();

  int alloc_no = n_su_gps; 	// number of cons per recv unit

  for(int ri = 0; ri<recv_lay->units.leaves; ri++) {
    Unit* ru_u = (Unit*)recv_lay->units.Leaf(ri);
    if(!ru_u) break;
    ru_u->ConnectAlloc(alloc_no, prjn);
    
    TwoDCoord suc;
    for(suc.y = 0; suc.y <= su_geo.y; suc.y++) {
      for(suc.x = 0; suc.x <= su_geo.x; suc.x++) {
	Unit_Group* su_gp = send_lay->FindUnitGpFmCoord(suc);
	if(!su_gp) continue;
	Unit* su_u = (Unit*)su_gp->SafeEl(ri);
	if(su_u) {
	  ru_u->ConnectFrom(su_u, prjn);
	}
      }
    }
  }
}

///////////////////////////////////////////////////////////////
//			V1 Layer
///////////////////////////////////////////////////////////////

void LeabraV1Layer::Initialize() {
}

void LeabraV1Layer::BuildUnits() {
  inherited::BuildUnits();
  // build feature unit groups
  feat_gps.gp.SetSize(un_geom.n); // one group per unit in unit groups
  for(int fg=0; fg<feat_gps.gp.size; fg++) {
    LeabraUnit_Group* fugp = (LeabraUnit_Group*)feat_gps.gp[fg];
    fugp->Reset();		// start over
    for(int gg=0; gg<units.gp.size; gg++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)units.gp[gg];
      if(rugp->size <= fg) continue;
      LeabraUnit* ru = (LeabraUnit*)rugp->FastEl(fg);
      fugp->Link(ru);
    }
  }
}

void LeabraV1Layer::ResetSortBuf() {
  inherited::ResetSortBuf();
  for(int fg=0; fg<feat_gps.gp.size; fg++) {
    LeabraUnit_Group* fugp = (LeabraUnit_Group*)feat_gps.gp[fg];
    fugp->Inhib_ResetSortBuf();
  }
}

void V1FeatInhibSpec::Initialize() {
  feat_gain = .1f;
  dist_sigma = .5f;
  i_rat_thr = .5f;
}

void LeabraV1LayerSpec::Initialize() {
  min_obj_type = &TA_LeabraV1Layer;
}

bool LeabraV1LayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;
  LeabraV1Layer* vlay = (LeabraV1Layer*)lay;

  bool rval = true;
  lay->CheckError(lay->units.gp.size == 0, quiet, rval,
		  "does not have unit groups -- MUST have unit groups!");
  vlay->CheckError(vlay->feat_gps.gp.size != vlay->un_geom.n, quiet, rval,
		   "not proper number of feature unit groups for layer size!");
  return rval;
}

void LeabraV1LayerSpec::Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fugp, 
					     LeabraNetwork* net) {
  fugp->active_buf.size = 0;
  for(int ui=0; ui<fugp->size; ui++) {
    LeabraUnit* u = (LeabraUnit*)fugp->FastEl(ui);
    LeabraUnit_Group* u_own = (LeabraUnit_Group*)u->owner; // NOT fugp!
    if(u->i_thr >= u_own->i_val.g_i) // compare to their own group's inhib val!
      fugp->active_buf.Add(u);
  }
}


void LeabraV1LayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  LeabraV1Layer* vlay = (LeabraV1Layer*)lay;

  float dst_norm_val = 1.0f / (feat_inhib.dist_sigma * (float)MAX(lay->gp_geom.x, lay->gp_geom.y));
  dst_norm_val *= dst_norm_val;	// using sq distances

  for(int fg=0; fg<vlay->feat_gps.gp.size; fg++) {
    LeabraUnit_Group* fugp = (LeabraUnit_Group*)vlay->feat_gps.gp[fg];
    // get active lists for each feature group
    Compute_FeatGpActive(vlay, fugp, net);

    for(int ui=0; ui<fugp->size; ui++) {
      LeabraUnit* u = (LeabraUnit*)fugp->FastEl(ui);
      LeabraUnit_Group* u_own = (LeabraUnit_Group*)u->owner; // NOT fugp!
      TwoDCoord up = u_own->GetGpGeomPos();

      float gp_i = u_own->i_val.g_i;
      if(gp_i <= 0) gp_i = .1f;	// note: should have min_i set!!

      // now compare each unit with all the active units and increase inhib in proportion!
      float sum_cost = 0.0f;
      if((u->i_thr / gp_i) > feat_inhib.i_rat_thr) { // only if this guy is even close to firing
	for(int ai=0; ai<fugp->active_buf.size; ai++) {
	  LeabraUnit* au = (LeabraUnit*)fugp->active_buf.FastEl(ai);
	  LeabraUnit_Group* au_own = (LeabraUnit_Group*)au->owner;
	  TwoDCoord aup = au_own->GetGpGeomPos();

	  float dst_sq = up.SqDist(aup);
	  float cost = taMath_float::exp_fast(-dst_sq * dst_norm_val);
	  sum_cost += cost;
	}
      }
      float ival = gp_i * (1.0f + feat_inhib.feat_gain * sum_cost);
      u->Compute_ApplyInhib(lay, (LeabraInhib*)u_own, net, ival);
    }    
  }
}


///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////


///////////////////////////////////////////////////////////////
//			SRN Context
///////////////////////////////////////////////////////////////

bool LeabraWizard::SRNContext(LeabraNetwork* net) {
  if(TestError(!net, "SRNContext", "must have basic constructed network first")) {
    return false;
  }
  OneToOnePrjnSpec* otop = (OneToOnePrjnSpec*)net->FindMakeSpec("CtxtPrjn", &TA_OneToOnePrjnSpec);
  LeabraContextLayerSpec* ctxts = (LeabraContextLayerSpec*)net->FindMakeSpec("CtxtLayerSpec", &TA_LeabraContextLayerSpec);

  if((otop == NULL) || (ctxts == NULL)) {
    return false;
  }

  LeabraLayer* hidden = (LeabraLayer*)net->FindLayer("Hidden");
  LeabraLayer* ctxt = (LeabraLayer*)net->FindMakeLayer("Context");
  
  if((hidden == NULL) || (ctxt == NULL)) return false;

  ctxt->SetLayerSpec(ctxts);
  ctxt->un_geom = hidden->un_geom;

  net->layers.MoveAfter(hidden, ctxt);
  net->FindMakePrjn(ctxt, hidden, otop); // one-to-one into the ctxt layer
  net->FindMakePrjn(hidden, ctxt); 	 // std prjn back into the hidden from context
  net->Build();
  return true;
}

