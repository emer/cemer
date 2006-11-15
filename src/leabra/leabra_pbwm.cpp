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

#include "leabra_pbwm.h"

#include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////
//	Patch Layer Spec	//
//////////////////////////////////

void PatchLayerSpec::Initialize() {
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
}

//////////////////////////////////
//	SNc Layer Spec		//
//////////////////////////////////

void SNcMiscSpec::Initialize() {
  patch_mode = NO_PATCH;
  patch_gain = .5f;
}

void SNcLayerSpec::Initialize() {
}

void SNcLayerSpec::Defaults() {
  PVLVDaLayerSpec::Defaults();
  snc.Initialize();
  Initialize();
}

void SNcLayerSpec::InitLinks() {
  PVLVDaLayerSpec::InitLinks();
  taBase::Own(snc, this);
}

void SNcLayerSpec::HelpConfig() {
  String help = "SNcLayerSpec Computation:\n\
 Provides a stripe-specifc DA signal to Matrix Layer units, based on patch input.\n\
 This is currently not supported.  Also, stripe-specific DA signals are computed\
 directly in the Matrix based on SNrThal multiplication of the signal, even though\
 biologically this signal is likely reflected here in the SNc activations\
 (this is computationally easier and creates fewer interdependencies.\n\
 After pressing OK here, you will see configuration info for the PVLVDaLayerSpec\
 which this layer is based on";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
  PVLVDaLayerSpec::HelpConfig();
}

bool SNcLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!PVLVDaLayerSpec::CheckConfig_Layer(lay, quiet)) return false;

  int myidx = lay->own_net->layers.FindLeaf(lay);

  int pc_prjn_idx;
  LeabraLayer* pclay = FindLayerFmSpec(lay, pc_prjn_idx, &TA_PatchLayerSpec);
  if(pclay != NULL) {
    int patchidx = lay->own_net->layers.FindLeaf(pclay);
    if(patchidx > myidx) {
      if (!quiet) taMisc::CheckError("SNcLayerSpec: Patch layer must be *before* this layer in list of layers -- it is now after, won't work");
      return false;
    }
  }
  else {
    snc.patch_mode = SNcMiscSpec::NO_PATCH;
  }

  return true;
}

void SNcLayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
  // todo: patch not supported right now!
  PVLVDaLayerSpec::Compute_Da(lay, net);
}

//////////////////////////////////
//	MatrixConSpec		//
//////////////////////////////////

void MatrixConSpec::Initialize() {
  SetUnique("lmix", true);
//   lmix.hebb = .001f;
//   lmix.err = .999f;
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  learn_rule = MAINT;
}

void MatrixConSpec::InitLinks() {
  LeabraConSpec::InitLinks();
  children.SetBaseType(&TA_LeabraConSpec); // make this the base type so bias specs
					   // can live under here..
  children.el_typ = &TA_MatrixConSpec; // but this is the default type
}

//////////////////////////////////////////
// 	Matrix Unit Spec		//
//////////////////////////////////////////

void MatrixBiasSpec::Initialize() {
  learn_rule = MAINT;
}

void MatrixUnitSpec::Initialize() {
  SetUnique("bias_spec", true);
  bias_spec.type = &TA_MatrixBiasSpec;
  SetUnique("g_bar", true);
  g_bar.a = .03f;
  g_bar.h = .01f;

  freeze_net = true;
}

void MatrixUnitSpec::Defaults() {
  DaModUnitSpec::Defaults();
  Initialize();
}

void MatrixUnitSpec::InitLinks() {
  DaModUnitSpec::InitLinks();
  bias_spec.type = &TA_MatrixBiasSpec;
}

void MatrixUnitSpec::Compute_NetAvg(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*, LeabraNetwork* net) {
  if(act.send_delta) {
    u->net_raw += u->net_delta;
    u->net += u->clmp_net + u->net_raw;
  }
  MatrixLayerSpec* mls = (MatrixLayerSpec*)lay->spec.spec;
  float eff_dt = dt.net;
  if(freeze_net) {
    if(mls->bg_type == MatrixLayerSpec::MAINT) {
      if(net->phase_no == 2) eff_dt = 0.0f;
    }
    else {
      if(net->phase_no >= 1) eff_dt = 0.0f;
    }
  }

  u->net = u->prv_net + eff_dt * (u->net - u->prv_net);
  u->prv_net = u->net;
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += noise_sched.GetVal(net->cycle) * noise.Gen();
  }
  u->i_thr = Compute_IThresh(u, lay, net);
}

void MatrixUnitSpec::PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			       LeabraNetwork* net, bool set_both)
{
  DaModUnitSpec::PostSettle(u, lay, thr, net, set_both);
  MatrixLayerSpec* mls = (MatrixLayerSpec*)lay->spec.spec;
  if(mls->bg_type == MatrixLayerSpec::MAINT) {
    DaModUnit* lu = (DaModUnit*)u;
    if((net->phase_order == LeabraNetwork::MINUS_PLUS_PLUS) && (net->phase_no == 2)) {
      lu->act_dif = lu->act_p2 - lu->act_p;
    }
  }
}

//////////////////////////////////
//	Matrix Layer Spec	//
//////////////////////////////////

void MatrixMiscSpec::Initialize() {
  neg_da_bl = 0.0002f;
  neg_gain = 1.5f;
  perf_gain = 0.0f;
  no_snr_mod = false;
}

void ContrastSpec::Initialize() {
  gain = 1.0f;
  go_p = .5f;
  go_n = .5f;
  nogo_p = .5f;
  nogo_n = .5f;
}

void MatrixRndGoSpec::Initialize() {
  avgrew = 0.9f;

  ucond_p = .0001f;
  ucond_da = 1.0f;

  nogo_thr = 50;
  nogo_p = .1f;
  nogo_da = 10.0f;
}

void MatrixErrRndGoSpec::Initialize() {
  on = true;
  min_cor = 5;			// use 1.0 for output
  min_errs = 1;
  err_p = 1.0f;
  gain = 0.2f;			// use .5 for output
  if_go_p = 0.0f;
  err_da = 10.0f;
}

void MatrixAvgDaRndGoSpec::Initialize() {
  on = true;
  avgda_p = 0.1f;
  gain = 0.5f;
  avgda_thr = 0.1f;
  nogo_thr = 10;
  avgda_da = 10.0f;
  avgda_dt = 0.005f;
}

/////////////////////////////////////////////////////

void MatrixLayerSpec::Initialize() {
  //  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  //  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .25f;
  //  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  //  SetUnique("compute_i", true);
  compute_i = KWTA_INHIB;
  //  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .25f;

  bg_type = MAINT;
}

void MatrixLayerSpec::UpdateAfterEdit() {
  LeabraLayerSpec::UpdateAfterEdit();
}

void MatrixLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
  matrix.Initialize();
  contrast.Initialize();
  rnd_go.Initialize();
  err_rnd_go.Initialize();
  avgda_rnd_go.Initialize();
  Initialize();
}

void MatrixLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(matrix, this);
  taBase::Own(contrast, this);
  taBase::Own(rnd_go, this);
  taBase::Own(err_rnd_go, this);
  taBase::Own(avgda_rnd_go, this);
}

void MatrixLayerSpec::HelpConfig() {
  String help = "MatrixLayerSpec Computation:\n\
 There are 2 types of units arranged sequentially in the following order within each\
 stripe whose firing affects the gating status of the corresponding stripe in PFC:\n\
 - GO unit = toggle maintenance of units in PFC: this is the direct pathway\n\
 - NOGO unit = maintain existing state in PFC (i.e. do nothing): this is the indirect pathway\n\
 \nMatrixLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Units must be DaModUnits w/ MatrixUnitSpec and must recv from PVLVDaLayerSpec layer\
 (either VTA or SNc) to get da modulation for learning signal\n\
 - Recv connections need to be MatrixConSpec as learning occurs based on the da-signal\
 on the matrix units.\n\
 - This layer must be after DaLayers in list of layers\n\
 - Units must be organized into groups (stipes) of same number as PFC";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
}

