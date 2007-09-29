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

#include "leabra_td.h"

// #include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////////////
// 	DaMod Units and Cons		//
//////////////////////////////////////////

void DaModUnit::Initialize() {
  act_m2 = 0.0f;
  act_p2 = 0.0f;
  p_act_m = -.01f;
  p_act_p = -.01f;
  dav = 0.0f;
}

void DaModUnit::Copy_(const DaModUnit& cp) {
  act_m2 = cp.act_m2;
  act_p2 = cp.act_p2;
  p_act_p = cp.p_act_p;
  p_act_m = cp.p_act_m;
  dav = cp.dav;
}

void DaModSpec::Initialize() {
  on = false;
  mod = PLUS_CONT;
  gain = .1f;
  neg_rec = .2f;
  p_dwt = false;
}

void DaModUnitSpec::Initialize() {
  min_obj_type = &TA_DaModUnit;
}

void DaModUnitSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void DaModUnitSpec::Init_Acts(LeabraUnit* u, LeabraLayer* lay) {
  inherited::Init_Acts(u, lay);
  DaModUnit* lu = (DaModUnit*)u;
  lu->act_m2 = 0.0f;
  lu->act_p2 = 0.0f;
  if(da_mod.p_dwt) {
    lu->p_act_m = -.01f;
    lu->p_act_p = -.01f;
  }
  lu->dav = 0.0f;
}

void DaModUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  if(da_mod.on) {
    if(da_mod.mod == DaModSpec::PLUS_CONT) {
      // note: u->misc_1 contains maintenance currents in PFC units: g_h should always be set to this!
      DaModUnit* lu = (DaModUnit*)u;
      if(net->phase == LeabraNetwork::PLUS_PHASE) {
	if(lu->dav > 0.0f) {
	  lu->vcb.g_a = 0.0f;
	  lu->vcb.g_h = u->misc_1 + da_mod.gain * lu->dav * lu->act_m; // increase in proportion to participation in minus phase
	}
	else {
	  lu->vcb.g_h = u->misc_1;
	  lu->vcb.g_a = -da_mod.gain * lu->dav * lu->act_m; // decrease in proportion to participation in minus phase
	}
      }
      else {
	lu->vcb.g_h = u->misc_1;
	lu->vcb.g_a = 0.0f;	// clear in minus phase!
      }
    }
    else if(da_mod.mod == DaModSpec::NEG_DIP) {
      if((net->phase == LeabraNetwork::PLUS_PHASE) && (net->phase_no < 2)) {
	if(net->cycle == 0) { // right at the beginning of the plus phase!
	  DaModUnit* lu = (DaModUnit*)u;
	  float new_neg = 0.0f;
	  if(lu->dav < 0.0f) {
	    new_neg = -da_mod.gain * lu->dav * lu->act_eq;
	  }
	  lu->vcb.g_a = lu->vcb.g_a + new_neg - da_mod.neg_rec * lu->vcb.g_a;
	}
      }
    }
  }

  inherited::Compute_Conduct(u, lay, thr, net);
}

void DaModUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(!da_mod.p_dwt) {
    inherited::Compute_dWt(u, lay, net);
    return;
  }
  DaModUnit* lu = (DaModUnit*)u;
  if((lu->act_p <= opt_thresh.learn) && (lu->act_m <= opt_thresh.learn)) {
    if((lu->p_act_p <= opt_thresh.learn) && (lu->p_act_m <= opt_thresh.learn))
      return;
  }
  if(lay->phase_dif_ratio < opt_thresh.phase_dif)
    return;
  Compute_dWt_impl(u, lay, net);
}

void DaModUnitSpec::Compute_Weights(Unit* u) {
  if(!da_mod.p_dwt) {
    inherited::Compute_Weights(u);
    return;
  }
  DaModUnit* lu = (DaModUnit*)u;
  ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_Weights((LeabraCon*)u->bias.Cn(0), lu, this);
  if(opt_thresh.updt_wts && 
     ((lu->act_p <= opt_thresh.learn) && (lu->act_m <= opt_thresh.learn)) &&
      ((lu->p_act_p <= opt_thresh.learn) && (lu->p_act_m <= opt_thresh.learn)))
    return;
  UnitSpec::Compute_Weights(lu);
}

void DaModUnitSpec::EncodeState(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  inherited::EncodeState(u, lay, net);
  DaModUnit* lu = (DaModUnit*)u;
  if(da_mod.p_dwt) {
    // just save phase activation states
    if(net->phase_max >= 3)
      lu->p_act_p = lu->act_p2;
    else
      lu->p_act_p = lu->act_p;
    if(net->phase_max >= 4)
      lu->p_act_m = lu->act_m2;
    else
      lu->p_act_m = lu->act_m;
  }
}

void DaModUnitSpec::DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay) {
  inherited::DecayEvent(u, lay, net, decay);
  DaModUnit* lu = (DaModUnit*)u;
  lu->dav = 0.0f;
}

