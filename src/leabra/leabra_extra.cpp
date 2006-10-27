// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
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

void LeabraContextLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(updt, this);
}

// void LeabraContextLayerSpec::UpdateAfterEdit() {
//   LeabraLayerSpec::UpdateAfterEdit();
//   hysteresis_c = 1.0f - hysteresis;
// }

bool LeabraContextLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::CheckError("LeabraContextLayerSpec requires trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
    rval = false;
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
    LeabraCon_Group* cg = (LeabraCon_Group*)u->recv.gp[0];
    if(cg == NULL) {
      taMisc::Error("*** LeabraContextLayerSpec requires one recv projection!");
      return;
    }
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(su == NULL) {
      taMisc::Error("*** LeabraContextLayerSpec requires one unit in recv projection!");
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
  lay->Inhib_SetVals(i_kwta_pt);		// assume 0 - 1 clamped inputs

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
  u->act = u->act_eq = act_range.Clip(new_act);
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

void SynDepSpec::Initialize() {
  rec = 1.0f;
  depl = 1.1f;
}

void TrialSynDepConSpec::Initialize() {
  min_con_type = &TA_TrialSynDepCon;
}

void TrialSynDepConSpec::InitLinks() {
  LeabraConSpec::InitLinks();
  taBase::Own(syn_dep, this);
}

void TrialSynDepConSpec::UpdateAfterEdit() {
  LeabraConSpec::UpdateAfterEdit();
  if(syn_dep.rec <= 0.0f)	// can't go to zero!
    syn_dep.rec = 1.0f;
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
  min_con_type = &TA_FastWtCon;
}

void FastWtConSpec::InitLinks() {
  LeabraConSpec::InitLinks();
  taBase::Own(fast_wt, this);
}

void FastWtConSpec::SetCurLrate(int epoch, LeabraNetwork* net) {
  LeabraConSpec::SetCurLrate(epoch, net);
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

void ActAvgHebbMixSpec::UpdateAfterEdit() {
  cur_act = 1.0f - act_avg;
}

void ActAvgHebbConSpec::UpdateAfterEdit() {
  LeabraConSpec::UpdateAfterEdit();
  act_avg_hebb.UpdateAfterEdit();
}

void ActAvgHebbConSpec::InitLinks() {
  LeabraConSpec::InitLinks();
  taBase::Own(act_avg_hebb, this);
}

void ActAvgHebbConSpec::Initialize() {
}


//////////////////////////////////
// 	Scalar Value Layer	//
//////////////////////////////////

void ScalarValSpec::Initialize() {
  rep = LOCALIST;
  un_width = .3f;
  min_net = .1f;
  clamp_pat = false;
  min_sum_act = 0.2f;
  val_mult_lrn = false;
  min = val = sb_ev = 0.0f;
  range = incr = 1.0f;
  sb_lt = 0;
}

void ScalarValSpec::InitVal(float sval, int ugp_size, float umin, float urng) {
  min = umin; range = urng;
  val = sval;
  if((rep == GAUSSIAN) || (rep == LOCALIST))
    incr = range / (float)(ugp_size - 2); // skip 1st unit, and count end..
  else
    incr = range / (float)(ugp_size - 1); // skip 1st unit
  //  incr -= .000001f;		// round-off tolerance..
  sb_lt = (int)floor((val - min) / incr);
  sb_ev = (val - (min + ((float)sb_lt * incr))) / incr;
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
    float dist = (cur - val) / un_width;
    return expf(-(dist * dist));
  }
  else if(rep == LOCALIST) {
    float cur = min + incr * (float)eff_idx;
    if(fabs(val - cur) > incr) return 0.0f;
    return 1.0f - (fabs(val - cur) / incr);
  }
  else {
    float rval;
    if(eff_idx < sb_lt) rval = 1.0f;
    else if(eff_idx > sb_lt) rval = 0.0f;
    else rval = sb_ev;
    return rval;
  }
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
  SetUnique("compute_i", true);
  compute_i = KWTA_AVG_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .9f;

  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    unit_range.min = -0.5f;   unit_range.max = 1.5f;
    unit_range.UpdateAfterEdit();
    val_range.min = unit_range.min + (.5f * scalar.un_width);
    val_range.max = unit_range.max - (.5f * scalar.un_width);
  }
  else if(scalar.rep == ScalarValSpec::LOCALIST) {
    unit_range.min = 0.0f;  unit_range.max = 1.0f;
    unit_range.UpdateAfterEdit();
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  else {			// SUM_BAR
    unit_range.min = 0.0f;   unit_range.max = 1.0f;
    unit_range.UpdateAfterEdit();
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit();
}

void ScalarValLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(scalar, this);
  taBase::Own(unit_range, this);
  taBase::Own(val_range, this);
  taBase::Own(bias_val, this);
}

void ScalarValLayerSpec::UpdateAfterEdit() {
  LeabraLayerSpec::UpdateAfterEdit();
  unit_range.UpdateAfterEdit();
  scalar.UpdateAfterEdit();
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    val_range.min = unit_range.min + (.5f * scalar.un_width);
    val_range.max = unit_range.max - (.5f * scalar.un_width);
  }
  else {
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit();
  if(scalar.rep == ScalarValSpec::SUM_BAR) {
    compute_i = UNIT_INHIB;
  }
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
  taMisc::Choice(help, "Ok");
}

bool ScalarValLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->n_units < 3) {
    rval = false;
    if(!quiet) taMisc::CheckError("ScalarValLayerSpec: coarse-coded scalar representation requires at least 3 units, I just set n_units");
    if(scalar.rep == ScalarValSpec::LOCALIST) {
      lay->n_units = 4;
      lay->geom.x = 4;
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      lay->n_units = 12;
      lay->geom.x = 12;
    }
    else if(scalar.rep == ScalarValSpec::SUM_BAR) {
      lay->n_units = 12;
      lay->geom.x = 12;
    }
  }

  if((scalar.rep == ScalarValSpec::SUM_BAR) && (compute_i != UNIT_INHIB)) {
    rval = false;
    compute_i = UNIT_INHIB;
    if(!quiet) taMisc::CheckError("ScalarValLayerSpec: SUM_BAR rep type requires compute_i = UNIT_INHIB, because it sets gc.i individually");
  }

  if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k = 1;		// localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == ScalarValBias::GC) {
    rval = false;
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
    if(us->hyst.init) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
      if(!quiet) taMisc::CheckError("ScalarValLayerSpec: bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
    if(us->acc.init) {
      rval = false;
      us->SetUnique("acc", true);
      us->acc.init = false;
      if(!quiet) taMisc::CheckError("ScalarValLayerSpec: bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  if(u == NULL) {
    taMisc::CheckError("Error: ScalarValLayerSpec: scalar val layer doesn't have any units:", lay->name);
    rval = false;
  }
    
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(cs->wt_scale.rel > 0.5f) {
        rval = false;
	cs->SetUnique("wt_scale", true);
	cs->wt_scale.rel = 0.1f;
	if(!quiet) taMisc::CheckError("ScalarValLayerSpec: scalar val self connections should have wt_scale < .5, I just set it to .1 for you in spec:",
		      cs->name,"(make sure this is appropriate for all connections that use this spec!)");
      }
      if(cs->lrate > 0.0f) {
        rval = false;
	cs->SetUnique("lrate", true);
	cs->lrate = 0.0f;
	if(!quiet) taMisc::CheckError("ScalarValLayerSpec: scalar val self connections should have lrate = 0, I just set it for you in spec:",
		      cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    else {
      if((scalar.rep == ScalarValSpec::SUM_BAR) && cs->lmix.err_sb) {
        rval = false;
	cs->SetUnique("lmix", true);
	cs->lmix.err_sb = false;
	if(!quiet) taMisc::CheckError("ScalarValLayerSpec: scalar val cons for SUM_BAR should have lmix.err_sb = false (are otherwise biased!), I just set it for you in spec:",
		      cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
  }
  return rval;
}

void ScalarValLayerSpec::ReConfig(Network* net, int n_units) {
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if(lay->spec.spec != this) continue;
    
    if(n_units > 0) {
      lay->n_units = n_units;
      lay->geom.x = n_units;
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
    
    if(scalar.rep == ScalarValSpec::SUM_BAR) {
      compute_i = UNIT_INHIB;
      bias_val.un = ScalarValBias::BWT;
      bias_val.un_shp = ScalarValBias::NEG_SLP;
      bias_val.wt = ScalarValBias::NO_WT;
      us->g_bar.h = .1f; us->g_bar.a = .1f;
      unit_range.min = 0.0; unit_range.max = 1.0f;

      LeabraCon_Group* recv_gp;
      int g;
      FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
	if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	   cs->InheritsFrom(TA_MarkerConSpec)) {
	  continue;
	}
	cs->lmix.err_sb = false;
	cs->rnd.mean = 0.1f;
      }
    }
    else if(scalar.rep == ScalarValSpec::LOCALIST) {
      scalar.min_sum_act = .2f;
      kwta.k = 1;
      compute_i = KWTA_AVG_INHIB;
      i_kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = 0.0f; unit_range.max = 1.0f;

      LeabraCon_Group* recv_gp;
      int g;
      FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
	if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
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
      compute_i = KWTA_INHIB;
      i_kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = -.5f; unit_range.max = 1.5f;

      LeabraCon_Group* recv_gp;
      int g;
      FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
	if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
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
    LeabraCon_Group* recv_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
      if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	 cs->InheritsFrom(TA_MarkerConSpec)) continue;
      int ci;
      for(ci=0;ci<recv_gp->size;ci++) {
	LeabraCon* cn = (LeabraCon*)recv_gp->Cn(ci);
	cn->wt += act;
	if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
	if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
	recv_gp->C_InitWtState_Post(cn, u, recv_gp->Un(ci));
      }
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
      u->bias->wt = act;
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
      u->bias->wt = -val;
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
      u->bias->wt = val;
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

void ScalarValLayerSpec::InitWtState(LeabraLayer* lay) {
  LeabraLayerSpec::InitWtState(lay);
  Compute_BiasVal(lay);
}

void ScalarValLayerSpec::Compute_NetScale(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_NetScale(lay, net);
  if(lay->hard_clamped) return;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    LeabraConSpec* bspec = (LeabraConSpec*)u->spec.spec->bias_spec.spec;
    u->clmp_net -= u->bias_scale * u->bias->wt;

    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    u->bias_scale /= 100.0f; 		  // keep a constant scaling so it doesn't depend on network size!
    u->clmp_net += u->bias_scale * u->bias->wt;
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
  LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
  u->SetExtFlag(Unit::EXT);
  float val = val_range.Clip(u->ext);		// first unit has the value to clamp
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
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
    float cur = scalar.GetUnitVal(i);
    float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
    avg += cur * act_val;
    sum_act += act_val;
  }
  sum_act = MAX(sum_act, scalar.min_sum_act);
  if(sum_act > 0.0f)
    avg /= sum_act;
  // set the first unit in the group to represent the value
  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
  if((scalar.rep == ScalarValSpec::GAUSSIAN) || (scalar.rep == ScalarValSpec::LOCALIST))
    u->act_eq = avg;
  else
    u->act_eq = scalar.min + scalar.incr * sum_act;
  u->act = 0.0f;		// very important to clamp act to 0: don't send!
  u->da = 0.0f;			// don't contribute to change in act
  return u->act_eq;
}

void ScalarValLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  if(ugp->size < 3) return;	// must be at least a few units..
  scalar.InitVal(0.0f, ugp->size, unit_range.min, unit_range.range);
  int i;
  for(i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float cur = scalar.GetUnitVal(i);
    u->name = (String)cur;
  }
}

void ScalarValLayerSpec::LabelUnits(LeabraLayer* lay) {
  UNIT_GP_ITR(lay, LabelUnits_impl(ugp); );
}

void ScalarValLayerSpec::LabelUnitsNet(Network* net) {
  LeabraLayer* l;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, l, net->layers., li) {
    if(l->spec.spec == this)
      LabelUnits(l);
  }
}

void ScalarValLayerSpec::ResetAfterClamp(LeabraLayer* lay, LeabraNetwork*) {
  UNIT_GP_ITR(lay, 
	      if(ugp->size > 2) {
		LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
		u->act = 0.0f;		// must reset so it doesn't contribute!
		u->act_eq = u->ext;	// avoid clamp_range!
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
  if(!(clamp.hard && (lay->ext_flag & Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }

  UNIT_GP_ITR(lay, if(ugp->size > 2) { ClampValue(ugp, net); } );
  HardClampExt(lay, net);
}

void ScalarValLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_Act_impl(lay, ug, thr, net);
  ReadValue(ug, net);		// always read out the value
}

void ScalarValLayerSpec::Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ugp, LeabraInhib* thr, LeabraNetwork* net) {
  if(scalar.rep != ScalarValSpec::SUM_BAR) {
    LeabraLayerSpec::Compute_Inhib_impl(lay, ugp, thr, net);
    return;
  }
  thr->i_val.g_i = 0.0f;	// make sure it's zero, cuz this gets added to units.. 
  thr->i_val.g_i_orig = thr->i_val.g_i;	// retain original values..
  if(ugp->size < 3) return;	// must be at least a few units..

  int i;
  for(i=1;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    //    LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
    float old_net = u->net;
    u->net = MIN(scalar.min_net, u->net);
    float ithr = u->Compute_IThreshNoAH(lay, net); // exclude AH here so they can be used as a modulator!
    if(ithr < 0.0f) ithr = 0.0f;
    u->net = old_net;
    u->gc.i = u->g_i_raw = ithr;
  }    
}

void ScalarValLayerSpec::Compute_dWtUgp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net) {
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

void ScalarValLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, 
	      Compute_dWtUgp(ugp, lay, net);
	      );
  AdaptKWTAPt(lay, net);
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

  int i;
  for(i=0;i<gp->size;i++) {
    Unit* ru = (Unit*)gp->FastEl(i);
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
  if(prjn->from == NULL)	return;
  if(prjn->from != prjn->layer) {
    taMisc::Error("Error: ScalarValSelfPrjnSpec must be used as a self-projection!");
    return;
  }

  Layer* lay = prjn->layer;
  UNIT_GP_ITR(lay, Connect_UnitGroup(ugp, prjn); );
}

void ScalarValSelfPrjnSpec::C_InitWtState(Projection*, Con_Group* cg, Unit* ru) {
  float neigh1 = 1.0f / wt_width;
  float val1 = expf(-(neigh1 * neigh1));
  float scale_val = wt_max / val1;

  int ru_idx = ((Unit_Group*)ru->owner)->Find(ru);

  int i;
  for(i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int su_idx = ((Unit_Group*)su->owner)->Find(su);
    float dist = (float)(ru_idx - su_idx) / wt_width;
    float wtval = scale_val * expf(-(dist * dist));
    cg->Cn(i)->wt = wtval;
  }
}


//////////////////////////////////
// 	TwoD Value Layer	//
//////////////////////////////////

void TwoDValSpec::Initialize() {
  rep = GAUSSIAN;
  n_vals = 1;
  un_width = .3f;
  clamp_pat = false;
  min_sum_act = 0.2f;
  mn_dst = 0.5f;
  x_min = x_val = y_min = y_val = 0.0f;
  x_range = x_incr = y_range = y_incr = 1.0f;
  x_size = y_size = 1;
}

void TwoDValSpec::InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
  x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng; x_val = xval; y_val = yval;
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
    float x_dist = (x_cur - x_val) / un_width;
    float y_cur = y_min + y_incr * (float)y_idx;
    float y_dist = (y_cur - y_val) / un_width;
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
  SetUnique("compute_i", true);
  compute_i = KWTA_AVG_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .6f;

  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit();
    x_val_range.min = x_range.min + (.5f * twod.un_width); x_val_range.max = x_range.max - (.5f * twod.un_width);
    y_val_range.min = y_range.min + (.5f * twod.un_width); y_val_range.max = y_range.max - (.5f * twod.un_width);
  }
  else if(twod.rep == TwoDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit(); y_val_range.UpdateAfterEdit();
}

void TwoDValLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(twod, this);
  taBase::Own(x_range, this);
  taBase::Own(y_range, this);
  taBase::Own(x_val_range, this);
  taBase::Own(y_val_range, this);
  taBase::Own(bias_val, this);
}

void TwoDValLayerSpec::UpdateAfterEdit() {
  LeabraLayerSpec::UpdateAfterEdit();
  x_range.UpdateAfterEdit(); y_range.UpdateAfterEdit();
  twod.UpdateAfterEdit();
  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    x_val_range.min = x_range.min + (.5f * twod.un_width);
    y_val_range.min = y_range.min + (.5f * twod.un_width);
    x_val_range.max = x_range.max - (.5f * twod.un_width);
    y_val_range.max = y_range.max - (.5f * twod.un_width);
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
  taMisc::Choice(help, "Ok");
}

bool TwoDValLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->n_units < 3) {
    rval = false;
    if(!quiet) taMisc::CheckError("TwoDValLayerSpec: coarse-coded twod representation requires at least 3 units, I just set n_units");
    if(twod.rep == TwoDValSpec::LOCALIST) {
      lay->n_units = 12;
      lay->geom.x = 3;
      lay->geom.y = 4;
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      lay->n_units = 132;
      lay->geom.x = 11;
      lay->geom.y = 12;
    }
  }

  if(twod.rep == TwoDValSpec::LOCALIST) {
    kwta.k = 1;		// localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == TwoDValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
    if(us->hyst.init) {
      rval = false;
      us->SetUnique("hyst", true);
      us->hyst.init = false;
      if(!quiet) taMisc::CheckError("TwoDValLayerSpec: bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
    if(us->acc.init) {
      rval = false;
      us->SetUnique("acc", true);
      us->acc.init = false;
      if(!quiet) taMisc::CheckError("TwoDValLayerSpec: bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  if(u == NULL) {
    if (!quiet)
    taMisc::CheckError("Error: TwoDValLayerSpec: twod val layer doesn't have any units:", lay->name);
    rval = false;
  }
    
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(cs->wt_scale.rel > 0.5f) {
        rval = false;
	cs->SetUnique("wt_scale", true);
	cs->wt_scale.rel = 0.1f;
	if(!quiet) taMisc::CheckError("TwoDValLayerSpec: twod val self connections should have wt_scale < .5, I just set it to .1 for you in spec:",
		      cs->name,"(make sure this is appropriate for all connections that use this spec!)");
      }
      if(cs->lrate > 0.0f) {
        rval = false;
	cs->SetUnique("lrate", true);
	cs->lrate = 0.0f;
	if(!quiet) taMisc::CheckError("TwoDValLayerSpec: twod val self connections should have lrate = 0, I just set it for you in spec:",
		      cs->name,"(make sure this is appropriate for all layers that use this spec!)");
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
    if(lay->spec.spec != this) continue;
    
    if(n_units > 0) {
      lay->n_units = n_units;
      lay->geom.x = n_units;
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
    
    if(twod.rep == TwoDValSpec::LOCALIST) {
      twod.min_sum_act = .2f;
      kwta.k = 1;
      compute_i = KWTA_AVG_INHIB;
      i_kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = TwoDValBias::GC; bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

      LeabraCon_Group* recv_gp;
      int g;
      FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
	if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
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
      compute_i = KWTA_INHIB;
      i_kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = TwoDValBias::GC;  bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

      LeabraCon_Group* recv_gp;
      int g;
      FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
	if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
	LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
	if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
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
  twod.InitVal(x_val, y_val, lay->geom.x, lay->geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  int i;
  for(i=lay->geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = .03f * bias_val.wt_gain * twod.GetUnitAct(i);
    LeabraCon_Group* recv_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
      if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
	 cs->InheritsFrom(TA_MarkerConSpec)) continue;
      int ci;
      for(ci=0;ci<recv_gp->size;ci++) {
	LeabraCon* cn = (LeabraCon*)recv_gp->Cn(ci);
	cn->wt += act;
	if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
	if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
	recv_gp->C_InitWtState_Post(cn, u, recv_gp->Un(ci));
      }
    }
  }
}

void TwoDValLayerSpec::Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  twod.InitVal(x_val, y_val, lay->geom.x, lay->geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  int i;
  for(i=lay->geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = bias_val.un_gain * twod.GetUnitAct(i);
    if(bias_val.un == TwoDValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == TwoDValBias::BWT)
      u->bias->wt = act;
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

void TwoDValLayerSpec::InitWtState(LeabraLayer* lay) {
  LeabraLayerSpec::InitWtState(lay);
  Compute_BiasVal(lay);
}

void TwoDValLayerSpec::Compute_NetScale(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_NetScale(lay, net);
  if(lay->hard_clamped) return;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    LeabraConSpec* bspec = (LeabraConSpec*)u->spec.spec->bias_spec.spec;
    u->clmp_net -= u->bias_scale * u->bias->wt;

    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    u->bias_scale /= 100.0f; 		  // keep a constant scaling so it doesn't depend on network size!
    u->clmp_net += u->bias_scale * u->bias->wt;
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
    if(lf < lay->geom.x) { lf++; continue; }
    thr->acts.avg += u->act_eq;
    if(u->act_eq > thr->acts.max) {
      thr->acts.max = u->act_eq;  thr->acts.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > lay->geom.x) thr->acts.avg /= (float)(ug->leaves - lay->geom.x);
}

void TwoDValLayerSpec::Compute_ActMAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf < lay->geom.x) { lf++; continue; }
    thr->acts_m.avg += u->act_m;
    if(u->act_m > thr->acts_m.max) {
      thr->acts_m.max = u->act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > lay->geom.x) thr->acts.avg /= (float)(ug->leaves - lay->geom.x);
}

void TwoDValLayerSpec::Compute_ActPAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    if(lf < lay->geom.x) { lf++; continue; }
    thr->acts_p.avg += u->act_p;
    if(u->act_p > thr->acts_p.max) {
      thr->acts_p.max = u->act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > lay->geom.x) thr->acts.avg /= (float)(ug->leaves - lay->geom.x);
}

void TwoDValLayerSpec::ClampValue(Unit_Group* ugp, LeabraNetwork*, float rescale) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  // first initialize to zero
  for(int i=lay->geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->SetExtFlag(Unit::EXT);
    u->ext = 0.0;
  }
  for(int k=0;k<twod.n_vals;k++) {
    LeabraUnit* x_u = (LeabraUnit*)ugp->FastEl(k*2);
    LeabraUnit* y_u = (LeabraUnit*)ugp->FastEl(k*2+1);
    LeabraUnitSpec* us = (LeabraUnitSpec*)x_u->spec.spec;
    x_u->SetExtFlag(Unit::EXT); y_u->SetExtFlag(Unit::EXT);
    float x_val = x_val_range.Clip(x_u->ext);
    float y_val = y_val_range.Clip(y_u->ext);
    twod.InitVal(x_val, y_val, lay->geom.x, lay->geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
    for(int i=lay->geom.x;i<ugp->size;i++) {
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
  twod.InitVal(0.0f, 0.0f, lay->geom.x, lay->geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  if(twod.n_vals == 1) {	// special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=lay->geom.x;i<ugp->size;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
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
    x_u->act_eq = x_avg;  x_u->act = 0.0f;  x_u->da = 0.0f;	
    y_u->act_eq = y_avg;  y_u->act = 0.0f;  y_u->da = 0.0f;
    for(int i=2;i<lay->geom.x;i++) {	// reset the rest!
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      u->act_eq = u->act = 0.0f; u->da = 0.0f;
    }
  }
  else {			// multiple items
    // first find the max values, using sum of -1..+1 region
    static ValIdx_Array sort_ary;
    sort_ary.Reset();
    for(int i=lay->geom.x;i<ugp->size;i++) {
      float sum = 0.0f;
      float nsum = 0.0f;
      for(int x=-1;x<=1;x++) {
	for(int y=-1;y<=1;y++) {
	  int idx = i + y * lay->geom.x + x;
	  if(idx < lay->geom.x || idx >= ugp->size) continue;
	  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(idx);
	  LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
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
    float mn_x = twod.mn_dst * twod.un_width * x_range.Range();
    float mn_y = twod.mn_dst * twod.un_width * y_range.Range();
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
      x_u->act_eq = x_cur;  x_u->act = 0.0f;  x_u->da = 0.0f;	
      y_u->act_eq = y_cur;  y_u->act = 0.0f;  y_u->da = 0.0f;
      j++; outi++;
    }
    for(int i=2 * twod.n_vals;i<lay->geom.x;i++) {	// reset the rest!
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      u->act_eq = u->act = 0.0f; u->da = 0.0f;
    }
  }
}

void TwoDValLayerSpec::LabelUnits_impl(Unit_Group* ugp) {
  if(ugp->size < 3) return;	// must be at least a few units..
  Layer* lay = ugp->own_lay;
  twod.InitVal(0.0f, 0.0f, lay->geom.x, lay->geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=lay->geom.x;i<ugp->size;i++) {
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
    if(l->spec.spec == this)
      LabelUnits(l);
  }
}

void TwoDValLayerSpec::ResetAfterClamp(LeabraLayer* lay, LeabraNetwork*) {
  UNIT_GP_ITR(lay, 
	      if(ugp->size > 2) {
		for(int i=0; i<lay->geom.x; i++) {
		  LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
		  u->act = 0.0f;		// must reset so it doesn't contribute!
		  u->act_eq = u->ext;	// avoid clamp_range!
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
  if(!(clamp.hard && (lay->ext_flag & Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }

  UNIT_GP_ITR(lay, if(ugp->size > 2) { ClampValue(ugp, net); } );
  HardClampExt(lay, net);
}

void TwoDValLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  LeabraLayerSpec::Compute_Act_impl(lay, ug, thr, net);
  ReadValue(ug, net);		// always read out the value
}

void TwoDValLayerSpec::Compute_dWtUgp(Unit_Group* ugp, LeabraLayer* lay, LeabraNetwork* net) {
  for(int i=lay->geom.x;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->Compute_dWt(lay, net);
  }
}

void TwoDValLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, 
	      Compute_dWtUgp(ugp, lay, net);
	      );
  AdaptKWTAPt(lay, net);
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
    if(u->recv.gp.size == 0) continue;
    LeabraCon_Group* cg = (LeabraCon_Group*)u->recv.gp[0];
    if(cg->size == 0) continue;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    u->net = su->net;
    u->act = su->act;
    u->act_eq = su->act_eq;
  }
  ReadValue(ug, net);		// always read out the value
}

void DecodeTwoDValLayerSpec::Compute_dWt(LeabraLayer*, LeabraNetwork*) {
  return; 			// no need to do this!
}

////////////////////////////////////////////////////////////
//

void GaborRFSpec::Initialize() {
  n_angles = 4;
  freq = 1.0f;
  width = 2.0f;
  length = 4.0f;
  amp = 1.0f;
}

void BlobRFSpec::Initialize() {
  n_sizes = 2;
  wdth_st = 2.0;
  wdth_inc = 2.0;
}

void V1RFPrjnSpec::Initialize() {
  init_wts = true;
}

void V1RFPrjnSpec::InitLinks() {
  TiledRFPrjnSpec::InitLinks();
  taBase::Own(gabor_rf, this);
  taBase::Own(blob_rf, this);
  taBase::Own(gabor_spec, this);
  taBase::Own(dog_spec, this);
}

// todo: new dog filter not yet supported!!!

bool V1RFPrjnSpec::InitGaborDoGSpec(Projection* prjn, int recv_idx) {
  if(!InitRFSizes(prjn)) return false;

  if(recv_idx < gabor_rf.n_angles * 2) { // gabor
    float phase_dx = (float)((recv_idx / gabor_rf.n_angles) % 2);
    float angle_dx = (float)(recv_idx % gabor_rf.n_angles);

    gabor_spec.x_size = rf_width.x;
    gabor_spec.y_size = rf_width.y;
    gabor_spec.ctr_x = (float)(gabor_spec.x_size - 1.0f) / 2.0f;
    gabor_spec.ctr_y = (float)(gabor_spec.y_size - 1.0f) / 2.0f;
    gabor_spec.angle = taMath_float::pi * angle_dx / (float)gabor_rf.n_angles;
    gabor_spec.phase = taMath_float::pi * phase_dx;
    gabor_spec.freq = gabor_rf.freq;
    gabor_spec.length = gabor_rf.length;
    gabor_spec.width = gabor_rf.width;
    gabor_spec.amp = gabor_rf.amp;
  }
  else {			// dog/blob
    /*
    int dog_idx = recv_idx - (gabor_rf.n_angles * 2);
    int phase_dx = ((dog_idx / blob_rf.n_sizes) % 2);
    float sz_dx = (float)(dog_idx % blob_rf.n_sizes);

    dog_spec.filter_size = rf_width.x; // assume symmetric!
    if(phase_dx == 0) {
      dog_spec.on_sigma = blob_rf.wdth_st + blob_rf.wdth_inc * sz_dx;
      dog_spec.off_sigma = 2.0f * dog_spec.on_sigma;
    }
    else {
      dog_spec.off_sigma = blob_rf.wdth_st + blob_rf.wdth_inc * sz_dx;
      dog_spec.on_sigma = 2.0f * dog_spec.off_sigma;
    }
//     cerr << "wdth: " << dog_spec.filter_width << ", on" << dog_spec.on_sigma << ", off: " << 
//       dog_spec.off_sigma << endl;
    MakeDoGFilter();
//     cerr << dog_spec.filter.size << endl;
*/
  }
  return true;
}

void V1RFPrjnSpec::MakeDoGFilter() {
  /*  dog_spec.filter.Reset();
  float flt_ctr = (dog_spec.filter_size - 1.0f) / 2.0f;
  float max_val = 0.0f;
  int x,y;
  for(y=0; y<dog_spec.filter_size; y++) {
    for(x=0; x<dog_spec.filter_size; x++) {
      float xf = (float)x - flt_ctr;
      float yf = (float)y - flt_ctr;
      float val = xf * xf + yf * yf;
      float ong = 1.0f / (4.0 * PI * dog_spec.on_sigma * dog_spec.on_sigma) * exp(-val / (2.0 * dog_spec.on_sigma * dog_spec.on_sigma));
      float offg = 1.0f / (4.0 * PI * dog_spec.off_sigma * dog_spec.off_sigma) * exp(-val / (2.0 * dog_spec.off_sigma * dog_spec.off_sigma));
//       float ong = exp(-val / (2.0 * dog_spec.on_sigma * dog_spec.on_sigma));
//       float offg = exp(-val / (2.0 * dog_spec.off_sigma * dog_spec.off_sigma));
      float net = ong - offg;
      if(fabs(net) > max_val)
	max_val = fabs(net);
      dog_spec.filter.Add(net);
    }
  }
  float rescale = gabor_rf.amp / max_val;
  int i;
  for(i=0;i<dog_spec.filter.size;i++) {
    dog_spec.filter[i] *= rescale;
  }
  */
  //  cerr << "+ sum: " << pos_sum_2 << " - sum: " << neg_sum_2 << endl;
}

void V1RFPrjnSpec::C_InitWtState(Projection* prjn, Con_Group* cg, Unit* ru) {
  Unit_Group* rugp = (Unit_Group*)ru->GetOwner(&TA_Unit_Group);
  int recv_idx = ru->pos.y * rugp->geom.x + ru->pos.x;

  if(!InitGaborDoGSpec(prjn, recv_idx)) return;

  bool on_rf = true;
  if(prjn->from->name.contains("_off"))
    on_rf = false;

  int send_x = gabor_spec.x_size;

  int i;
  for(i=0; i<cg->size; i++) {
    int su_x = i % send_x;
    int su_y = i / send_x;
    float val = 0.0;

    if(recv_idx < gabor_rf.n_angles * 2) { // gabor
      val = gabor_spec.Eval(su_x, su_y);
    }
//     else {
//       val = dog_spec.filter[i];
//     }
    if(on_rf) {
      if(val > 0.0f)
	cg->Cn(i)->wt = val;
      else
	cg->Cn(i)->wt = 0.0f;
    }
    else {
      if(val < 0.0f)
	cg->Cn(i)->wt = -val;
      else
	cg->Cn(i)->wt = 0.0f;
    }
  }
}

// void V1RFPrjnSpec::GraphFilter(Projection* prjn, int recv_unit_no, GraphLog* graph_log) {
//   if(!InitGaborDoGSpec(prjn, recv_unit_no)) return;
//   gabor_spec.GraphFilter(graph_log);
// }

// void V1RFPrjnSpec::GridFilter(Projection* prjn, int recv_unit_no, GridLog* disp_log) {
//   if(!InitGaborDoGSpec(prjn, recv_unit_no)) return;
//   gabor_spec.GridFilter(disp_log);
// }

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////


///////////////////////////////////////////////////////////////
//			SRN Context
///////////////////////////////////////////////////////////////

void LeabraWizard::SRNContext(LeabraNetwork* net) {
  if(net == NULL) {
    taMisc::Error("SRNContext: must have basic constructed network first");
    return;
  }
  OneToOnePrjnSpec* otop = (OneToOnePrjnSpec*)net->FindMakeSpec("CtxtPrjn", &TA_OneToOnePrjnSpec);
  LeabraContextLayerSpec* ctxts = (LeabraContextLayerSpec*)net->FindMakeSpec("CtxtLayerSpec", &TA_LeabraContextLayerSpec);

  if((otop == NULL) || (ctxts == NULL)) {
    return;
  }

  LeabraLayer* hidden = (LeabraLayer*)net->FindLayer("Hidden");
  LeabraLayer* ctxt = (LeabraLayer*)net->FindMakeLayer("Context");
  
  if((hidden == NULL) || (ctxt == NULL)) return;

  ctxt->SetLayerSpec(ctxts);
  ctxt->n_units = hidden->n_units;
  ctxt->geom = hidden->geom;

  net->layers.MoveAfter(hidden, ctxt);
  net->FindMakePrjn(ctxt, hidden, otop); // one-to-one into the ctxt layer
  net->FindMakePrjn(hidden, ctxt); 	 // std prjn back into the hidden from context
  net->Build();
  net->Connect();
}