bool MatrixLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig_Layer(lay, quiet))
    return false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  if(lay->units.gp.size == 1) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: layer must contain multiple unit groups (= stripes) for indepent searching of gating space!");
    return false;
  }

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::CheckError("MatrixLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: must have DaModUnits!");
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_MatrixUnitSpec)) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: UnitSpec must be MatrixUnitSpec!");
    return false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = false; // don't need prior state dwt
  if((us->opt_thresh.learn >= 0.0f) || us->opt_thresh.updt_wts) {
    if(!quiet) taMisc::CheckError("MatrixLayerSpec: UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
			     "I just set it for you in spec:", us->name,
			     "(make sure this is appropriate for all layers that use this spec!)");
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  if(us->act.avg_dt <= 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.005f;
    if(!quiet) taMisc::CheckError("MatrixLayerSpec: requires UnitSpec act.avg_dt > 0, I just set it to .005 for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->SetUnique("g_bar", true);
  // must have these not initialized every trial!
  if(us->hyst.init) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
    if(!quiet) taMisc::CheckError("MatrixLayerSpec: requires UnitSpec hyst.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if(us->acc.init) {
    us->SetUnique("acc", true);
    us->acc.init = false;
    if(!quiet) taMisc::CheckError("MatrixLayerSpec: requires UnitSpec acc.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->UpdateAfterEdit();

  LeabraBiasSpec* bs = (LeabraBiasSpec*)us->bias_spec.spec;
  if(bs == NULL) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: Error: null bias spec in unit spec", us->name);
    return false;
  }

  LeabraLayer* da_lay = NULL;
  LeabraLayer* snr_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->prjn->from == recv_gp->prjn->layer) // self projection, skip it
      continue;
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from;
      if(fmlay->spec.spec->InheritsFrom(TA_SNcLayerSpec)) da_lay = fmlay;
      if(fmlay->spec.spec->InheritsFrom(TA_SNrThalLayerSpec)) snr_lay = fmlay;
      continue;
    }
    MatrixConSpec* cs = (MatrixConSpec*)recv_gp->spec.spec;
    if(!cs->InheritsFrom(TA_MatrixConSpec)) {
      if (!quiet) taMisc::CheckError("MatrixLayerSpec:  Receiving connections must be of type MatrixConSpec!");
      return false;
    }
    if(cs->wt_limits.sym != false) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
      if(!quiet) taMisc::CheckError("MatrixLayerSpec: requires recv connections to have wt_limits.sym=false, I just set it for you in spec:",
			       cs->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
    if(bg_type == MatrixLayerSpec::OUTPUT) {
      if((cs->learn_rule != MatrixConSpec::OUTPUT_DELTA) && (cs->learn_rule != MatrixConSpec::OUTPUT_CHL)) {
	cs->SetUnique("learn_rule", true);
	cs->learn_rule = MatrixConSpec::OUTPUT_DELTA;
	if(!quiet) taMisc::CheckError("MatrixLayerSpec: OUTPUT BG requires MatrixConSpec learn_rule of OUTPUT type, I just set it for you in spec:",
				 cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
    else {			// pfc
      if((cs->learn_rule == MatrixConSpec::OUTPUT_DELTA) || (cs->learn_rule == MatrixConSpec::OUTPUT_CHL)) {
	cs->SetUnique("learn_rule", true);
	cs->learn_rule = MatrixConSpec::MAINT;
	if(!quiet) taMisc::CheckError("MatrixLayerSpec: BG_pfc requires MatrixConSpec learn_rule of MAINT type, I just set it for you in spec:",
				 cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
  }
  if(da_lay == NULL) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: Could not find DA layer (PVLVDaLayerSpec, VTA or SNc) -- must receive MarkerConSpec projection from one!");
    return false;
  }
  if(snr_lay == NULL) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: Could not find SNrThal layer -- must receive MarkerConSpec projection from one!");
    return false;
  }
  // vta/snc must be before matrix!  good.
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int daidx = lay->own_net->layers.FindLeaf(da_lay);
  if(daidx > myidx) {
    if (!quiet) taMisc::CheckError("MatrixLayerSpec: DA layer (PVLVDaLayerSpec, VTA or SNc) must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }
  return true;
}

void MatrixLayerSpec::InitWtState(LeabraLayer* lay) {
  LeabraLayerSpec::InitWtState(lay);
  UNIT_GP_ITR(lay, 
	      DaModUnit* u = (DaModUnit*)ugp->FastEl(0);
	      u->misc_1 = avgda_rnd_go.avgda_thr;	// initialize to above rnd go val..
	      );
}


bool MatrixLayerSpec::Check_RndGoAvgRew(LeabraLayer* lay, LeabraNetwork* net) {
  float avg_rew = -1.0f;

  // if in a test process, don't do random go's!
  if((net->epoch >= 1) && (net->context != Network::TEST))
    avg_rew = net->avg_ext_rew;

  if(avg_rew == -1.0f) {	// didn't get from stat, use value on layer
    LeabraLayer* er_lay = FindLayerFmSpecNet(lay->own_net, &TA_ExtRewLayerSpec);
    if(er_lay != NULL) {
      DaModUnit* er_un = (DaModUnit*)er_lay->units.Leaf(0);
      avg_rew = er_un->act_avg;
    }
  }

  if(avg_rew >= rnd_go.avgrew) return false; // don't do Random Go
  return true;			// do it.
}

void MatrixLayerSpec::Compute_UCondNoGoRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];

    // unconditional
    if(Random::ZeroOne() < rnd_go.ucond_p) {
      mugp->misc_state1 = PFCGateSpec::UCOND_RND_GO;
    }

    // nogo
    if((int)fabs((float)mugp->misc_state) > rnd_go.nogo_thr) {
      if(Random::ZeroOne() < rnd_go.nogo_p) {
	mugp->misc_state1 = PFCGateSpec::NOGO_RND_GO;
      }
    }
  }
}

void MatrixLayerSpec::Compute_ErrRndGo(LeabraLayer* lay, LeabraNetwork*) {
  if(!err_rnd_go.on) return;
  LeabraLayer* er_lay = FindLayerFmSpecNet(lay->own_net, &TA_ExtRewLayerSpec);
  if(er_lay == NULL) return;
  DaModUnit* er_un = (DaModUnit*)er_lay->units.Leaf(0);

  int n_err_cor = (int)-er_un->p_act_m; // number of sequential err vs. correct
  int n_cor = (int)er_un->p_act_p; // last count of sequential correct in a row
  bool cor_over_thresh = (n_cor >= err_rnd_go.min_cor);
  if(!cor_over_thresh || (n_err_cor < err_rnd_go.min_errs)) return; // not ready to go

//   if(err_rnd_go.mutex)
  int snr_prjn_idx = 0;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)snrthal_lay->spec.spec;

  bool all_nogo = (snrthal_lay->acts_m.max < snrthalsp->snrthal.go_thr);

  if(all_nogo) {
    if(Random::ZeroOne() > err_rnd_go.err_p) return;	// not this time..
  }
  else {
    if(Random::ZeroOne() > err_rnd_go.if_go_p) return;	// not this time..
  }

  // compute softmax over snr unit netinputs for probabilities of stripe firing
  float sum_exp_val = 0.0f;
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
    DaModUnit* snru = (DaModUnit*)snrgp->FastEl(0);
    float exp_val = 0.0f;
    if(all_nogo) {
      exp_val = expf(err_rnd_go.gain * snru->p_act_m);	// p_act_m saves net from minus phase!
    }
    else { // if_go
      if(snru->act_m < snrthalsp->snrthal.go_thr) // do not include already-firing guys
	exp_val = expf(err_rnd_go.gain * snru->p_act_m);
    }
    sum_exp_val += exp_val;
    snru->p_act_m = exp_val;	// then using p_act_m for storing this!
  }
  if(sum_exp_val > 0.0f) {
    for(int gi=0; gi<lay->units.gp.size; gi++) {
      LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
      DaModUnit* snru = (DaModUnit*)snrgp->FastEl(0);
      snru->p_act_m /= sum_exp_val;
    }
  }

  float p_stripe = Random::ZeroOne();
  float sum_p = 0.0f;
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
    DaModUnit* snru = (DaModUnit*)snrgp->Leaf(0);

    sum_p += snru->p_act_m;	// p_act_m = softmax probability value
    if(p_stripe <= sum_p) {
      // only random Go if currently not firing
      if(snru->act_m < snrthalsp->snrthal.go_thr) {
	mugp->misc_state1 = PFCGateSpec::ERR_RND_GO;
      }
      break;			// done!
    }
  }
}

void MatrixLayerSpec::Compute_AvgDaRndGo(LeabraLayer* lay, LeabraNetwork*) {
  if(!avgda_rnd_go.on) return;

  if(Random::ZeroOne() > avgda_rnd_go.avgda_p) return;	// not this time..

  int snr_prjn_idx = 0;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)snrthal_lay->spec.spec;

  // compute softmax over snr unit netinputs to select the stripe to go
  float sum_exp_val = 0.0f;
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    DaModUnit* u = (DaModUnit*)mugp->FastEl(0);
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
    DaModUnit* snru = (DaModUnit*)snrgp->FastEl(0);
    float exp_val = 0.0f;
    float avgda_val = u->misc_1; // stored average da value
    // softmax competition is for everyone below avgda_thr
    if(avgda_val < avgda_rnd_go.avgda_thr) {
      exp_val = expf(avgda_rnd_go.gain * (avgda_rnd_go.avgda_thr - avgda_val));
    }
    sum_exp_val += exp_val;
    snru->p_act_p = exp_val;	// using p_act_p for storing this!
  }
  if(sum_exp_val > 0.0f) {
    for(int gi=0; gi<lay->units.gp.size; gi++) {
      LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
      DaModUnit* snru = (DaModUnit*)snrgp->FastEl(0);
      snru->p_act_p /= sum_exp_val;
    }
  }

  float p_stripe = Random::ZeroOne();
  float sum_p = 0.0f;
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
    DaModUnit* snru = (DaModUnit*)snrgp->Leaf(0);

    sum_p += snru->p_act_p;	// p_act_p = softmax probability value
    if(p_stripe <= sum_p) {
      // only actually do it if it meets the other criteria: not firing, inactive longer than nogo_thr
      if((snru->act_eq < snrthalsp->snrthal.go_thr) && 
	 ((int)fabs((float)mugp->misc_state) >= avgda_rnd_go.nogo_thr)) {
	mugp->misc_state1 = PFCGateSpec::AVGDA_RND_GO;
	DaModUnit* u = (DaModUnit*)mugp->FastEl(0);
	u->misc_1 = avgda_rnd_go.avgda_thr; // reset to above impunity..
      }
      break;			// done!
    }
  }
}

void MatrixLayerSpec::Compute_ClearRndGo(LeabraLayer* lay, LeabraNetwork*) {
  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    if(mugp->misc_state1 >= PFCGateSpec::UCOND_RND_GO)
      mugp->misc_state1 = PFCGateSpec::EMPTY_GO;
  }
}

void MatrixLayerSpec::Compute_DaModUnit_NoContrast(DaModUnit* u, float dav, int go_no) {
  if(go_no == (int)PFCGateSpec::GATE_GO) {	// we are a GO gate unit
    if(dav >= 0.0f)  { 
      u->vcb.g_h = dav;
      u->vcb.g_a = 0.0f;
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -dav;
    }
  }
  else {			// we are a NOGO gate unit
    if(dav >= 0.0f) {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = dav;
    }
    else {
      u->vcb.g_h = -dav;
      u->vcb.g_a = 0.0f;
    }
  }
}


void MatrixLayerSpec::Compute_DaModUnit_Contrast(DaModUnit* u, float dav, float act_val, int go_no) {
  if(go_no == (int)PFCGateSpec::GATE_GO) {	// we are a GO gate unit
    if(dav >= 0.0f)  { 
      u->vcb.g_h = contrast.gain * dav * ((1.0f - contrast.go_p) + (contrast.go_p * act_val));
      u->vcb.g_a = 0.0f;
    }
    else {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = -matrix.neg_gain * contrast.gain * dav * ((1.0f - contrast.go_n) + (contrast.go_n * act_val));
    }
  }
  else {			// we are a NOGO gate unit
    if(dav >= 0.0f) {
      u->vcb.g_h = 0.0f;
      u->vcb.g_a = contrast.gain * dav * ((1.0f - contrast.nogo_p) + (contrast.nogo_p * act_val));
    }
    else {
      u->vcb.g_h = -matrix.neg_gain * contrast.gain * dav * ((1.0f - contrast.nogo_n) + (contrast.nogo_n * act_val));
      u->vcb.g_a = 0.0f;
    }
  }
}

void MatrixLayerSpec::Compute_DaTonicMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraInhib*, LeabraNetwork*) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_SNcLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.spec;
  float dav = contrast.gain * dals->da.tonic_da;
  int idx = 0;
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, mugp->, i) {
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx % 2); // GO = 0, NOGO = 1
    u->dav = dav;		// accurately reflect tonic modulation!
    Compute_DaModUnit_NoContrast(u, dav, go_no);
    idx++;
  }
}