void DaModUnitSpec::PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			       LeabraNetwork* net, bool set_both)
{
  inherited::PostSettle(u, lay, thr, net, set_both);
  DaModUnit* lu = (DaModUnit*)u;

  if((net->phase == LeabraNetwork::MINUS_PHASE) && (net->phase_no < 2)) {
    lu->act_m2 = lu->act_m;	// set this just in case..
  }
  if((net->phase == LeabraNetwork::PLUS_PHASE) && (net->phase_no < 2)) {
    if(da_mod.on && (da_mod.mod == DaModSpec::PLUS_POST)) {
      float dact = da_mod.gain * lu->dav * lu->act_m; // delta activation
      if(dact > 0.0f) {
	dact *= 1.0f - lu->act_p;
      }
      else {
	dact *= lu->act_p;
      }
      lu->act_p = act_range.Clip(lu->act_p + dact);
      u->act_dif = u->act_p - u->act_m;
    }
    lu->act_p2 = lu->act_p;	// always set this just in case..
  }

  if((net->phase_order == LeabraNetwork::MINUS_PLUS_PLUS) && (net->phase_no == 2))
    lu->act_p2 = lu->act_eq;
  else if(net->phase_order == LeabraNetwork::MINUS_PLUS_2) {
    if(net->phase_no == 2)
      lu->act_m2 = lu->act_eq;
    else
      lu->act_p2 = lu->act_eq;
  }
}

//////////////////////////////////////////
//	Ext Rew Layer Spec		//
//////////////////////////////////////////

void AvgExtRewSpec::Initialize() {
  sub_avg = false;
  avg_dt = .005f;
}

void OutErrSpec::Initialize() {
  err_tol = 0.5f;
  graded = false;
  no_off_err = false;
  seq_all_cor = false;
  scalar_val_max = 1.0f;
}

void ExtRewSpec::Initialize() {
  err_val = 0.0f;
  norew_val = 0.5f;
  rew_val = 1.0f;
}

void ExtRewLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = true;

  rew_type = OUT_ERR_REW;

//   kwta.k = 1;
//   scalar.rep = ScalarValSpec::LOCALIST;
//   unit_range.min = 0.0f;  unit_range.max = 1.0f;
//   unit_range.UpdateAfterEdit();
//   val_range.min = unit_range.min;
//   val_range.max = unit_range.max;
}

void ExtRewLayerSpec::Defaults() {
  inherited::Defaults();
  rew.Defaults();
  avg_rew.Defaults();
  out_err.Defaults();
  Initialize();
}

void ExtRewLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew.UpdateAfterEdit();
  avg_rew.UpdateAfterEdit();
  out_err.UpdateAfterEdit();
}

void ExtRewLayerSpec::HelpConfig() {
  String help = "ExtRewLayerSpec Computation:\n\
 Computes external rewards based on network performance on an output layer or directly provided rewards.\n\
 - Minus phase = zero reward represented\n\
 - Plus phase = external reward value (computed at start of 1+) is clamped as distributed scalar-val representation.\n\
 - misc_1 on units stores the average reward value, computed using rew.avg_dt.\n\
 \nExtRewLayerSpec Configuration:\n\
 - OUT_ERR_REW: A recv connection from the output layer(s) where error is computed (marked with MarkerConSpec)\n\
 AND a MarkerConSpec from a layer called RewTarg that signals (>.5 act) when output errors count\n\
 - EXT_REW: external TARGET inputs to targ values deliver the reward value (e.g., input pattern or script)\n\
 - DA_REW: A recv connection or other means of setting da values = reward values.\n\
 - This layer must be before layers that depend on it in list of layers\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool ExtRewLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
	     "must have DaModUnits!");

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = true;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  lay->CheckError(!us->InheritsFrom(TA_DaModUnitSpec), quiet, rval,
	     "UnitSpec must be DaModUnitSpec!");
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
		"requires UnitSpec act.avg_dt = 0, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  bool got_marker = false;
  LeabraLayer* rew_targ_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      if(recv_gp->prjn->from->name == "RewTarg")
	rew_targ_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      else
	got_marker = true;
      continue;
    }
  }
  if(!got_marker) {
    lay->CheckError(rew_type == DA_REW, quiet, rval,
	       "requires at least one recv MarkerConSpec connection from DA layer",
	       "to get reward based on performance.  This was not found -- please fix!");
    lay->CheckError(rew_type == OUT_ERR_REW, quiet, rval,
	       "requires at least one recv MarkerConSpec connection from output/response layer(s) to compute",
	       "reward based on performance.  This was not found -- please fix!");
  }
  if(rew_type == OUT_ERR_REW) {
    if(lay->CheckError(rew_targ_lay == NULL, quiet, rval,
		  "requires a recv MarkerConSpec connection from layer called RewTarg",
		  "that signals (act > .5) when output error should be used for computing rewards.  This was not found -- please fix!")) {
      return false;			// fatal
    }
    lay->CheckError(rew_targ_lay->units.size == 0, quiet, rval,
	       "RewTarg layer must have one unit (has zero) -- please fix!");
    int myidx = lay->own_net->layers.FindLeafEl(lay);
    int rtidx = lay->own_net->layers.FindLeafEl(rew_targ_lay);
    lay->CheckError(rtidx > myidx, quiet, rval,
	       "reward target (RewTarg) layer must be *before* this layer in list of layers -- it is now after, won't work");
  }
  return rval;
}