void MatrixLayerSpec::Compute_DaPerfMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraInhib*, LeabraNetwork*) {
  int da_prjn_idx;
  LeabraLayer* da_lay = FindLayerFmSpec(lay, da_prjn_idx, &TA_SNcLayerSpec);
  PVLVDaLayerSpec* dals = (PVLVDaLayerSpec*)da_lay->spec.spec;
  float tonic_da = dals->da.tonic_da;

  int idx = 0;
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, mugp->, i) {
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx % 2); // GO = 0, NOGO = 1

    // need to separate out the tonic and non-tonic because tonic contributes with contrast.gain
    // but perf is down-modulated by matrix.perf_gain..
    float non_tonic = u->dav - tonic_da;
    float dav = contrast.gain * (tonic_da + matrix.perf_gain * non_tonic);
    // NOTE: do not include rnd go here, because extra prf da in LearnMod will also produce learning benefits
    // and performance is already assured through the PFC gating signal.
    Compute_DaModUnit_NoContrast(u, dav, go_no);
    idx++;
  }
}

void MatrixLayerSpec::Compute_DaLearnMod(LeabraLayer* lay, LeabraUnit_Group* mugp, LeabraInhib*, LeabraNetwork* net) {
  int snr_prjn_idx = 0;
  FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);

  PFCGateSpec::GateSignal gate_sig = (PFCGateSpec::GateSignal)mugp->misc_state2;
    
  int idx = 0;
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, mugp->, i) {
    PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(idx % 2); // GO = 0, NOGO = 1
    LeabraCon_Group* snrcg = (LeabraCon_Group*)u->recv.gp[snr_prjn_idx];
    DaModUnit* snrsu = (DaModUnit*)snrcg->Un(0);

    float gating_act = 0.0f;	// activity of the unit during the gating action firing
    float snrthal_act = 0.0f;	// activity of the snrthal during gating action firing
    if(net->phase_no == 3) 	{ gating_act = u->act_m2; snrthal_act = snrsu->act_m2; } // TRANS
    else if(net->phase_no == 2) { gating_act = u->act_p;  snrthal_act = snrsu->act_p; }	// GOGO
    else if(net->phase_no == 1)	{ gating_act = u->act_m;  snrthal_act = snrsu->act_m; }	// OUTPUT

    if(gate_sig == PFCGateSpec::GATE_NOGO)	// if didn't actually GO (act > thresh), then no learning!
      snrthal_act = 0.0f;
    
    if(matrix.no_snr_mod)	// disable!
      snrthal_act = 1.0f;

    if(mugp->misc_state1 == PFCGateSpec::ERR_RND_GO) {
      // ERR_RND_GO means that an error occurred and no stripes were firing.
      // this used to be called the all_nogo_mod condition.  
      // in this case, modulate by the raw netinput of the snr units, which is in p_act_m
      snrthal_act = -err_rnd_go.err_da * (snrsu->p_act_m + 1.0f);
    }

    float dav = snrthal_act * u->dav - matrix.neg_da_bl; // da is modulated by snrthal; sub baseline
    if(mugp->misc_state1 == PFCGateSpec::UCOND_RND_GO) {
      dav += rnd_go.ucond_da; 
    }
    if(mugp->misc_state1 == PFCGateSpec::NOGO_RND_GO) {
      dav += rnd_go.nogo_da; 
    }
    if(mugp->misc_state1 == PFCGateSpec::AVGDA_RND_GO) {
      dav += avgda_rnd_go.avgda_da;
    }
    u->dav = dav;		// make it show up in display
    Compute_DaModUnit_Contrast(u, dav, gating_act, go_no);
    idx++;
  }
}

void MatrixLayerSpec::Compute_MotorGate(LeabraLayer* lay, LeabraNetwork*) {
  int snr_prjn_idx = 0;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snr_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)snrthal_lay->spec.spec;

  for(int gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[gi];
    DaModUnit* snru = (DaModUnit*)snrgp->Leaf(0);

    PFCGateSpec::GateSignal gate_sig = PFCGateSpec::GATE_NOGO;
    if(snru->act_eq > snrthalsp->snrthal.go_thr) {
      gate_sig = PFCGateSpec::GATE_GO;
      mugp->misc_state = 0;
    }
    else {
      mugp->misc_state++;
    }
    snrgp->misc_state2 = mugp->misc_state2 = gate_sig; // store the raw gating signal itself
  }
}

void MatrixLayerSpec::Compute_AvgGoDa(LeabraLayer* lay, LeabraNetwork*) {
  int snc_prjn_idx = 0;
  FindLayerFmSpec(lay, snc_prjn_idx, &TA_SNcLayerSpec);

  int gi;
  for(gi=0; gi<lay->units.gp.size; gi++) {
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)lay->units.gp[gi];
    PFCGateSpec::GateSignal gate_sig = (PFCGateSpec::GateSignal)mugp->misc_state2;
    if(gate_sig != PFCGateSpec::GATE_GO) continue; // no action

    DaModUnit* u = (DaModUnit*)mugp->FastEl(0);
    LeabraCon_Group* snccg = (LeabraCon_Group*)u->recv.gp[snc_prjn_idx];
    DaModUnit* sncsu = (DaModUnit*)snccg->Un(0);
    float raw_da = sncsu->dav;	// need to use raw da here because otherwise negatives don't show up!!

    u->misc_1 += avgda_rnd_go.avgda_dt * (raw_da - u->misc_1);
    // copy value to other units, to make it easier to monitor value using unit group monitor stat!
    for(int i=1;i<mugp->size;i++) {
      DaModUnit* nu = (DaModUnit*)mugp->FastEl(i);
      nu->misc_1 = u->misc_1;
    }
  }
}

void MatrixLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  LeabraUnit_Group* mugp = (LeabraUnit_Group*)ug;
  if(bg_type == MatrixLayerSpec::MAINT) {
    if(net->phase_no == 0)
      Compute_DaTonicMod(lay, mugp, thr, net);
    else if(net->phase_no == 1)
      Compute_DaPerfMod(lay, mugp, thr, net);
    else if(net->phase_no == 2)
      Compute_DaLearnMod(lay, mugp, thr, net);
  }
  else {			// OUTPUT
    if(net->phase_no == 0)
      Compute_DaTonicMod(lay, mugp, thr, net);
    else if(net->phase_no == 1)
      Compute_DaLearnMod(lay, mugp, thr, net);
    // don't do anything in 2nd plus!
  }
  LeabraLayerSpec::Compute_Act_impl(lay, ug, thr, net);
}

void MatrixLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(bg_type == MatrixLayerSpec::MAINT) {
    if(net->phase_no == 0)
      Compute_ClearRndGo(lay, net);
    else if(net->phase_no == 1) {
      if(Check_RndGoAvgRew(lay, net)) {
	Compute_UCondNoGoRndGo(lay, net);
	Compute_AvgDaRndGo(lay, net);
      }
      Compute_ErrRndGo(lay, net); // does not depend on avgrew!
    }
  }
  else {			// OUTPUT
    if(net->phase_no == 0) {
      Compute_ClearRndGo(lay, net);
      if(Check_RndGoAvgRew(lay, net)) {
	Compute_UCondNoGoRndGo(lay, net);
	Compute_AvgDaRndGo(lay, net);
      }
    }
    else if(net->phase_no == 1) {
      Compute_ErrRndGo(lay, net); // does not depend on avgrew
    }
  }

  LeabraLayerSpec::Compute_HardClamp(lay, net);
}

void MatrixLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both) {
  LeabraLayerSpec::PostSettle(lay, net, set_both);

  if(bg_type == MatrixLayerSpec::OUTPUT) {
    if(net->phase_no == 0)
      Compute_MotorGate(lay, net);
  }

  if(net->phase_no == 2) {
    Compute_AvgGoDa(lay, net);
  }
}

void MatrixLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  if(bg_type == MatrixLayerSpec::OUTPUT) {
    if((net->phase_max > 2) && (net->phase_no != 1))
      return;
  }
  else {
    if(net->phase_no < net->phase_max-1)	// only final dwt!
      return;
  }
  LeabraLayerSpec::Compute_dWt(lay, net);
}

//////////////////////////////////
//	SNrThal Layer Spec	//
//////////////////////////////////

void SNrThalMiscSpec::Initialize() {
  avg_net_dt = .005f;
  go_thr = 0.1f;
  rnd_go_inc = 0.2f;
}

void SNrThalLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_PCT;
  kwta.pct = .75f;
  SetUnique("tie_brk", true);	// turn on tie breaking by default
  tie_brk.on = true;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("compute_i", true);
  compute_i = KWTA_AVG_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .6f;
}

void SNrThalLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
}

void SNrThalLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
  Initialize();
}

void SNrThalLayerSpec::HelpConfig() {
  String help = "SNrThalLayerSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - da is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nSNrThalLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
 - Sending connections must connect to units of type DaModUnit/Spec \
     (da signal from this layer put directly into da var on units)\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
}

bool SNrThalLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig_Layer(lay, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::CheckError("SNrThalLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  // must have the appropriate ranges for unit specs..
  //  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;

  // check recv connection
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixLayerSpec);

  if(matrix_lay == NULL) {
    if (!quiet) taMisc::CheckError("SNrThalLayerSpec: did not find Matrix layer to recv from!");
    return false;
  }

  if(matrix_lay->units.gp.size != lay->units.gp.size) {
    if (!quiet) taMisc::CheckError("SNrThalLayerSpec: MatrixLayer unit groups must = SNrThalLayer unit groups!");
    lay->gp_geom.n = matrix_lay->units.gp.size;
    return false;
  }

  int myidx = lay->own_net->layers.FindLeaf(lay);
  int matidx = lay->own_net->layers.FindLeaf(matrix_lay);
  if(matidx > myidx) {
    if (!quiet) taMisc::CheckError("SNrThalLayerSpec: Matrix layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }
  return true;
}

// include matrix XXX_RND_GO signals directly in here
// as an extra increment to the go vs. nogo balance.  this provides
// direct visual confirmation of what is happening, and more sensible correspondence
// with what we think is really going on: random extra activation in snrthal pathway.

void SNrThalLayerSpec::Compute_GoNogoNet(LeabraLayer* lay, LeabraNetwork* net) {
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixLayerSpec);
  MatrixLayerSpec* mls = (MatrixLayerSpec*)matrix_lay->spec.spec;

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    float gonogo = 0.0f;
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    MatrixUnitSpec* us = (MatrixUnitSpec*)matrix_lay->unit_spec.spec;
    if((mugp->size > 0) && (mugp->acts.max >= us->opt_thresh.send)) {
      float sum_go = 0.0f;
      float sum_nogo = 0.0f;
      for(int i=0;i<mugp->size;i++) {
	DaModUnit* u = (DaModUnit*)mugp->FastEl(i);
	PFCGateSpec::GateSignal go_no = (PFCGateSpec::GateSignal)(i % 2); // GO = 0, NOGO = 1
	if(go_no == PFCGateSpec::GATE_GO)
	  sum_go += u->act_eq;
	else
	  sum_nogo += u->act_eq;
      }
      if(sum_go + sum_nogo > 0.0f) {
	gonogo = (sum_go - sum_nogo) / (sum_go + sum_nogo);
      }
      if(mugp->misc_state1 >= PFCGateSpec::UCOND_RND_GO) {
	// do NOT add rnd_go_inc for OUTPUT ERR_RND_GO because it is a learning only case, NOT performance go!
	if(!((mls->bg_type == MatrixLayerSpec::OUTPUT) && (mugp->misc_state1 == PFCGateSpec::ERR_RND_GO))) {
	  gonogo += snrthal.rnd_go_inc;
	  if(gonogo > 1.0f) gonogo = 1.0f;
	}
      }
    }
    for(int i=0;i<rugp->size;i++) {
      DaModUnit* ru = (DaModUnit*)rugp->FastEl(i);
      ru->net = gonogo;
      if(net->phase == LeabraNetwork::MINUS_PHASE)
	ru->p_act_m = gonogo;	// save this for err_rnd_go computation
    }
  }
}

void SNrThalLayerSpec::Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_GoNogoNet(lay, net);
  LeabraLayerSpec::Compute_Clamp_NetAvg(lay, net);
}

void SNrThalLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork*) {
  DaModUnit* u = (DaModUnit*)lay->units.Leaf(0);
  float cur_avg = u->misc_1;
  float new_avg = cur_avg + snrthal.avg_net_dt * (lay->netin.avg - cur_avg);
  taLeafItr ui;
  FOR_ITR_EL(DaModUnit, u, lay->units., ui) {
    u->misc_1 = new_avg;
  }
}


//////////////////////////////////
//	PFC Layer Spec		//
//////////////////////////////////

void PFCGateSpec::Initialize() {
  off_accom = 0.0f;
  updt_reset_sd = true;
}

void PFCLayerSpec::Initialize() {
  SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .15f;
  SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
  SetUnique("compute_i", true);
  compute_i = KWTA_AVG_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .6f;
  SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
}

void PFCLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
  gate.Initialize();
  Initialize();
}

void PFCLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(gate, this);
}

void PFCLayerSpec::HelpConfig() {
  String help = "PFCLayerSpec Computation:\n\
 The PFC maintains activation over time (activation-based working memory) via\
 excitatory intracelluar ionic mechanisms (implemented via the hysteresis channels, gc.h),\
 and excitatory self-connections. These ion channels are toggled on and off via units in the\
 SNrThalLayerSpec layer, which are themsepves driven by MatrixLayerSpec units,\
 which are in turn trained up by dynamic dopamine changes computed by the PVLV system.\
 Updating occurs at the end of the 1st plus phase --- if a gating signal was activated, any previous ion\
 current is turned off, and the units are allowed to settle into a new state in the 2nd plus (update) --\
 then the ion channels are activated in proportion to activations at the end of this 2nd phase.\n\
 \nPFCLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Units must recv MarkerConSpec from SNrThalLayerSpec layer for gating\n\
 - This layer must be after SNrThalLayerSpec layer in list of layers\n\
 - Units must be organized into groups corresponding to the matrix groups (stripes).";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
}

bool PFCLayerSpec::CheckConfig_Layer(LeabraLayer* lay,  bool quiet) {
  if(!LeabraLayerSpec::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;

  if(lay->units.gp.size == 1) {
    if (!quiet) taMisc::CheckError("PFCLayerSpec: layer must contain multiple unit groups (= stripes) for indepent searching of gating space!");
    return false;
  }

  if(net->phase_order != LeabraNetwork::MINUS_PLUS_PLUS) {
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires LeabraNetwork phase_oder = MINUS_PLUS_PLUS, I just set it for you");
    net->phase_order = LeabraNetwork::MINUS_PLUS_PLUS;
  }
  if(net->first_plus_dwt != LeabraNetwork::ONLY_FIRST_DWT) {
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires LeabraNetwork first_plus_dwt = ONLY_FIRST_DWT, I just set it for you");
    net->first_plus_dwt = LeabraNetwork::ONLY_FIRST_DWT;
  }

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(net->no_plus_test) {
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires LeabraNetwork no_plus_test = false, I just set it for you");
    net->no_plus_test = false;
  }

  if(net->min_cycles_phase2 < 35) {
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires LeabraNetwork min_cycles_phase2 >= 35, I just set it for you");
    net->min_cycles_phase2 = 35;
  }

  if(net->sequence_init != LeabraNetwork::DO_NOTHING) {
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires network sequence_init = DO_NOTHING, I just set it for you");
    net->sequence_init = LeabraNetwork::DO_NOTHING;
  }

  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    if (!quiet) taMisc::CheckError("PFCLayerSpec: must have DaModUnits!");
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    if (!quiet) taMisc::CheckError("PFCLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }

  if(us->act.avg_dt <= 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.005f;
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires UnitSpec act.avg_dt > 0, I just set it to .005 for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->SetUnique("g_bar", true);
  if(us->hyst.init) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires UnitSpec hyst.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if(us->acc.init) {
    us->SetUnique("acc", true);
    us->acc.init = false;
    if(!quiet) taMisc::CheckError("PFCLayerSpec: requires UnitSpec acc.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  for(g=0;g<u->recv.gp.size; g++) {
    recv_gp = (LeabraCon_Group*)u->recv.gp[g];
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from;
    if(fmlay == NULL) {
      if (!quiet) taMisc::CheckError("*** PFCLayerSpec: null from layer in recv projection:", (String)g);
      return false;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(cs->InheritsFrom(TA_MarkerConSpec)) continue;
    // could check the conspec parameters here..
  }

  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  if(snrthal_lay == NULL) {
    if (!quiet) taMisc::CheckError("*** PFCLayerSpec: Warning: no projection from SNrThal Layer found: must have MarkerConSpec!");
    return false;
  }
  if(snrthal_lay->units.gp.size != lay->units.gp.size) {
    if (!quiet) taMisc::CheckError("PFCLayerSpec: Gating Layer unit groups must = PFCLayer unit groups!");
    snrthal_lay->gp_geom.n = lay->units.gp.size;
    return false;
  }

  // check for ordering of layers!
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int gateidx = lay->own_net->layers.FindLeaf(snrthal_lay);
  if(gateidx > myidx) {
    if (!quiet) taMisc::CheckError("PFCLayerSpec: SNrThal Layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  return true;
}

void PFCLayerSpec::ResetSynDep(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  LeabraCon_Group* send_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
    if(!send_gp->spec.spec->InheritsFrom(TA_TrialSynDepConSpec)) continue;
    TrialSynDepConSpec* cs = (TrialSynDepConSpec*)send_gp->spec.spec;
    cs->Reset_EffWt(send_gp);
  }
}


void PFCLayerSpec::Compute_MaintUpdt(LeabraUnit_Group* ugp, MaintUpdtAct updt_act, LeabraLayer* lay, LeabraNetwork* net) {
  if(updt_act == NO_UPDT) return;
  for(int j=0;j<ugp->size;j++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(j);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
    if(updt_act == STORE) {
      u->vcb.g_h = u->misc_1 = u->act_eq;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
      if(gate.updt_reset_sd)
	ResetSynDep(u, lay, net);
    }
    else if(updt_act == CLEAR) {
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = gate.off_accom * u->vcb.g_h;
      u->vcb.g_h = u->misc_1 = 0.0f;
      if(gate.updt_reset_sd)
	ResetSynDep(u, lay, net);
    }
    else if(updt_act == RESTORE) {
      u->vcb.g_h = u->act_eq = u->misc_1;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    else if(updt_act == TMP_STORE) {
      u->vcb.g_h = u->act_eq;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    else if(updt_act == TMP_CLEAR) {
      u->vcb.g_h = 0.0f;
      if(gate.off_accom > 0.0f)
	u->vcb.g_a = 0.0f;
    }
    us->Compute_Conduct(u, lay, (LeabraInhib*)ugp, net); // update displayed conductances!
  }
  if(updt_act == STORE) ugp->misc_state = 1;
  else if(updt_act == CLEAR) ugp->misc_state = 0;
}

void PFCLayerSpec::Compute_TmpClear(LeabraLayer* lay, LeabraNetwork* net) {
  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    Compute_MaintUpdt(ugp, TMP_CLEAR, lay, net); // temporary clear for trans input!
  }
}

void PFCLayerSpec::Compute_GatingGOGO(LeabraLayer* lay, LeabraNetwork* net) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)snrthal_lay->spec.spec;

  for(int mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    DaModUnit* snru = (DaModUnit*)snrgp->Leaf(0);

    PFCGateSpec::GateSignal gate_sig = PFCGateSpec::GATE_NOGO;
    if(snru->act_eq > snrthalsp->snrthal.go_thr) {
      gate_sig = PFCGateSpec::GATE_GO;
    }

    ugp->misc_state2 = gate_sig; // store the raw gating signal itself

    if(net->phase_no == 1) {
      if(ugp->misc_state <= 0) { // empty stripe
	if(gate_sig == PFCGateSpec::GATE_GO) {
	  ugp->misc_state1 = PFCGateSpec::EMPTY_GO;
	  Compute_MaintUpdt(ugp, STORE, lay, net);
	}
	else {
	  ugp->misc_state1 = PFCGateSpec::EMPTY_NOGO;
	  ugp->misc_state--;	// more time off
	}
      }
      else {			// latched stripe
	if(gate_sig == PFCGateSpec::GATE_GO) {
	  ugp->misc_state1 = PFCGateSpec::LATCH_GO;
	  Compute_MaintUpdt(ugp, CLEAR, lay, net); // clear in first phase
	}
	else {
	  ugp->misc_state1 = PFCGateSpec::LATCH_NOGO;
	  ugp->misc_state++;  // keep on mainting
	}
      }
    }
    else {			// second plus (2m)
      if(ugp->misc_state <= 0) {
	if(gate_sig == PFCGateSpec::GATE_GO) {
	  if(ugp->misc_state1 == PFCGateSpec::LATCH_GO)
	    ugp->misc_state1 = PFCGateSpec::LATCH_GOGO;
	  else
	    ugp->misc_state1 = PFCGateSpec::EMPTY_GO;
	  Compute_MaintUpdt(ugp, STORE, lay, net);
	}
      }
    }
  }
  SendGateStates(lay, net);
}

void PFCLayerSpec::SendGateStates(LeabraLayer* lay, LeabraNetwork*) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(snrthal_lay, mtx_prjn_idx, &TA_MatrixLayerSpec);
  int mg;
  for(mg=0;mg<lay->units.gp.size;mg++) {
    LeabraUnit_Group* ugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
    LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    // everybody gets gate state info from PFC!
    snrgp->misc_state = mugp->misc_state = ugp->misc_state;
    snrgp->misc_state1 = ugp->misc_state1; 
    if(mugp->misc_state1 < PFCGateSpec::UCOND_RND_GO) { // don't override random go signals
      mugp->misc_state1 = ugp->misc_state1;
    }
    snrgp->misc_state2 = mugp->misc_state2 = ugp->misc_state2;
  }
}

void PFCLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(gate.allow_clamp && clamp.hard && (lay->ext_flag & Unit::EXT)) {
    LeabraLayerSpec::Compute_HardClamp(lay, net);
  }
  else {
    // not to hard clamp: needs to update in 2nd plus phase!
    lay->hard_clamped = false;
    lay->InitExterns();
  }
}

void PFCLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both) {
  LeabraLayerSpec::PostSettle(lay, net, set_both);

  if(net->phase_no >= 1) {
    Compute_GatingGOGO(lay, net);	// do gating
  }
}

void PFCLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->phase_max > 2) && (net->phase_no != 1))
    return; // only do first dwt!
  LeabraLayerSpec::Compute_dWt(lay, net);
}