void ExtRewLayerSpec::Compute_UnitDa(float er, DaModUnit* u, Unit_Group* ugp, LeabraLayer*, LeabraNetwork* net) {
  u->dav = er;
  if(avg_rew.sub_avg) u->dav -= u->act_avg;
  u->ext = u->dav;
  u->act_avg += avg_rew.avg_dt * (er - u->act_avg);

  float err_thr = (rew.rew_val - rew.err_val) * .5f + rew.err_val;

  // compute sequential error values
  // p_act_m = count of # sequential correct/errs (+ = correct, - = errors)
  // p_act_p = last count of correct in a row
  if(er < err_thr) {		// made an error
    if(u->p_act_m > 0.0f)	// had been correct
      u->p_act_m = 0.0f;
    u->p_act_m -= 1.0f;
  }
  else {			// no error
    if(u->p_act_m < 0.0f)	// had been errors
      u->p_act_m = 0.0f;
    u->p_act_m += 1.0f;		// increment count of correct
    u->p_act_p = u->p_act_m;	// record last positive 
  }

  ClampValue(ugp, net);
}

bool ExtRewLayerSpec::OutErrRewAvail(LeabraLayer* lay, LeabraNetwork*) {
  bool got_some = false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
      if(rew_lay->name != "RewTarg") continue;
      LeabraUnit* rtu = (LeabraUnit*)rew_lay->units[0];
      if(rtu->act_eq > .5) {
	got_some = true;
	break;
      }
    }
  }
  return got_some;
}

float ExtRewLayerSpec::GetOutErrRew(LeabraLayer* lay, LeabraNetwork*) {
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative

  // first pass: find the layers: use COMP if no TARG is found
  int	n_targs = 0;		// number of target layers
  int	n_comps = 0;		// number of comp layers
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec))
      continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(rew_lay->name == "RewTarg") continue;

    if(rew_lay->ext_flag & Unit::TARG) n_targs++;
    else if(rew_lay->ext_flag & Unit::COMP) n_comps++;
  }

  int rew_chk_flag = Unit::TARG;
  if(n_targs == 0) rew_chk_flag |= Unit::COMP; // also check comp if no targs!

  float totposs = 0.0f;		// total possible error (unitwise)
  float toterr = 0.0f;		// total error
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec))
      continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(rew_lay->name == "RewTarg") continue;

    if(!(rew_lay->ext_flag & rew_chk_flag)) continue; // only proceed if valid 

    LeabraLayerSpec* rls = (LeabraLayerSpec*)rew_lay->spec.SPtr();
    if(rls->InheritsFrom(&TA_ScalarValLayerSpec)) {
      UNIT_GP_ITR(rew_lay,
		  LeabraUnit* eu = (LeabraUnit*)ugp->Leaf(0);
		  float err = fabsf(eu->act_m - eu->targ);
		  if(err < out_err.err_tol) err = 0.0f;
		  toterr += err;
		  totposs += out_err.scalar_val_max;
		  )
    }
    else {
      if(out_err.no_off_err) {
	totposs += rew_lay->kwta.k; // only on units can make errors
      }
      else {
	totposs += 2 * rew_lay->kwta.k; // both on and off units count
      }
      LeabraUnit* eu;
      taLeafItr i;
      FOR_ITR_EL(LeabraUnit, eu, rew_lay->units., i) {
	if(out_err.no_off_err) {
	  if(!(eu->ext_flag & rew_chk_flag)) continue;
	  if(eu->act_m > 0.5f) {	// was active
	    if(eu->targ < 0.5f)	// shouldn't have been
	      toterr += 1.0f;
	  }
	}
	else {
	  if(!(eu->ext_flag & rew_chk_flag)) continue;
	  float tmp = fabsf(eu->act_m - eu->targ);
	  float err = 0.0f;
	  if(tmp >= out_err.err_tol) err = 1.0f;
	  toterr += err;
	}
      }
    }
  }
  if(totposs == 0.0f)
    return -1.0f;		// -1 = no reward signal at all
  if(out_err.graded) {
    float nrmerr = toterr / totposs;
    if(nrmerr > 1.0f) nrmerr = 1.0f;
    return 1.0f - nrmerr;
  }
  if(toterr > 0.0f) return 0.0f; // 0 = wrong, 1 = correct
  return 1.0f;
}