//////////////////////////////////
//	PFCOut Layer Spec	//
//////////////////////////////////

void PFCOutGateSpec::Initialize() {
  base_gain = 0.5f;
  go_gain = 0.5f;
  graded_go = false;
}

void PFCOutLayerSpec::Initialize() {
  // this guy should always inherit from PFCLayerSpec
//   SetUnique("gp_kwta", true);
  gp_kwta.k_from = KWTASpec::USE_PCT;
  gp_kwta.pct = .15f;
//   SetUnique("inhib_group", true);
  inhib_group = UNIT_GROUPS;
//   SetUnique("compute_i", true);
  compute_i = KWTA_AVG_INHIB;
//   SetUnique("i_kwta_pt", true);
  i_kwta_pt = .6f;
//   SetUnique("decay", true);
  decay.event = 0.0f;
  decay.phase = 0.0f;
  decay.phase2 = 0.1f;
  decay.clamp_phase2 = false;	// this is the one exception!
}

void PFCOutLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
  out_gate.Initialize();
  Initialize();
}

void PFCOutLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(out_gate, this);
}

void PFCOutLayerSpec::HelpConfig() {
  String help = "PFCOutLayerSpec Computation:\n\
 The PFCOut layer gets activations from corresponding PFC layer (via MarkerCon)\
 and sets unit activations as a function of the Go gating signals received from\
 associated SNrThal layer\n\
 \nPFCOutLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure PFC and BG layers.\n\
 - Units must recv MarkerConSpec from SNrThalLayerSpec layer for gating\n\
 - Units must recv MarkerConSpec from PFCLayerSpec layer for activations\n\
 - This layer must be after SNrThalLayerSpec layer in list of layers\n\
 - This layer must be after PFCLayerSpec layer in list of layers\n\
 - Units must be organized into groups corresponding to the matrix groups (stripes).";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
}