void ExtRewLayerSpec::Compute_OutErrRew(LeabraLayer* lay, LeabraNetwork* net) {
  if(!OutErrRewAvail(lay, net)) {
    Compute_NoRewAct(lay, net);	
    return;
  }

  float er = 0.0f;
  if(TestError(out_err.seq_all_cor, "Compute_OutErrRew", "out_err.seq_all_cor is not yet supported!")) {
    return;
    // todo!  not supported
//     bool old_graded = out_err.graded;
//     out_err.graded = false;		// prevent graded sig here!
//     float itm_er = GetOutErrRew(lay, net);
//     out_err.graded = old_graded;

//     lay->misc_iar.SetSize(3); // 0 = addr of eg; 1 = # tot; 2 = # cor
//     Event_Group* eg = net->GetMyCurEventGp();
//     int eg_addr = (int)eg;
//     if(lay->misc_iar[0] != eg_addr) { // new seq
//       lay->misc_iar[0] = eg_addr;
//       lay->misc_iar[1] = 1;
//       lay->misc_iar[2] = (int)itm_er;
//     }
//     else {
//       lay->misc_iar[1]++;
//       lay->misc_iar[2] += (int)itm_er;
//     }
//     Event* ev = net->GetMyCurEvent();
//     int idx = eg->Find(ev);
//     if(idx < eg->size-1) {	// not last event: no reward!
//       Compute_NoRewAct(lay, net);
//       return;
//     }
//     er = (float)lay->misc_iar[2] / (float)lay->misc_iar[1];
//     if(!out_err.graded && (er < 1.0f)) er = 0.0f; // didn't make it!
  }
  else {
    er = GetOutErrRew(lay, net);
  }

  // starts out 0-1, transform into correct range
  er = (rew.rew_val - rew.err_val) * er + rew.err_val;

  UNIT_GP_ITR
    (lay,
     DaModUnit* u = (DaModUnit*)ugp->Leaf(0);
     u->misc_1 = 1.0f;		// indication of reward!
     Compute_UnitDa(er, u, ugp, lay, net);
     );
}

void ExtRewLayerSpec::Compute_ExtRew(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::TARG)) {
    Compute_NoRewAct(lay, net);	
    return;
  }    
  UNIT_GP_ITR
    (lay, 
     DaModUnit* u = (DaModUnit*)ugp->Leaf(0);
     u->misc_1 = 1.0f;		// indication of reward!
     float er = u->ext;
     Compute_UnitDa(er, u, ugp, lay, net);
     );
}

void ExtRewLayerSpec::Compute_DaRew(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay, 
     DaModUnit* u = (DaModUnit*)ugp->Leaf(0);
     float er = u->dav;
     Compute_UnitDa(er, u, ugp, lay, net);
     );
}

void ExtRewLayerSpec::Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)ugp->Leaf(0);
     u->misc_1 = 0.0f;		// indication of no reward!
     u->ext = rew.norew_val;	// this is appropriate to set here..
     ClampValue(ugp, net);
     );
}

void ExtRewLayerSpec::Compute_NoRewAct(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR
    (lay,
     LeabraUnit* u = (LeabraUnit*)ugp->Leaf(0);
     u->misc_1 = 0.0f;		// indication of no reward!
     u->ext = rew.norew_val;
     ClampValue(ugp, net);
     );
}

void ExtRewLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    lay->SetExtFlag(Unit::EXT);
    Compute_ZeroAct(lay, net);	// zero reward in minus
    HardClampExt(lay, net);
  }
  else if(net->phase_no == 1) {
    lay->SetExtFlag(Unit::EXT);
    if(rew_type == OUT_ERR_REW)
      Compute_OutErrRew(lay, net);
    else if(rew_type == EXT_REW)
      Compute_ExtRew(lay, net);
    else if(rew_type == DA_REW)
      Compute_DaRew(lay, net);
    HardClampExt(lay, net);
  }
  else {
    // clamp to prior act_p value: will happen automatically
    HardClampExt(lay, net);
  }
}

void ExtRewLayerSpec::Compute_dWt(LeabraLayer*, LeabraNetwork*) {
  return;			// never compute dwts!
}


////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// 	Standard TD Reinforcement Learning 		//
//////////////////////////////////////////////////////////

void TDRewPredConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("rnd", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;
}

//////////////////////////////////////////
//	TD Rew Pred Layer Spec		//
//////////////////////////////////////////

void TDRewPredLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;
  unit_range.min = 0.0f;
  unit_range.max = 3.0f;
  unit_range.UpdateAfterEdit();
  val_range.UpdateAfterEdit();
}

void TDRewPredLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void TDRewPredLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void TDRewPredLayerSpec::HelpConfig() {
  String help = "TDRewPredLayerSpec Computation:\n\
 Computes expected rewards according to the TD algorithm: predicts V(t+1) at time t. \n\
 - Minus phase = previous expected reward V^(t) clamped\
 - Plus phase = free-running expected reward computed (over settlng, fm recv wts)\n\
 - Learning is (act_p - act_m) * p_act_p: delta on recv units times sender activations at (t-1).\n\
 \nTDRewPredLayerSpec Configuration:\n\
 - All units I recv from must be DaModUnit/Spec units (to hold t-1 act vals)\n\
 - Sending connection to a TDRewIntegLayerSpec to integrate predictions with external rewards";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool TDRewPredLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(lay->CheckError(net->no_plus_test, quiet, rval,
		"requires LeabraNetwork no_plus_test = false, I just set it for you")) {
    net->no_plus_test = false;
  }
  if(lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
		"must have DaModUnits!")) {
    return false;
  }

  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_DaModUnitSpec), quiet, rval,
		"UnitSpec must be DaModUnitSpec!")) {
    return false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = true; // do need prior state dwt
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(lay->CheckError(!cs->InheritsFrom(TA_TDRewPredConSpec), quiet, rval,
		  "requires recv connections to be of type TDRewPredConSpec")) {
      return false;
    }
    if(lay->CheckError(cs->wt_limits.sym != false, quiet, rval,
		  "requires recv connections to have wt_limits.sym=false, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
    }
  }
  return true;
}