bool PFCOutLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig_Layer(lay, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  if(lay->units.gp.size == 1) {
    if (!quiet) taMisc::CheckError("PFCOutLayerSpec: layer must contain multiple unit groups (= stripes) for indepent searching of gating space!");
    return false;
  }

  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    if (!quiet) taMisc::CheckError("PFCOutLayerSpec: must have DaModUnits!");
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    if (!quiet) taMisc::CheckError("PFCOutLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }

  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  if(snrthal_lay == NULL) {
    if (!quiet) taMisc::CheckError("*** PFCOutLayerSpec: Warning: no projection from SNrThal Layer found: must have MarkerConSpec!");
    return false;
  }
  if(snrthal_lay->units.gp.size != lay->units.gp.size) {
    if (!quiet) taMisc::CheckError("PFCOutLayerSpec: Gating Layer unit groups must = PFCOutLayer unit groups!");
    snrthal_lay->unit_groups = true;
    snrthal_lay->gp_geom.n = lay->units.gp.size;
    return false;
  }

  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);
  if(pfc_lay == NULL) {
    if (!quiet) taMisc::CheckError("*** PFCOutLayerSpec: Warning: no projection from PFC Layer found: must have MarkerConSpec!");
    return false;
  }
  if(pfc_lay->units.gp.size != lay->units.gp.size) {
    if(!quiet)
      taMisc::CheckError("PFCOutLayerSpec: PFC Layer unit groups must = PFCOutLayer unit groups, copiped from PFC Layer; Please do a Build of network");
    lay->unit_groups = true;
    lay->gp_geom.n = pfc_lay->units.gp.size;
  }
  if(pfc_lay->units.leaves != lay->units.leaves) {
    if(!quiet)
      taMisc::CheckError("PFCOutLayerSpec: PFC Layer units must = PFCOutLayer units, copied from PFC Layer; Please do a Build of network");
    lay->un_geom = pfc_lay->un_geom;
  }

  PFCLayerSpec* pfcsp = (PFCLayerSpec*)pfc_lay->spec.spec;
  kwta = pfcsp->kwta;
  gp_kwta = pfcsp->gp_kwta;
  inhib_group = pfcsp->inhib_group;
  compute_i = pfcsp->compute_i;
  i_kwta_pt = pfcsp->i_kwta_pt;

  // check for ordering of layers!
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int gateidx = lay->own_net->layers.FindLeaf(snrthal_lay);
  if(gateidx > myidx) {
    if (!quiet) taMisc::CheckError("PFCOutLayerSpec: SNrThal Layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  // check for ordering of layers!
  int pfcidx = lay->own_net->layers.FindLeaf(pfc_lay);
  if(pfcidx > myidx) {
    if (!quiet) taMisc::CheckError("PFCOutLayerSpec: PFC Layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  return true;
}

void PFCOutLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork*) {
  // not to hard clamp: needs to update in 2nd plus phase!
  lay->hard_clamped = false;
  lay->InitExterns();
  return;
}

void PFCOutLayerSpec::Compute_Inhib(LeabraLayer*, LeabraNetwork*) {
  return;			// do nothing!
}

void PFCOutLayerSpec::Compute_InhibAvg(LeabraLayer*, LeabraNetwork*) {
  return;
}

void PFCOutLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  SNrThalLayerSpec* snrthalsp = (SNrThalLayerSpec*)snrthal_lay->spec.spec;
//   int mtx_prjn_idx;
//   LeabraLayer* matrix_lay = FindLayerFmSpec(snrthal_lay, mtx_prjn_idx, &TA_MatrixLayerSpec);
  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);

  for(int mg=0; mg<lay->units.gp.size; mg++) {
    LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[mg];
    LeabraUnit_Group* pfcgp = (LeabraUnit_Group*)pfc_lay->units.gp[mg];
    LeabraUnit_Group* snrgp = (LeabraUnit_Group*)snrthal_lay->units.gp[mg];
//     LeabraUnit_Group* mugp = (LeabraUnit_Group*)matrix_lay->units.gp[mg];
    DaModUnit* snru = (DaModUnit*)snrgp->Leaf(0);

    // note that random go is added into activation at the snrthal level, not here.

    float gain = out_gate.base_gain;
    if(snru->act_eq > snrthalsp->snrthal.go_thr) {
      if(out_gate.graded_go) 
	gain += snru->act_eq * out_gate.go_gain;
      else
	gain += out_gate.go_gain;
    }
    
    for(int i=0;i<rugp->size;i++) {
      DaModUnit* ru = (DaModUnit*)rugp->FastEl(i);
      DaModUnit* pfcu = (DaModUnit*)pfcgp->FastEl(i);
      
      ru->act = gain * pfcu->act;
      ru->act_eq = ru->act;
      ru->da = 0.0f;		// I'm fully settled!
      ru->act_delta = ru->act - ru->act_sent;
    }
  }
  Compute_ActAvg(lay, net);
}

void PFCOutLayerSpec::Compute_dWt(LeabraLayer*, LeabraNetwork*) {
  return; 			// no need to do this!
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

///////////////////////////////////////////////////////////////
//			BgPFC
///////////////////////////////////////////////////////////////

static void set_n_stripes(LeabraNetwork* net, char* nm, int n_stripes, int n_units, bool sp) {
  LeabraLayer* lay = (LeabraLayer*)net->FindLayer(nm);
  if(lay == NULL) return;
  lay->gp_geom.n = n_stripes;
  lay->unit_groups = true;
  if(n_units > 0) lay->un_geom.n = n_units;
  if(sp) {
    lay->gp_spc.x = 1;
    lay->gp_spc.y = 1;
  }
  lay->UpdateAfterEdit();
  if(n_stripes <= 4) {
    if(lay->name.contains("Patch")) {
      lay->gp_geom.x = 1;
      lay->gp_geom.y = n_stripes;
    }
    else {
      lay->gp_geom.x = n_stripes;
      lay->gp_geom.y = 1;
    }
  }
}

static void lay_set_geom(LeabraLayer* lay, int half_stripes) {
  lay->unit_groups = true;
  lay->gp_geom.n = half_stripes * 2;
  lay->gp_geom.y = 2; lay->gp_geom.x = half_stripes;
  lay->UpdateAfterEdit();
}

void LeabraWizard::SetPFCStripes(LeabraNetwork* net, int n_stripes, int n_units) {
  set_n_stripes(net, "PFC", n_stripes, n_units, true);
  set_n_stripes(net, "PFC_mnt", n_stripes, n_units, true);
  set_n_stripes(net, "PFC_out", n_stripes, n_units, true);
  set_n_stripes(net, "Matrix", n_stripes, -1, true);
  set_n_stripes(net, "Matrix_mnt", n_stripes, -1, true);
  set_n_stripes(net, "Matrix_out", n_stripes, -1, true);
  set_n_stripes(net, "Patch", n_stripes, -1, true);
  set_n_stripes(net, "SNc", n_stripes, -1, false);
  set_n_stripes(net, "SNrThal", n_stripes, -1, false);
  set_n_stripes(net, "SNrThal_mnt", n_stripes, -1, false);
  set_n_stripes(net, "SNrThal_out", n_stripes, -1, false);
  net->LayoutUnitGroups();
  net->Build();
  net->Connect();
}

void LeabraWizard::BgPFC(LeabraNetwork* net, bool bio_labels, bool localist_val, bool fm_hid_cons, bool fm_out_cons, bool da_mod_all, int n_stripes, bool mat_fm_pfc_full, bool out_gate, bool nolrn_pfc, bool lr_sched) {
  PVLV(net, bio_labels, localist_val, fm_hid_cons, fm_out_cons, da_mod_all); // first configure PVLV system..

  String msg = "Configuring BG PFC (Basal Ganglia Prefrontal Cortex) Layers:\n\n\
 There is one thing you will need to check manually after this automatic configuration\
 process completes (this note will be repeated when things complete --- there may be some\
 messages in the interim):\n\n";

  String man_msg = "1. Check the bidirectional connections between the PFC and all appropriate hidden layers\
 The con specs INTO the PFC should be ToPFC conspecs; the ones out should be regular learning conspecs.";

  msg += man_msg + "\n\nThe configuration will now be checked and a number of default parameters\
 will be set.  If there are any actual errors which must be corrected before\
 the network will run, you will see a message to that effect --- you will then need to\
 re-run this configuration process to make sure everything is OK.  When you press\
 Re/New/Init on the control process these same checks will be performed, so you\
 can be sure everything is ok.";
  taMisc::Choice(msg,"Ok");

  int half_stripes = n_stripes /2;
  half_stripes = MAX(1, half_stripes);
  n_stripes = half_stripes * 2;	// make it even

  net->RemoveUnits();

  String pvenm = "PVe_LHA";  String pvinm = "PVi_VSpatch";
  String lvenm = "LVe_CNA";  String lvinm = "LVi_VSpatch";
  String vtanm = "VTA";
  if(!bio_labels) {
    pvenm = "PVe"; 	pvinm = "PVi";    lvenm = "LVe";    lvinm = "LVi";    vtanm = "DA";
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  // get these from the DA function..
  LeabraLayer* rew_targ_lay = (LeabraLayer*)net->FindLayer("RewTarg");
  LeabraLayer* pve = (LeabraLayer*)net->FindLayer(pvenm);
  LeabraLayer* pvi = (LeabraLayer*)net->FindLayer(pvinm);
  LeabraLayer* lve = (LeabraLayer*)net->FindLayer(lvenm);
  LeabraLayer* lvi = (LeabraLayer*)net->FindLayer(lvinm);
  LeabraLayer* vta = (LeabraLayer*)net->FindLayer(vtanm);
  if(rew_targ_lay == NULL || lve == NULL || pve == NULL || pvi == NULL || vta == NULL) return;

//   LeabraLayer* patch = NULL;
//   bool patch_new = false;

  // if not new layers, don't make prjns into them!
  bool	snc_new = false;
  bool matrix_m_new = false;  bool snrthal_m_new = false; bool pfc_m_new = false;
  bool matrix_o_new = false;  bool snrthal_o_new = false; bool pfc_o_new = false;

  LeabraLayer* snc = (LeabraLayer*)net->FindMakeLayer("SNc", NULL, snc_new);

  LeabraLayer* matrix_m = NULL;
  LeabraLayer* snrthal_m = NULL;
  LeabraLayer* pfc_m = NULL;

  LeabraLayer* matrix_o = NULL;	// output gate versions
  LeabraLayer* snrthal_o = NULL;
  LeabraLayer* pfc_o = NULL;

  if(out_gate) {
    matrix_m = (LeabraLayer*)net->FindMakeLayer("Matrix_mnt", NULL, matrix_m_new, "Matrix");
    matrix_o = (LeabraLayer*)net->FindMakeLayer("Matrix_out", NULL, matrix_o_new);

    snrthal_m = (LeabraLayer*)net->FindMakeLayer("SNrThal_mnt", NULL, snrthal_m_new, "SNrThal");
    snrthal_o = (LeabraLayer*)net->FindMakeLayer("SNrThal_out", NULL, snrthal_o_new);

    pfc_m = (LeabraLayer*)net->FindMakeLayer("PFC_mnt", NULL, pfc_m_new, "PFC");
    pfc_o = (LeabraLayer*)net->FindMakeLayer("PFC_out", NULL, pfc_o_new);
  }
  else {
    matrix_m = (LeabraLayer*)net->FindMakeLayer("Matrix", NULL, matrix_m_new, "Matrix");
    snrthal_m = (LeabraLayer*)net->FindMakeLayer("SNrThal", NULL, snrthal_m_new, "SNrThal");
    pfc_m = (LeabraLayer*)net->FindMakeLayer("PFC", NULL, pfc_m_new);
  }

//   if(make_patch)  { patch = (LeabraLayer*)net->FindMakeLayer("Patch", NULL, patch_new); }
//   else 		  { net->layers.Remove("Patch"); }

  if(matrix_m == NULL || snrthal_m == NULL || pfc_m == NULL) return;

  //////////////////////////////////////////////////////////////////////////////////
  // sort layers

  rew_targ_lay->name = "0000";  pve->name = "0001"; pvi->name = "0002";  
  lve->name = "0003";  lvi->name = "0004";    vta->name = "0005";

  snc->name = "ZZZ3";
  matrix_m->name = "ZZZ4";  
  snrthal_m->name = "ZZZ6";
  pfc_m->name = "ZZZ8";
  if(out_gate) {
    matrix_o->name = "ZZZ5";  
    snrthal_o->name = "ZZZ7";
    pfc_o->name = "ZZZ9";
  }
//   if(make_patch) 	{ patch->name = "ZZZ1"; }

  net->layers.Sort();

  rew_targ_lay->name = "RewTarg";  pve->name = pvenm;  pvi->name = pvinm;
  lve->name = lvenm;  	lvi->name = lvinm;	vta->name = vtanm;
  snc->name = "SNc";
  if(out_gate) {
    snrthal_m->name = "SNrThal_mnt";
    snrthal_o->name = "SNrThal_out";
    matrix_m->name = "Matrix_mnt";
    matrix_o->name = "Matrix_out";
    pfc_m->name = "PFC_mnt";
    pfc_o->name = "PFC_out";
  }
  else {
    snrthal_m->name = "SNrThal";
    matrix_m->name = "Matrix";
    pfc_m->name = "PFC";
  }
//   if(make_patch)	{ patch->name = "Patch"; }

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  int mx_z1 = 0;		// max x coordinate on layer z=1
  int mx_z2 = 0;		// z=2
  Layer_Group other_lays;  Layer_Group hidden_lays;
  Layer_Group output_lays;  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    lay->SetUnitType(&TA_DaModUnit);
    if(lay != rew_targ_lay && lay != pve && lay != pvi && lay != lve && lay != lvi && lay != vta
       && lay != snc && lay != snrthal_m && lay != matrix_m && lay != pfc_m
       && lay != snrthal_o && lay != matrix_o && lay != pfc_o) {
      other_lays.Link(lay);
      int xm = lay->pos.x + lay->act_geom.x + 1;
      if(lay->pos.z == 1) mx_z1 = MAX(mx_z1, xm);
      if(lay->pos.z == 2) mx_z2 = MAX(mx_z2, xm);
      if(lay->layer_type == Layer::HIDDEN)
	hidden_lays.Link(lay);
      else if(lay->layer_type == Layer::INPUT)
	input_lays.Link(lay);
      else 
	output_lays.Link(lay);
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
      if(us == NULL || !us->InheritsFrom(TA_DaModUnitSpec)) {
	us->ChangeMyType(&TA_DaModUnitSpec);
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // make specs

  BaseSpec_Group* units = net->FindMakeSpecGp("PFC_BG_Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp("PFC_BG_Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp("PFC_BG_Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp("PFC_BG_Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return;

//   LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_DaModUnitSpec);

  LeabraUnitSpec* pfc_units = (LeabraUnitSpec*)units->FindMakeSpec("PFCUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* matrix_units = (LeabraUnitSpec*)units->FindMakeSpec("MatrixUnits", &TA_MatrixUnitSpec);
  LeabraUnitSpec* snrthal_units = (LeabraUnitSpec*)units->FindMakeSpec("SNrThalUnits", &TA_DaModUnitSpec);
  if(pfc_units == NULL || matrix_units == NULL) return;
  MatrixUnitSpec* matrixo_units = NULL;
  if(out_gate) {
    matrixo_units = (MatrixUnitSpec*)matrix_units->FindMakeChild("MatrixOut", &TA_MatrixUnitSpec);
  }

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(learn_cons == NULL) return;

  LeabraConSpec* pvi_cons = (LeabraConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  LeabraConSpec* lve_cons = (LeabraConSpec*)pvi_cons->FindMakeChild("LVe", &TA_LVConSpec);
  LeabraConSpec* lvi_cons = (LeabraConSpec*)lve_cons->FindMakeChild("LVi", &TA_LVConSpec);

  LeabraConSpec* topfc_cons = (LeabraConSpec*)learn_cons->FindMakeChild("ToPFC", &TA_LeabraConSpec);
  if(topfc_cons == NULL) return;
  LeabraConSpec* intra_pfc = (LeabraConSpec*)topfc_cons->FindMakeChild("IntraPFC", &TA_LeabraConSpec);
  LeabraConSpec* pfc_bias = (LeabraConSpec*)topfc_cons->FindMakeChild("PFCBias", &TA_LeabraBiasSpec);
  MatrixConSpec* matrix_cons = (MatrixConSpec*)learn_cons->FindMakeChild("MatrixCons", &TA_MatrixConSpec);
  MatrixConSpec* mfmpfc_cons = (MatrixConSpec*)matrix_cons->FindMakeChild("MatrixFmPFC", &TA_MatrixConSpec);

  MatrixConSpec* matrixo_cons = NULL;
  MatrixConSpec* mofmpfc_cons = NULL;
  if(out_gate) {
    matrixo_cons = (MatrixConSpec*)matrix_cons->FindMakeChild("Matrix_out", &TA_MatrixConSpec);
    mofmpfc_cons = (MatrixConSpec*)matrixo_cons->FindMakeChild("Matrix_out_FmPFC", &TA_MatrixConSpec);
  }
  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  LeabraConSpec* pfc_self = (LeabraConSpec*)cons->FindMakeSpec("PFCSelfCon", &TA_LeabraConSpec);

  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return;
  LeabraConSpec* matrix_bias = (LeabraConSpec*)bg_bias->FindMakeChild("MatrixBias", &TA_MatrixBiasSpec);
  if(pfc_self == NULL || intra_pfc == NULL || matrix_cons == NULL || marker_cons == NULL 
     || matrix_bias == NULL)
    return;

//   LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec);
  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindMakeSpec(vtanm + "Layer", &TA_PVLVDaLayerSpec);
  LeabraLayerSpec* pfcmsp = (LeabraLayerSpec*)layers->FindMakeSpec("PFCLayer", &TA_PFCLayerSpec);
  LeabraLayerSpec* pfcosp = NULL;
  if(out_gate)
    pfcosp = (LeabraLayerSpec*)pfcmsp->FindMakeChild("PFCOutLayer", &TA_PFCOutLayerSpec);
  LeabraLayerSpec* sncsp = (LeabraLayerSpec*)dasp->FindMakeChild("SNcLayer", &TA_SNcLayerSpec);
  MatrixLayerSpec* matrixsp = (MatrixLayerSpec*)layers->FindMakeSpec("MatrixLayer", &TA_MatrixLayerSpec);
  if(pfcmsp == NULL || matrixsp == NULL) return;
//   LeabraLayerSpec* patchsp = (LeabraLayerSpec*)lvesp->FindMakeChild("PatchLayer", &TA_PatchLayerSpec);

  MatrixLayerSpec* matrixosp = NULL;
  if(out_gate)
    matrixosp = (MatrixLayerSpec*)matrixsp->FindMakeChild("Matrix_out", &TA_MatrixLayerSpec);

  LeabraLayerSpec* snrthalsp = (LeabraLayerSpec*)layers->FindMakeSpec("SNrThalLayer", &TA_SNrThalLayerSpec);
  LeabraLayerSpec* snrthalosp = NULL;
  if(out_gate)
    snrthalosp = (LeabraLayerSpec*)snrthalsp->FindMakeChild("SNrThalOut", &TA_SNrThalLayerSpec);

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* gponetoone = (ProjectionSpec*)prjns->FindMakeSpec("GpOneToOne", &TA_GpOneToOnePrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  UniformRndPrjnSpec* topfc = (UniformRndPrjnSpec*)prjns->FindMakeSpec("ToPFC", &TA_UniformRndPrjnSpec);
  ProjectionSpec* pfc_selfps = (ProjectionSpec*)prjns->FindMakeSpec("PFCSelf", &TA_OneToOnePrjnSpec);
  GpRndTesselPrjnSpec* intra_pfcps = (GpRndTesselPrjnSpec*)prjns->FindMakeSpec("IntraPFC", &TA_GpRndTesselPrjnSpec);
  TesselPrjnSpec* input_pfc = (TesselPrjnSpec*)prjns->FindMakeSpec("Input_PFC", &TA_TesselPrjnSpec);
  if(topfc == NULL || pfc_selfps == NULL || intra_pfcps == NULL || gponetoone == NULL || input_pfc == NULL) return;

  input_pfc->send_offs.New(1); // this is all it takes!

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

//   if(make_patch) 	{ patch->SetLayerSpec(patchsp); patch->SetUnitSpec(lv_units); }
  snc->SetLayerSpec(sncsp); snc->SetUnitSpec(da_units);
  snrthal_m->SetLayerSpec(snrthalsp); snrthal_m->SetUnitSpec(snrthal_units);
  matrix_m->SetLayerSpec(matrixsp);   matrix_m->SetUnitSpec(matrix_units);
  pfc_m->SetLayerSpec(pfcmsp);	pfc_m->SetUnitSpec(pfc_units);
  if(out_gate) {
    snrthal_o->SetLayerSpec(snrthalosp); snrthal_o->SetUnitSpec(snrthal_units);
    matrix_o->SetLayerSpec(matrixosp);   matrix_o->SetUnitSpec(matrixo_units);
    pfc_o->SetLayerSpec(pfcosp);	pfc_o->SetUnitSpec(pfc_units);
  }

  // set bias specs for unit specs
  pfc_units->bias_spec.SetSpec(pfc_bias);
  matrix_units->bias_spec.SetSpec(matrix_bias);
  
  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(snc, pvi, fullprjn, marker_cons);
  net->FindMakePrjn(snc, lve, fullprjn, marker_cons);
  net->FindMakePrjn(snc, lvi, fullprjn, marker_cons);
  net->FindMakePrjn(matrix_m, snc, gponetoone, marker_cons);

//   if(make_patch) {
//     // todo: not right..
//     net->FindMakePrjn(patch, pve, fullprjn, marker_cons);
//     net->FindMakePrjn(patch, snc, gponetoone, marker_cons);
//     net->FindMakePrjn(snc, patch, gponetoone, marker_cons);
//   }

  net->FindMakePrjn(snrthal_m, matrix_m, gponetoone, marker_cons);
  net->FindMakePrjn(pfc_m, snrthal_m, gponetoone, marker_cons);
  net->FindMakePrjn(matrix_m, snrthal_m, gponetoone, marker_cons);

  if(out_gate) {
    net->FindMakePrjn(matrix_o, snc, gponetoone, marker_cons);

    net->FindMakePrjn(snrthal_o, matrix_o, gponetoone, marker_cons);
    net->FindMakePrjn(pfc_o, snrthal_o, gponetoone, marker_cons);
    net->FindMakePrjn(matrix_o, snrthal_o, gponetoone, marker_cons);

    net->FindMakePrjn(pfc_o, pfc_m, onetoone, marker_cons);

    // todo: what kind of descending connectivity should this have??
    // basic assumption would be that all subcortical comes from _o!
    // but it could have prjns from pfc_m via subsets of non-gated pfc units..
    if(mat_fm_pfc_full) {
      net->FindMakePrjn(matrix_m, pfc_m, fullprjn, mfmpfc_cons);
      net->FindMakePrjn(matrix_o, pfc_m, fullprjn, mofmpfc_cons);
    }
    else {
      net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);
      net->FindMakePrjn(matrix_o, pfc_m, gponetoone, mofmpfc_cons);
    }

    net->FindMakeSelfPrjn(pfc_m, pfc_selfps, pfc_self);
    //  net->FindMakeSelfPrjn(pfc_m, intra_pfcps, intra_pfc);

    // this part in particular doesn't make sense for pfc_o only..
    // critics need up reflect updating!
    net->FindMakePrjn(pvi, pfc_m, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_m, fullprjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_m, fullprjn, lvi_cons);
  }
  else {			// !out_gate
    if(mat_fm_pfc_full)
      net->FindMakePrjn(matrix_m, pfc_m, fullprjn, mfmpfc_cons);
    else
      net->FindMakePrjn(matrix_m, pfc_m, gponetoone, mfmpfc_cons);

    net->FindMakeSelfPrjn(pfc_m, pfc_selfps, pfc_self);
    //  net->FindMakeSelfPrjn(pfc, intra_pfcps, intra_pfc);

    net->FindMakePrjn(pvi, pfc_m, fullprjn, pvi_cons);
    net->FindMakePrjn(lve, pfc_m, fullprjn, lve_cons);
    net->FindMakePrjn(lvi, pfc_m, fullprjn, lvi_cons);
  }

//   if(make_patch) {
//     net->FindMakePrjn(patch, pfc_m, gponetoone, lve_cons);
//   }

  for(i=0;i<input_lays.size;i++) {
    Layer* il = (Layer*)input_lays[i];
    if(pfc_m_new) {
      if(nolrn_pfc)
	net->FindMakePrjn(pfc_m, il, input_pfc, topfc_cons);
      else
	net->FindMakePrjn(pfc_m, il, fullprjn, topfc_cons);
    }
    if(matrix_m_new)
      net->FindMakePrjn(matrix_m, il, fullprjn, matrix_cons);
    if(matrix_o_new)
      net->FindMakePrjn(matrix_o, il, fullprjn, matrixo_cons);
//     if(make_patch && patch_new) {
//       net->FindMakePrjn(patch, il, fullprjn, lve_cons);
//     }
  }
  for(i=0;i<hidden_lays.size;i++) {
    Layer* hl = (Layer*)hidden_lays[i];
    if(out_gate) {
      net->FindMakePrjn(hl, pfc_o, fullprjn, learn_cons);
      net->RemovePrjn(hl, pfc_m); // get rid of any existing ones
    }
    else {
      net->FindMakePrjn(hl, pfc_m, fullprjn, learn_cons);
    }
    if(fm_hid_cons) {
      if(pfc_m_new && !nolrn_pfc)
	net->FindMakePrjn(pfc_m, hl, fullprjn, topfc_cons);
      if(matrix_m_new)
	net->FindMakePrjn(matrix_m, hl, fullprjn, matrix_cons);
      if(matrix_o_new)
	net->FindMakePrjn(matrix_o, hl, fullprjn, matrixo_cons);
//       if(make_patch && patch_new) {
// 	net->FindMakePrjn(patch, hl, fullprjn, lve_cons);
//       }
    }
  }
  if(fm_out_cons) {
    for(i=0;i<output_lays.size;i++) {
      Layer* ol = (Layer*)output_lays[i];
      if(pfc_m_new && !nolrn_pfc)
	net->FindMakePrjn(pfc_m, ol, fullprjn, topfc_cons);
      if(matrix_m_new)
	net->FindMakePrjn(matrix_m, ol, fullprjn, matrix_cons);
      if(matrix_o_new)
	net->FindMakePrjn(matrix_o, ol, fullprjn, matrixo_cons);
//       if(make_patch && patch_new) {
// 	net->FindMakePrjn(patch, ol, fullprjn, lve_cons);
//       }
    }
  }

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  // todo: update these values!

  if(lr_sched) {
    learn_cons->lrs_value = LeabraConSpec::EXT_REW_STAT;
    learn_cons->lrate_sched.EnforceSize(2);
    SchedItem* si = (SchedItem*)learn_cons->lrate_sched.FastEl(0);
    si->start_val = 1.0f;
    si = (SchedItem*)learn_cons->lrate_sched.FastEl(1);
    si->start_ctr = 90;
    si->start_val = .1f;
  }

  // slow learning rate on to pfc cons!
  topfc_cons->SetUnique("lrate", true);
  if(nolrn_pfc) {
    topfc_cons->lrate = 0.0f;
    topfc_cons->SetUnique("rnd", true);
    topfc_cons->rnd.var = 0.0f;
  }
  else {
    topfc_cons->lrate = .001f;
    topfc_cons->SetUnique("rnd", false);
    topfc_cons->rnd.var = 0.25f;
  }
  topfc_cons->SetUnique("lmix", true);
  topfc_cons->lmix.hebb = .001f;
  intra_pfc->SetUnique("wt_scale", true);
  intra_pfc->wt_scale.rel = .1f;

  pfc_self->SetUnique("lrate", true);
  pfc_self->lrate = 0.0f;
  pfc_self->SetUnique("rnd", true);
  pfc_self->rnd.mean = 0.9f;
  pfc_self->rnd.var = 0.0f;
  pfc_self->SetUnique("wt_scale", true);
  pfc_self->wt_scale.rel = .1f;

  matrix_cons->SetUnique("lrate", true);
  matrix_cons->lrate = .01f;

  mfmpfc_cons->SetUnique("wt_scale", true);
  mfmpfc_cons->wt_scale.rel = .2f;
  mfmpfc_cons->SetUnique("lmix", false);

  if(out_gate) {
    matrixo_units->SetUnique("freeze_net", true);
    matrixo_units->freeze_net = false;

    matrixo_units->SetUnique("g_bar", true);
    matrixo_units->g_bar.h = .02f; matrixo_units->g_bar.a = .06f;

    matrixo_cons->SetUnique("learn_rule", true);
    matrixo_cons->learn_rule = MatrixConSpec::OUTPUT_DELTA;
    matrixo_cons->SetUnique("lmix", false);
    matrixo_cons->SetUnique("lrate", true);
    matrixo_cons->lrate = .2f;
    matrixo_cons->SetUnique("rnd", true);
    matrixo_cons->rnd.var = .2f;
    matrixo_cons->SetUnique("wt_sig", true);
    matrixo_cons->wt_sig.gain = 1.0f;
    matrixo_cons->wt_sig.off = 1.0f;

    mofmpfc_cons->SetUnique("wt_scale", true);
    mofmpfc_cons->wt_scale.rel = .2f;
    mofmpfc_cons->SetUnique("lmix", false);

    // snrthalosp->SetUnique("kwta", true);
    // but otherwise just inherit it..

    matrixosp->SetUnique("err_rnd_go", true);
    matrixosp->err_rnd_go.min_cor = 1;
    matrixosp->err_rnd_go.gain = 0.5f;
    matrixosp->SetUnique("avgda_rnd_go", true);
    matrixosp->avgda_rnd_go.on = false;
  }

  matrix_units->g_bar.h = .01f;
  matrix_units->g_bar.a = .03f;
  pfc_units->SetUnique("g_bar", true);
  if(nolrn_pfc)
    pfc_units->g_bar.h = 1.0f;
  else
    pfc_units->g_bar.h = .5f;
  pfc_units->g_bar.a = 2.0f;
  pfc_units->SetUnique("act_reg", true);
  if(nolrn_pfc)
    pfc_units->act_reg.on = false;
  else
    pfc_units->act_reg.on = true;
  pfc_units->SetUnique("dt", true);
  pfc_units->dt.vm = .1f;	// slower is better..  .1 is even better!

  snrthal_units->dt.vm = .1f;
  snrthal_units->act.gain = 20.0f;

  // set projection parameters
  topfc->p_con = .4;
  pfc_selfps->self_con = true;

  // todo: out of date!
  intra_pfcps->def_p_con = .4;
  intra_pfcps->recv_gp_n.y = 1;
  intra_pfcps->recv_gp_group.x = half_stripes;
  intra_pfcps->MakeRectangle(half_stripes, 1, 0, 1);
  intra_pfcps->wrap = false;
  
  matrixsp->bg_type = MatrixLayerSpec::MAINT;
  // set these to fix old projects..
  matrixsp->gp_kwta.k_from = KWTASpec::USE_PCT;
  matrixsp->gp_kwta.pct = .25f;
  matrixsp->compute_i = LeabraLayerSpec::KWTA_INHIB;
  matrixsp->i_kwta_pt = .25f;
  matrixsp->UpdateAfterEdit();

  if(out_gate) {
    matrixosp->SetUnique("bg_type", true);
    matrixosp->bg_type = MatrixLayerSpec::OUTPUT;
    matrixosp->SetUnique("contrast", true);
    matrixosp->contrast.go_p = 1.0f;  matrixosp->contrast.go_n = 1.0f;
    matrixosp->contrast.nogo_p = 1.0f;  matrixosp->contrast.nogo_n = 1.0f;
//     matrixosp->SetUnique("rnd_go", true);
//     matrixosp->rnd_go.nogo_rgo_da = 10f;
  }

//   if(make_patch) {
//     // NOT unique: inherit from lve:
//     patchsp->SetUnique("decay", false);
//     patchsp->SetUnique("kwta", false);
//     patchsp->SetUnique("inhib_group", false);
//     patchsp->SetUnique("compute_i", false);
//     patchsp->SetUnique("i_kwta_pt", false);
//   }


  //////////////////////////////////////////////////////////////////////////////////
  // set positions & geometries

  snc->un_geom.n = 1;
  if(snc_new) { 
    snc->pos.z = 0; snc->pos.y = 3; snc->pos.x = vta->pos.x; 
  }
  lay_set_geom(snc, half_stripes);

  if(pfc_m_new) {
    pfc_m->pos.z = 2; pfc_m->pos.y = 0; pfc_m->pos.x = mx_z2 + 1;
    if(nolrn_pfc && (input_lays.size > 0)) {
      Layer* il = (Layer*)input_lays[0];
      pfc_m->un_geom = il->un_geom;
    }
    else {
      pfc_m->un_geom.n = 30; pfc_m->un_geom.x = 5; pfc_m->un_geom.y = 6;
    }
  }
  lay_set_geom(pfc_m, half_stripes);

  if(matrix_m_new) { 
    matrix_m->pos.z = 1; matrix_m->pos.y = 0; matrix_m->pos.x = mx_z1 + 1; 
    matrix_m->un_geom.n = 28; matrix_m->un_geom.x = 4; matrix_m->un_geom.y = 7;
  }
  lay_set_geom(matrix_m, half_stripes);

  snrthal_m->un_geom.n = 1;
  if(snrthal_m_new) {
    snrthal_m->pos.z = 0; snrthal_m->pos.y = 6; snrthal_m->pos.x = snc->pos.x;
  }
  lay_set_geom(snrthal_m, half_stripes);

  // this is here, to allow it to get act_geom for laying out the pfc and matrix guys!
  SetPFCStripes(net, n_stripes);

  if(out_gate) {
    if(pfc_o_new) {
      pfc_o->pos.z = pfc_m->pos.z; pfc_o->pos.y = pfc_m->pos.y;
      pfc_o->pos.x = pfc_m->pos.x + pfc_m->act_geom.x + 2;
      if(nolrn_pfc && (input_lays.size > 0)) {
	Layer* il = (Layer*)input_lays[0];
	pfc_o->un_geom = il->un_geom;
      }
      else {
	pfc_o->un_geom = pfc_m->un_geom;
      }
    }
    lay_set_geom(pfc_o, half_stripes);

    if(matrix_o_new) { 
      matrix_o->pos.z = matrix_m->pos.z; matrix_o->pos.y = matrix_m->pos.y;
      matrix_o->pos.x = matrix_m->pos.x + matrix_m->act_geom.x + 2; 
      matrix_o->un_geom.n = 28; matrix_o->un_geom.x = 4; matrix_o->un_geom.y = 7;
    }
    lay_set_geom(matrix_o, half_stripes);

    snrthal_o->un_geom.n = 1;
    if(snrthal_o_new) {
      snrthal_o->pos.z = 0; snrthal_o->pos.y = 6;
      snrthal_o->pos.x = snrthal_m->pos.x + snrthal_m->act_geom.x + 2;
    }
    lay_set_geom(snrthal_o, half_stripes);
  }

//   if(make_patch) {
//     if(patch_new) {
//       matrix_m->UpdateAfterEdit();
//       patch->pos.z = 1; patch->pos.y = 0; patch->pos.x = matrix_m->pos.x + matrix_m->act_geom.x + 2; 
//       patch->un_geom.n = 20; patch->un_geom.x = 20; patch->un_geom.y = 1;
//       patch->gp_geom.x = 1; patch->gp_geom.y = n_stripes;
//       patch->UpdateAfterEdit();
//     }
//     lay_set_geom(patch, half_stripes);
//     patch->gp_geom.x = 1; patch->gp_geom.y = n_stripes;
//   }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  SetPFCStripes(net, n_stripes);

  bool ok = pfcmsp->CheckConfig_Layer(pfc_m, true) && matrixsp->CheckConfig_Layer(matrix_m, true)
    && snrthalsp->CheckConfig_Layer(snrthal_m, true) && sncsp->CheckConfig_Layer(snc, true);

  if(ok && out_gate) {
    ok = pfcosp->CheckConfig_Layer(pfc_o, true) && matrixosp->CheckConfig_Layer(matrix_o, true)
      && snrthalosp->CheckConfig_Layer(snrthal_o, true);
  }

//   if(ok && make_patch) ok = patchsp->CheckConfig_Layer(patch, true);

  if(!ok) {
    msg =
      "BG/PFC: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg = 
    "BG/PFC configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Choice(msg,"Ok");

  for(int j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = pdpMisc::FindSelectEdit(proj);
  if(edit != NULL) {
    pfc_units->SelectForEditNm("g_bar", edit, "pfc");
    pfcmsp->SelectForEditNm("gate", edit, "pfc_m");
    pfcmsp->SelectForEditNm("act_reg", edit, "pfc_m");
    matrixsp->SelectForEditNm("matrix", edit, "matrix");
    matrixsp->SelectForEditNm("contrast", edit, "matrix");
    matrixsp->SelectForEditNm("rnd_go", edit, "matrix");
    matrixsp->SelectForEditNm("err_rnd_go", edit, "matrix");
    matrixsp->SelectForEditNm("avgda_rnd_go", edit, "matrix");
    matrix_units->SelectForEditNm("g_bar", edit, "matrix");
//     matrix_cons->SelectForEditNm("lrate", edit, "matrix");
    matrix_cons->SelectForEditNm("lmix", edit, "matrix");
    mfmpfc_cons->SelectForEditNm("wt_scale", edit, "mtx_fm_pfc");
//     sncsp->SelectForEditNm("snc", edit, "snc");
    snrthalsp->SelectForEditNm("kwta", edit, "snr_thal");
//       snrthal_units->SelectForEditNm("g_bar", edit, "snr_thal");
//       snrthal_units->SelectForEditNm("dt", edit, "snr_thal");
    if(out_gate) {
      matrixo_units->SelectForEditNm("g_bar", edit, "mtx_out");
      matrixo_cons->SelectForEditNm("lrate", edit, "mtx_out");
      matrixo_cons->SelectForEditNm("rnd", edit, "mtx_out");
      matrixo_cons->SelectForEditNm("wt_sig", edit, "mtx_out");
      matrixosp->SelectForEditNm("contrast", edit, "mtx_out");
      matrixosp->SelectForEditNm("err_rnd_go", edit, "mtx_out");
      matrixosp->SelectForEditNm("avgda_rnd_go", edit, "mtx_out");
      pfcosp->SelectForEditNm("out_gate", edit, "pfc_out");
      snrthalosp->SelectForEditNm("kwta", edit, "snr_thal_out");
    }    
  }
}