void TDRewPredLayerSpec::Init_Acts(LeabraLayer* lay) {
  inherited::Init_Acts(lay);
  // initialize the misc_1 variable to 0.0 -- no prior predictions!
  UNIT_GP_ITR(lay, 
      LeabraUnit* u = (LeabraUnit*)ugp->Leaf(0);
      u->misc_1 = 0.0f;
	      );
}  

void TDRewPredLayerSpec::Compute_SavePred(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->misc_1 = u->act_eq;
  }
}

void TDRewPredLayerSpec::Compute_ClampPred(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->ext = u->misc_1;
    u->SetExtFlag(Unit::EXT);
  }
}

void TDRewPredLayerSpec::Compute_ClampPrev(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, Compute_ClampPred(ugp, net); );
}

void TDRewPredLayerSpec::Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
  }
}

void TDRewPredLayerSpec::Compute_TdPlusPhase_impl(Unit_Group* ugp, LeabraNetwork* net) {
  Compute_SavePred(ugp, net);	// first, always save current predictions!

  DaModUnit* u = (DaModUnit*)ugp->FastEl(0);
  u->ext = u->act_m + u->dav;
  ClampValue(ugp, net);		// apply new value
  Compute_ExtToPlus(ugp, net);	// copy ext values to act_p
}

void TDRewPredLayerSpec::Compute_TdPlusPhase(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, Compute_TdPlusPhase_impl(ugp, net); );
}

void TDRewPredLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both) {
  inherited::PostSettle(lay, net, set_both); 
  if(net->phase_no < net->phase_max-1)
    return; // only at very last phase, do this!  see note on Compute_dWt as to why..
  Compute_TdPlusPhase(lay, net);
}

void TDRewPredLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    lay->SetExtFlag(Unit::EXT);
    Compute_ClampPrev(lay, net);
    HardClampExt(lay, net);
  }
  else {
    lay->hard_clamped = false;	// run free: generate prediction of future reward
    lay->Init_InputData();
  }
}

void TDRewPredLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  // doing second because act_p is computed only at end of settling!
  // this is better than clamping the value in the middle of everything
  // and then continuing with settling..
  if(net->phase_no < net->phase_max-1)
    return; // only do FINAL dwt!
  inherited::Compute_dWt(lay, net);
}

//////////////////////////////////////////
//	TD Rew Integ Layer Spec		//
//////////////////////////////////////////

void TDRewIntegSpec::Initialize() {
  discount = .8f;
}

void TDRewIntegLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;
  unit_range.min = 0.0f;
  unit_range.max = 3.0f;
  unit_range.UpdateAfterEdit();
  val_range.UpdateAfterEdit();
}

void TDRewIntegLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void TDRewIntegLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew_integ.UpdateAfterEdit();
}

void TDRewIntegLayerSpec::HelpConfig() {
  String help = "TDRewIntegLayerSpec Computation:\n\
 Integrates reward predictions from TDRewPred layer, and external actual rewards from\
 ExtRew layer.  Plus-minus phase difference is td value.\n\
 - Minus phase = previous expected reward V^(t) copied directly from TDRewPred\n\
 - Plus phase = integration of ExtRew r(t) and new TDRewPred computing V^(t+1)).\n\
 - No learning.\n\
 \nTDRewIntegLayerSpec Configuration:\n\
 - Requires 2 input projections, from TDRewPred, ExtRew layers.\n\
 - Sending connection to TdLayerSpec(s) (marked with MarkerConSpec)\n\
 (to compute the td change in expected rewards as computed by this layer)\n\
 - This layer must be before  TdLayerSpec layer in list of layers\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool TDRewIntegLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(lay->CheckError(!lay->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
		"must have DaModUnits!")) {
    return false;
  }

  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError(!us->InheritsFrom(TA_DaModUnitSpec), quiet, rval,
		"UnitSpec must be DaModUnitSpec!")) {
    return false;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* rew_pred_lay = NULL;
  LeabraLayer* ext_rew_lay = NULL;

  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from.ptr() == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
    if(fls->InheritsFrom(&TA_TDRewPredLayerSpec)) {
      rew_pred_lay = flay;
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      ext_rew_lay = flay;
    }
  }

  if(lay->CheckError(rew_pred_lay == NULL, quiet, rval,
		"requires recv projection from TDRewPredLayerSpec!")) {
    return false;
  }
  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int rpidx = lay->own_net->layers.FindLeafEl(rew_pred_lay);
  if(lay->CheckError(rpidx > myidx, quiet, rval,
		"reward prediction layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }

  if(lay->CheckError(ext_rew_lay == NULL, quiet, rval,
		"TD requires recv projection from ExtRewLayerSpec!")) {
    return false;
  }
  int eridx = lay->own_net->layers.FindLeafEl(ext_rew_lay);
  if(lay->CheckError(eridx > myidx, quiet, rval,
		"external reward layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }
  return true;
}

void TDRewIntegLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  lay->SetExtFlag(Unit::EXT);

  float rew_pred_val = 0.0f;
  float ext_rew_val = 0.0f;
  bool ext_rew_avail = true;

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.SPtr();
    if(fls->InheritsFrom(&TA_TDRewPredLayerSpec)) {
      LeabraUnit* rpu = (LeabraUnit*)flay->units.Leaf(0); // todo; base on connections..
      rew_pred_val = rpu->act_eq; // use current input 
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      LeabraUnit* eru = (LeabraUnit*)flay->units.Leaf(0);
      ext_rew_val = eru->act_eq;
      if(flay->acts.max < .1f)	// indication of no reward available!
	ext_rew_avail = false;
    }
  }

  float new_val;
  if(net->phase_no == 0) {
    new_val = rew_pred_val; // no discount in minus phase!!!  should only reflect previous V^(t)
  }
  else {
    new_val = rew_integ.discount * rew_pred_val + ext_rew_val; // now discount new rewpred!
  }
    
  UNIT_GP_ITR(lay, 
      DaModUnit* u = (DaModUnit*)ugp->FastEl(0);
      u->ext = new_val;
      ClampValue(ugp, net);
	      );
  HardClampExt(lay, net);
}

void TDRewIntegLayerSpec::Compute_dWt(LeabraLayer*, LeabraNetwork*) {
  return;
}

//////////////////////////////////
//	Td Layer Spec		//
//////////////////////////////////

void TdLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25;
}

void TdLayerSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

void TdLayerSpec::HelpConfig() {
  String help = "TdLayerSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - td is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nTdLayerSpec Configuration:\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
 - Sending connections must connect to units of type DaModUnit/Spec \
     (td signal from this layer put directly into td var on units)\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative td = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

bool TdLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  if(lay->CheckError(net->trial_init != LeabraNetwork::DECAY_STATE, quiet, rval,
		"requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you")) {
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  if(lay->CheckError((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f), quiet, rval,
		"requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)") ) {
    us->SetUnique("act_range", true);
    us->act_range.max = 2.0f;
    us->act_range.min = -2.0f;
    us->act_range.UpdateAfterEdit();
  }
  if(lay->CheckError((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f), quiet, rval,
		"requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("clamp_range", true);
    us->clamp_range.max = 2.0f;
    us->clamp_range.min = -2.0f;
    us->clamp_range.UpdateAfterEdit();
  }

  // check recv connection
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraLayer* rewinteg_lay = NULL;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    if(lay->CheckError(fmlay == NULL, quiet, rval,
		  "null from layer in recv projection:", (String)g)) {
      return false;
    }
    if(recv_gp->GetConSpec()->InheritsFrom(TA_MarkerConSpec)
	&& fmlay->spec.SPtr()->InheritsFrom(TA_TDRewIntegLayerSpec)) {
      rewinteg_lay = fmlay;
      if(lay->CheckError(recv_gp->cons.size <= 0, quiet, rval,
		    "requires one recv projection with at least one unit!")) {
	return false;
      }
      if(lay->CheckError(!recv_gp->Un(0)->InheritsFrom(TA_DaModUnit), quiet, rval,
		    "I need to receive from a DaModUnit!")) {
	return false;
      }
    }
  }

  if(lay->CheckError(rewinteg_lay == NULL, quiet, rval,
		"did not find TDRewInteg layer to get Td from!")) {
    return false;
  }

  int myidx = lay->own_net->layers.FindLeafEl(lay);
  int rpidx = lay->own_net->layers.FindLeafEl(rewinteg_lay);
  if(lay->CheckError(rpidx > myidx, quiet, rval,
		"reward integration layer must be *before* this layer in list of layers -- it is now after, won't work")) {
    return false;
  }

  // check sending layer projections for appropriate unit types
  int si;
  for(si=0;si<lay->send_prjns.size;si++) {
    Projection* prjn = (Projection*)lay->send_prjns[si];
    if(lay->CheckError(!prjn->from->units.el_typ->InheritsFrom(TA_DaModUnit), quiet, rval,
		  "all layers I send to must have DaModUnits!, layer:",
		  prjn->from->GetPath(),"doesn't")) {
      return false;
    }
  }
  return true;
}

void TdLayerSpec::Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork*) {
  lay->dav = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->ext = 0.0f;
    u->SetExtFlag(Unit::EXT);
  }      
}

void TdLayerSpec::Compute_Td(LeabraLayer* lay, LeabraNetwork*) {
  int ri_prjn_idx;
  FindLayerFmSpec(lay, ri_prjn_idx, &TA_TDRewIntegLayerSpec);

  lay->dav = 0.0f;
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, lay->units., i) {
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv[ri_prjn_idx];
    // just taking the first unit = scalar val
    DaModUnit* su = (DaModUnit*)cg->Un(0);
    u->dav = su->act_eq - su->act_m; // subtract current minus previous!
    u->ext = u->dav;
    u->act_eq = u->act = u->net = u->ext;
    lay->dav += u->dav;
  }
  if(lay->units.leaves > 0) lay->dav /= (float)lay->units.leaves;
}

void TdLayerSpec::Send_Td(LeabraLayer* lay, LeabraNetwork*) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())	continue;
      for(int j=0;j<send_gp->cons.size; j++) {
	((DaModUnit*)send_gp->Un(j))->dav = u->act;
      }
    }
  }
}

void TdLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing
  Compute_Td(lay, net);	// now get the td and clamp it to layer
  Send_Td(lay, net);
  Compute_ActAvg(lay, net);
}

void TdLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no == 0) {
    lay->hard_clamped = true;
    lay->SetExtFlag(Unit::EXT);
    lay->Inhib_SetVals(inhib.kwta_pt); // assume 0 - 1 clamped inputs
    Compute_ZeroAct(lay, net);	// can't do anything during settle anyway -- just zero it
  }
  else {
    // run "free" in plus phase: compute act = td
    lay->hard_clamped = false;
    lay->UnSetExtFlag(Unit::EXT);
  }
  inherited::Compute_HardClamp(lay, net);
}


///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

///////////////////////////////////////////////////////////////
//			TD
///////////////////////////////////////////////////////////////

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

bool LeabraWizard::TD(LeabraNetwork* net, bool bio_labels, bool td_mod_all) {
  String msg = "Configuring TD Temporal Differences Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check that connection(s) were made from all appropriate output layers\
 to the ExtRew layer, using the MarkerConSpec (MarkerCons) Con spec.\
 This will provide the error signal to the system based on output error performance.\n\n";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters\
 will be set.  If there are any actual errors which must be corrected before\
 the network will run, you will see a message to that effect --- you will then need to\
 re-run this configuration process to make sure everything is OK.  When you press\
 Re/New/Init on the control process these same checks will be performed, so you\
 can be sure everything is ok.";
  taMisc::Confirm(msg);

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool	tdrp_new = false;
  String tdrpnm = "ABL";  String tdintnm = "NAc";  String tddanm = "VTA";
  if(!bio_labels) {
    tdrpnm = "TDRewPred";    tdintnm = "TDRewInteg";    tddanm = "TD";
  }

  LeabraLayer* rew_targ_lay = (LeabraLayer*)net->FindMakeLayer("RewTarg");
  LeabraLayer* extrew = (LeabraLayer*)net->FindMakeLayer("ExtRew");
  LeabraLayer* tdrp = (LeabraLayer*)net->FindMakeLayer(tdrpnm, NULL, tdrp_new);
  LeabraLayer* tdint = (LeabraLayer*)net->FindMakeLayer(tdintnm);
  LeabraLayer* tdda = (LeabraLayer*)net->FindMakeLayer(tddanm);
  if(rew_targ_lay == NULL || tdrp == NULL || extrew == NULL || tdint == NULL || tdda == NULL) return false;
  if(tdrp_new) {
    extrew->pos.z = 0; extrew->pos.y = 4; extrew->pos.x = 0;
    tdrp->pos.z = 0; tdrp->pos.y = 2; tdrp->pos.x = 0;
    tdint->pos.z = 0; tdint->pos.y = 0; tdint->pos.x = 0;
    tdda->pos.z = 0; tdda->pos.y = 4; tdda->pos.x = 10;
  }

  //////////////////////////////////////////////////////////////////////////////////
  // sort layers

  rew_targ_lay->name = "0000";  extrew->name = "0001"; tdrp->name = "0002";  
  tdint->name = "0003";  tdda->name = "0004";

  net->layers.Sort();

  rew_targ_lay->name = "RewTarg";  extrew->name = "ExtRew"; tdrp->name = tdrpnm; 
  tdint->name = tdintnm;  tdda->name = tddanm;

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
    lay->SetUnitType(&TA_DaModUnit);
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != tdrp && lay != extrew && lay != tdint && lay != tdda
       && !laysp->InheritsFrom(&TA_PFCLayerSpec) && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
      other_lays.Link(lay);
      if(lay->layer_type == Layer::HIDDEN)
	hidden_lays.Link(lay);
      else if(lay->layer_type == Layer::INPUT)
	input_lays.Link(lay);
      else 
	output_lays.Link(lay);
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
      if(us == NULL || !us->InheritsFrom(TA_DaModUnitSpec)) {
	us->ChangeMyType(&TA_DaModUnitSpec);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  String gpprfx = "PFC_BG_";
  if(!bio_labels)
    gpprfx = "TD_";

  BaseSpec_Group* units = net->FindMakeSpecGp(gpprfx + "Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp(gpprfx + "Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp(gpprfx + "Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp(gpprfx + "Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return false;

  LeabraUnitSpec* rewpred_units = (LeabraUnitSpec*)units->FindMakeSpec("TDRewPredUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* td_units = (LeabraUnitSpec*)units->FindMakeSpec("TdUnits", &TA_DaModUnitSpec);
  if(rewpred_units == NULL || td_units == NULL) return false;

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(learn_cons == NULL) return false;

  TDRewPredConSpec* rewpred_cons = (TDRewPredConSpec*)learn_cons->FindMakeChild("TDRewPredCons", &TA_TDRewPredConSpec);
  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return false;
  LeabraConSpec* fixed_bias = (LeabraConSpec*)bg_bias->FindMakeChild("FixedBias", &TA_LeabraBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  if(rewpred_cons == NULL || marker_cons == NULL || fixed_bias == NULL)
    return false;

  ExtRewLayerSpec* ersp = (ExtRewLayerSpec*)layers->FindMakeSpec("ExtRewLayer", &TA_ExtRewLayerSpec);
  TDRewPredLayerSpec* tdrpsp = (TDRewPredLayerSpec*)layers->FindMakeSpec(tdrpnm + "Layer", &TA_TDRewPredLayerSpec);
  TDRewIntegLayerSpec* tdintsp = (TDRewIntegLayerSpec*)layers->FindMakeSpec(tdintnm + "Layer", &TA_TDRewIntegLayerSpec);
  TdLayerSpec* tdsp = (TdLayerSpec*)layers->FindMakeSpec(tddanm + "Layer", &TA_TdLayerSpec);
  if(tdrpsp == NULL || ersp == NULL || tdintsp == NULL || tdsp == NULL) return false;

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  if(fullprjn == NULL || onetoone == NULL) return false;

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  learn_cons->lmix.hebb = .01f; // .01 hebb on learn cons
//   learn_cons->not_used_ok = true;
  learn_cons->UpdateAfterEdit();
  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;
  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;
  rewpred_cons->SetUnique("rnd", true);
  rewpred_cons->rnd.mean = 0.1f; rewpred_cons->rnd.var = 0.0f;
  rewpred_cons->SetUnique("wt_sig", true);
  rewpred_cons->wt_sig.gain = 1.0f;  rewpred_cons->wt_sig.off = 1.0f;
  rewpred_cons->SetUnique("lmix", true);
  rewpred_cons->lmix.hebb = 0.0f;

  rewpred_units->SetUnique("g_bar", true);
  rewpred_units->g_bar.h = .015f;
  rewpred_units->g_bar.a = .045f;

  if(output_lays.size > 0)
    ersp->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
  else
    ersp->rew_type = ExtRewLayerSpec::EXT_REW;

  int n_rp_u = 19;		// number of rewpred-type units
  tdrpsp->unit_range.min = 0.0f;  tdrpsp->unit_range.max = 3.0f;
  tdintsp->unit_range.min = 0.0f;  tdintsp->unit_range.max = 3.0f;

  // optimization to speed up settling in phase 2: only the basic layers here
  int j;
  for(j=0;j<net->specs.size;j++) {
    if(net->specs[j]->InheritsFrom(TA_LeabraLayerSpec)) {
      LeabraLayerSpec* sp = (LeabraLayerSpec*)net->specs[j];
      sp->decay.clamp_phase2 = true;
      sp->UpdateAfterEdit();
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set geometries

  if(tdrp->un_geom.n != n_rp_u) { tdrp->un_geom.n = n_rp_u; tdrp->un_geom.x = n_rp_u; tdrp->un_geom.y = 1; }
  if(extrew->un_geom.n != 8) { extrew->un_geom.n = 8; extrew->un_geom.x = 8; extrew->un_geom.y = 1; }
  if(tdint->un_geom.n != n_rp_u) { tdint->un_geom.n = n_rp_u; tdint->un_geom.x = n_rp_u; tdint->un_geom.y = 1; }
  tdda->un_geom.n = 1;
  rew_targ_lay->un_geom.n = 1;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  tdrp->SetLayerSpec(tdrpsp);	tdrp->SetUnitSpec(rewpred_units);
  extrew->SetLayerSpec(ersp);	extrew->SetUnitSpec(rewpred_units);
  tdint->SetLayerSpec(tdintsp);	tdint->SetUnitSpec(rewpred_units);
  tdda->SetLayerSpec(tdsp);	tdda->SetUnitSpec(td_units);

  rewpred_units->bias_spec.SetSpec(bg_bias);
  td_units->bias_spec.SetSpec(fixed_bias);
  
  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(extrew, rew_targ_lay, onetoone, marker_cons);
  net->FindMakePrjn(tdint, tdrp, onetoone, marker_cons);
  net->FindMakePrjn(tdda, tdint, onetoone, marker_cons);
  net->FindMakePrjn(tdint, extrew, onetoone, marker_cons);
  net->FindMakePrjn(tdrp, tdda, onetoone, marker_cons);

  for(i=0;i<other_lays.size;i++) {
    Layer* ol = (Layer*)other_lays[i];
    if(tdrp_new)
      net->FindMakePrjn(tdrp, ol, fullprjn, rewpred_cons);
    if(td_mod_all)
      net->FindMakePrjn(ol, tdda, fullprjn, marker_cons);
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(extrew, ol, onetoone, marker_cons);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();

  bool ok = tdrpsp->CheckConfig_Layer(tdrp, true) && tdintsp->CheckConfig_Layer(tdint, true)
    && tdsp->CheckConfig_Layer(tdda, true) && ersp->CheckConfig_Layer(extrew, true);

  if(!ok) {
    msg =
      "TD: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg = 
    "TD configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Confirm(msg);

  tdrpsp->UpdateAfterEdit();
  ersp->UpdateAfterEdit();
  tdintsp->UpdateAfterEdit();
  
  for(j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = proj->FindMakeSelectEdit("TD");
  if(edit != NULL) {
    rewpred_cons->SelectForEditNm("lrate", edit, "rewpred");
    ersp->SelectForEditNm("rew", edit, "extrew");
    tdrpsp->SelectForEditNm("rew_pred", edit, "tdrp");
    tdintsp->SelectForEditNm("rew_integ", edit, "tdint");
  }
  return true;
}
