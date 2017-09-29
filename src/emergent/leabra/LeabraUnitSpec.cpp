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

#include "LeabraUnitSpec.h"
#include <LeabraNetwork>
#include <LeabraBiasSpec>
#include <DeepCtxtConSpec>
#include <SendDeepRawConSpec>
#include <SendDeepModConSpec>
#include <taProject>
#include <taMath_double>
#include <DataTable>
#include <CircBufferIndex>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraActFunSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActMiscSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeFunSpec);
TA_BASEFUNS_CTORS_DEFN(SpikeMiscSpec);
TA_BASEFUNS_CTORS_DEFN(OptThreshSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraInitSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraDtSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraActAvgSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAvgLSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraAvgL2Spec);
TA_BASEFUNS_CTORS_DEFN(LeabraChannels);
TA_BASEFUNS_CTORS_DEFN(ActAdaptSpec);
TA_BASEFUNS_CTORS_DEFN(ShortPlastSpec);
TA_BASEFUNS_CTORS_DEFN(SynDelaySpec);
TA_BASEFUNS_CTORS_DEFN(DeepSpec);
TA_BASEFUNS_CTORS_DEFN(TRCSpec);
TA_BASEFUNS_CTORS_DEFN(DaModSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraNoiseSpec);

TA_BASEFUNS_CTORS_DEFN(LeabraUnitSpec_core);
TA_BASEFUNS_CTORS_DEFN(LeabraUnitSpec);

SMARTREF_OF_CPP(LeabraUnitSpec);


/* TODO:
void LeabraAvgLSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  taVersion v804(8, 0, 4);
  if(taMisc::is_loading && taMisc::loading_version < v804) {
    if(gain == 1.5f || gain == 2.5f) {
      gain = 2.5f;
    }
    else {
      TestWarning(true, "UAE",
                  "avg_l computation has been updated to a running-average formulation that works much better overall -- previous max value was a nonstandard:", String(gain),
                  "(default was 1.5 before, 2.5 now) -- updating to 2.5 now but you should experiment and find the best current value for this param!");
      gain = 2.5f;
    }
    if(lrn_max == 0.05f && lrn_min == 0.005f) {
      lrn_max = 0.5f;
      lrn_min = 0.0001f;
    }
    else {
      TestWarning(true, "UAE",
                  "avg_l computation has been updated to a running-average formulation that works much better overall -- previous lrn_max and/or lrn_min value(s) were nonstandard:",
                  String(lrn_max), String(lrn_min),
                  "new defaults are 0.5, 0.0001 -- recommend trying them!");
    }
  }
}
*/

void LeabraUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraUnit;
  bias_spec.SetBaseType(&TA_LeabraBiasSpec);

  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  Defaults_init();
}

void LeabraUnitSpec::Defaults_init() {
}

void LeabraUnitSpec::InitLinks() {
  //  bias_spec.type = &TA_LeabraBiasSpec;
  inherited::InitLinks();
  InitLinks_taAuto(&TA_LeabraUnitSpec);
  children.SetBaseType(&TA_LeabraUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void LeabraUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(noise_type.type == LeabraNoiseSpec::VM_NOISE && act_fun != SPIKE) {
    taMisc::Warning("Cannot use noise_type = VM_NOISE with rate-code (non-spiking) activation function -- changing noise_type to NETIN_NOISE");
    noise_type.type = LeabraNoiseSpec::NETIN_NOISE;
  }

  if(spike_misc.ex)
    spike_misc.eff_spk_thr = spike_misc.spk_thr;
  else
    spike_misc.eff_spk_thr = act.thr;

  act_misc.UpdateAfterEdit_NoGui();
  spike.UpdateAfterEdit_NoGui();
  spike_misc.UpdateAfterEdit_NoGui();
  clamp_range.UpdateAfterEdit_NoGui();
  vm_range.UpdateAfterEdit_NoGui();
  dt.UpdateAfterEdit_NoGui();
  act_avg.UpdateAfterEdit_NoGui();
  adapt.UpdateAfterEdit_NoGui();
  stp.UpdateAfterEdit_NoGui();
  deep.UpdateAfterEdit_NoGui();

  // if(deep.on && deep_raw_qtr == QNULL) { // doesn't make sense to not have any deep raw..
  //   deep_raw_qtr = Q4;
  // }
}

void LeabraUnitSpec::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);
  if(net) {
    // if(dt.integ != 1000.0f * net->times.time_inc) {
    //   taMisc::Warning("unit time integration constant dt.integ of:", (String)dt.integ,
    //                   "does not match network phases.time_inc increment of:",
    //                   (String)net->times.time_inc, "time_inc should be 0.001 * dt.integ");
    // }
  }
}

bool LeabraUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;

  return rval;
}

void LeabraUnitSpec::CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl, 
                                   FunLookup& noise_fl) {
  // first create the gaussian noise convolver
  nxx1_fl.x_range.min = -.1f;
  nxx1_fl.x_range.max = 1.0f;
  nxx1_fl.res = .001f;       // needs same fine res to get the noise transitions
  nxx1_fl.UpdateAfterEdit_NoGui();

  float ns_rng = 3.0f * act_spec.nvar;       // range factor based on noise level -- 3 sd
  ns_rng = MAX(ns_rng, nxx1_fl.res);
  nxx1_fl.x_range.min = -ns_rng;

  noise_fl.x_range.min = -ns_rng;
  noise_fl.x_range.max = ns_rng;
  noise_fl.res = nxx1_fl.res;
  noise_fl.UpdateAfterEdit_NoGui();

  noise_fl.AllocForRange();
  int i;
  float eff_nvar = fmaxf(act_spec.nvar, 1.0e-6f); // just too lazy to do proper conditional for 0..
  float var = eff_nvar * eff_nvar;
  for(i=0; i < noise_fl.size; i++) {
    float x = noise_fl.Xval(i);
    noise_fl[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_fl.size; i++)
    sum += noise_fl[i];
  for(i=0; i < noise_fl.size; i++)
    noise_fl[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fl.x_range.min + noise_fl.x_range.min;
  fun.x_range.max = nxx1_fl.x_range.max + noise_fl.x_range.max;
  fun.res = nxx1_fl.res;
  fun.UpdateAfterEdit_NoGui();
  fun.AllocForRange();

  for(i=0; i<fun.size; i++) {
    float x = fun.Xval(i);
    float val = 0.0f;
    if(x > 0.0f)
      val = (act_spec.gain * x) / ((act_spec.gain * x) + 1.0f);
    fun[i] = val;
  }

  nxx1_fl.Convolve(fun, noise_fl); // does alloc
}

void LeabraUnitSpec::Init_Vars(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Vars(u, net, thr_no);

  Init_Vars_impl(u);
  
  CircBufferIndex::Reset(u->spike_e_st, u->spike_e_len);
  CircBufferIndex::Reset(u->spike_i_st, u->spike_i_len);
  
  // act_buf = NULL;
  // spike_e_buf = NULL;
  // spike_i_buf = NULL;
}

void LeabraUnitSpec::Init_Weights(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;

  if(bias_spec) {
    bias_spec->B_Init_Weights(u, net, thr_no);
  }

  Init_Weights_impl(u);

  Init_ActAvg(u, net, thr_no);  // likely redundant

  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->Init_Weights_rcgp(recv_gp, net, thr_no);
  }
}

void LeabraUnitSpec::LoadBiasWtVal(float bwt, UnitVars* uv, Network* net) {
  LeabraUnitVars* u = (LeabraUnitVars*)uv;
  u->bias_wt = bwt;
  u->bias_fwt = bwt;
  u->bias_swt = bwt;
}

void LeabraUnitSpec::Init_ActAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Init_ActAvg_impl(u);
}

void LeabraUnitSpec::Init_Netins(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Init_Netins_impl(u);

  Init_Netins_cons(u, net, thr_no);
}

void LeabraUnitSpec::Init_Netins_cons(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    recv_gp->net = 0.0f;
    recv_gp->net_raw = 0.0f;
  }
}

void LeabraUnitSpec::Init_Acts(UnitVars* ru, Network* rnet, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  LeabraNetwork* net = (LeabraNetwork*)rnet;
  inherited::Init_Acts(u, net, thr_no);

  Init_Acts_impl(u);
  Init_Netins_cons(u, net, thr_no);

  CircBufferIndex::Reset(u->spike_e_st, u->spike_e_len);
  CircBufferIndex::Reset(u->spike_i_st, u->spike_i_len);
  
  // if(syn_delay.on) {
  //   u->act_buf->Reset();
  // }
}


void LeabraUnitSpec::DecayState(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                                float decay) {
  DecayState_impl(u, decay);
  
  if(decay == 1.0f) {
    if(act_fun == SPIKE) {
      CircBufferIndex::Reset(u->spike_e_st, u->spike_e_len);
      CircBufferIndex::Reset(u->spike_i_st, u->spike_i_len);
    }

    // if(syn_delay.on) {
    //   u->act_buf->Reset();
    // }
  }
}

void LeabraUnitSpec::ResetSynTR(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  ResetSynTR_impl(u);
}


///////////////////////////////////////////////////////////////////////
//      TrialInit functions

void LeabraUnitSpec::Trial_Init_Specs(LeabraNetwork* net) {
  if(act_fun == SPIKE) {
    net->net_misc.spike = true;
  }
  else {
    TestWarning(net->net_misc.spike, "Trial_Init_Specs",
                "detected a mix of SPIKE and NOISY_XX1 activation functions -- due to code optimizations, must all be either one or the other!");
  }
  if(deep.on) {
    net->deep.on = true;
    net->deep.raw_qtr = (LeabraNetDeep::Quarters)(net->deep.raw_qtr | deep_raw_qtr);
  }
  if(bias_spec) {
    ((LeabraConSpec*)bias_spec.SPtr())->Trial_Init_Specs(net);
  }
}

void LeabraUnitSpec::Trial_Init_Unit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Trial_STP_TrialBinary_Updt(u, net, thr_no); //do this b4 decay, because using act_q3 from previous trial
  Trial_Init_PrvVals(u, net, thr_no);   // do this b4 decay, so vals are intact
  Trial_Init_SRAvg(u, net, thr_no);     // do this b4 decay..
  Trial_DecayState(u, net, thr_no);
  Trial_NoiseInit(u, net, thr_no);
}

void LeabraUnitSpec::Trial_Init_PrvVals(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Trial_Init_PrvVals_impl(u);
}

void LeabraUnitSpec::Trial_Init_SRAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  Trial_Init_SRAvg_impl(u, lay->acts_p.avg, lay->cos_diff_avg_lrn,
                        ((lay->layer_type != Layer::HIDDEN) || deep.IsTRC()));
}

void LeabraUnitSpec::Trial_DecayState(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  Trial_DecayState_impl(u, ls->decay.trial);
  Init_Netins_cons(u, net, thr_no);
}

// todo: need this on cuda side..
void LeabraUnitSpec::Trial_NoiseInit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(noise_type.type != LeabraNoiseSpec::NO_NOISE && noise_type.trial_fixed &&
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen(thr_no);
  }
}

void LeabraUnitSpec::Trial_STP_TrialBinary_Updt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!stp.on || stp.algorithm != ShortPlastSpec::TRIAL_BINARY) return;

  if (u->syn_tr > 0.0f) { // if the unit isn't currently depressed
    if (u->act_q3 > stp.thresh) {
      u->syn_kre += 1.0f;       // note: ++ not generally defined for floats
    }
    else {
      u->syn_kre = 0.0f;
    }
    if (u->syn_kre >= stp.n_trials) {
      u->syn_tr  = 0.0f;
    }
  }
  else { // this unit is currently depressed
    bool recover = Random::BoolProb(stp.rec_prob, thr_no);
    if (recover) {
      u->syn_tr = 1.0f;
      u->syn_kre = 0.0f;
    }
  }
}

///////////////////////////////////////////////////////////////////////
//      QuarterInit functions

void LeabraUnitSpec::Quarter_Init_Unit(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Quarter_Init_TargFlags(u, net, thr_no);
  Quarter_Init_PrvVals(u, net, thr_no);
  Compute_NetinScale(u, net, thr_no);
  Compute_HardClamp(u, net, thr_no);
}

void LeabraUnitSpec::Quarter_Init_TargFlags(LeabraUnitVars* u, LeabraNetwork* net,
                                            int thr_no) {
  Quarter_Init_TargFlags_impl(u, (net->phase == LeabraNetwork::MINUS_PHASE));
}

void LeabraUnitSpec::Quarter_Init_PrvVals(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no) {
  Quarter_Init_PrvVals_impl(u, net->quarter);
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // this is all receiver-based and done only at beginning of each quarter
  bool plus_phase = (net->phase == LeabraNetwork::PLUS_PHASE);
  Unit* un = u->Un(net, thr_no);
  float net_scale = 0.0f;
  float inhib_net_scale = 0.0f;
  float deep_raw_scale = 0.0f;
  float deep_mod_scale = 0.0f;
  
  // important: count all projections so it is uniform across all units
  // in the layer!  if a unit does not have a connection in a given projection,
  // then it counts as a zero, but it counts in overall normalization!
  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    cs->Compute_NetinScale(recv_gp, from, plus_phase); // sets recv_gp->scale_eff
    float rel_scale = cs->wt_scale.rel;
    
    if(cs->inhib) {
      inhib_net_scale += rel_scale;
    }
    else if(cs->IsDeepRawCon()) {
      deep_raw_scale += rel_scale;
    }
    else if(cs->IsDeepModCon()) {
      deep_mod_scale += rel_scale;
    }
    else {                      // DeepCtxtCon gets included in overall netin!
      net_scale += rel_scale;
    }
  }
  
  // add the bias weight into the netinput, scaled by 1/n
  if(bias_spec) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(un->n_recv_cons > 0)
      u->bias_scale /= (float)un->n_recv_cons; // one over n scaling for bias!
  }

  // now renormalize, each one separately..
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->prjn->NotActive()) continue; // key!! just check for prjn, not con group!
    Projection* prjn = (Projection*) recv_gp->prjn;
    LeabraLayer* from = (LeabraLayer*) prjn->from.ptr();
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(cs->inhib) {
      if(inhib_net_scale > 0.0f)
        recv_gp->scale_eff /= inhib_net_scale;
    }
    else if(cs->IsDeepRawCon()) {
      if(deep_raw_scale > 0.0f)
        recv_gp->scale_eff /= deep_raw_scale;
    }
    else if(cs->IsDeepModCon()) {
      if(deep_mod_scale > 0.0f)
        recv_gp->scale_eff /= deep_mod_scale;
    }
    else {
      if(net_scale > 0.0f)
        recv_gp->scale_eff /= net_scale;
    }
  }
}

void LeabraUnitSpec::Send_DeepCtxtNetin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  float act_ts = u->deep_raw;
  if(act_ts > opt_thresh.send) {
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      // LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      // if(tol->hard_clamped)      continue;
      LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
      if(cs->IsDeepCtxtCon()) {
        DeepCtxtConSpec* sp = (DeepCtxtConSpec*)cs;
        sp->Send_DeepCtxtNetin(send_gp, net, thr_no, act_ts);
      }
    }
  }
}

void LeabraUnitSpec::Compute_DeepCtxt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;
  // note: this is OK to only integrate selectively b/c not using delta-based netin

  int flat_idx = u->flat_idx;
  int nt = net->n_thrs_built;
  float net_sum = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendDeepRawNetTmp(j)[flat_idx];
    net_sum += ndval;
  }
  if(deep.tick_updt >= 0) {
    if(net->tick == deep.tick_updt) {
      u->deep_ctxt = deep.ctxt_prv * u->deep_ctxt + deep.ctxt_new * net_sum;
    }
    else {
      u->deep_ctxt = deep.else_prv * u->deep_ctxt + deep.else_new * net_sum;
    }
  }
  else {
    u->deep_ctxt = deep.ctxt_prv * u->deep_ctxt + deep.ctxt_new * net_sum;
  }
}

void LeabraUnitSpec::Compute_DeepStateUpdt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_DeepStateUpdt_impl(u, net->quarter);
}

void LeabraUnitSpec::Init_InputData(UnitVars* uv, Network* net, int thr_no) {
  LeabraUnitVars* u = (LeabraUnitVars*)uv;
  Init_InputData_impl(u);
}

void LeabraUnitSpec::ApplyInputData_post(LeabraUnitVars* u) {
  ApplyInputData_post_impl(u);
}

void LeabraUnitSpec::Compute_HardClamp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(!(ls->clamp.hard && lay->HasExtFlag(UnitVars::EXT))) {
    return;
  }

  Compute_HardClamp_impl(u, net->cycle, true); // true = clip

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
  // u->AddToActBuf(syn_delay);
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnitVars* u, LeabraNetwork* net,
                                             int thr_no) {
  if(!u->HasExtFlag(UnitVars::EXT))
    return;
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
  if(!(ls->clamp.hard && lay->HasExtFlag(UnitVars::EXT))) {
    return;
  }
  
  Compute_HardClamp_impl(u, net->cycle, false); // false = no clip

  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);
  // u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::ExtToComp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  ExtToComp_impl(u);
}

void LeabraUnitSpec::TargExtToComp(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  TargExtToComp_impl(u);
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 1: netinput

void LeabraUnitSpec::Send_NetinDelta(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->n_thrs_built == 1) {
    net->send_pct_tot++;        // only safe for non-thread case
  }
  float act_ts = u->act;
  // if(syn_delay.on) { // todo!
  //   if(!u->act_buf)
  //     Init_ActBuff(u);
  //   act_ts = u->act_buf->CircSafeEl(0); // get first logical element..
  // }

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->act_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      if(net->n_thrs_built == 1) {
        net->send_pct_n++;
      }
      const int nsg = u->NSendConGps(net, thr_no); 
      for(int g=0; g< nsg; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
        if(send_gp->NotActive()) continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsDeepModCon()) {
          SendDeepModConSpec* sp = (SendDeepModConSpec*)cs;
          sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
        }
        if(!cs->DoesStdNetin()) continue;
        LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        if(tol->hard_clamped)      continue;
        if(cs->wt_scale.no_plus_net && net->quarter == 3) {
          // netin typically reset at start of plus phase, so we need to send minus phase
          // activation for first cycle and then stop sending after that
          const int cyc_per_qtr = net->times.quarter;
          int qtr_cyc = net->cycle;
          if(net->cycle > cyc_per_qtr)             // just in case cycle being reset
            qtr_cyc -= net->quarter * cyc_per_qtr; // quarters into this cyc
          if(qtr_cyc > 0) continue;
          float actm_delta = u->act_m - u->act_sent;
          ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                               actm_delta);
          continue;
        }
        ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                             act_delta);
      }
      u->act_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    if(net->n_thrs_built == 1) {
      net->send_pct_n++;
    }
    float act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
      if(cs->IsDeepModCon()) {
        SendDeepModConSpec* sp = (SendDeepModConSpec*)cs;
        sp->Send_DeepModNetDelta(send_gp, net, thr_no, act_delta);
      }
      if(!cs->DoesStdNetin()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->hard_clamped)        continue;
      if(cs->wt_scale.no_plus_net && net->quarter == 3) {
        // netin typically reset at start of plus phase, so we need to send minus phase
        // activation for first cycle and then stop sending after that
        const int cyc_per_qtr = net->times.quarter;
        int qtr_cyc = net->cycle;
        if(net->cycle > cyc_per_qtr)
          qtr_cyc -= net->quarter * cyc_per_qtr; // quarters into this cyc
        if(qtr_cyc > 0) continue;
        if(u->act_m > opt_thresh.send) continue; // not actually off!
      }
      ((LeabraConSpec*)send_gp->con_spec)->Send_NetinDelta(send_gp, net, thr_no,
                                                           act_delta);
    }
    u->act_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LeabraUnitSpec::Compute_NetinRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // this integrates from SendDelta into net_raw and gi_syn
  int nt = net->n_thrs_built;
  const int flat_idx = u->flat_idx;
  float net_delta = 0.0f;
  float gi_delta = 0.0f;
  if(net->NetinPerPrjn()) {
    const int nrg = u->NRecvConGps(net, thr_no); 
    for(int g=0; g< nrg; g++) {
      LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
      if(recv_gp->NotActive()) continue;
      float g_net_delta = 0.0f;
      for(int j=0;j<nt;j++) {
        float& ndval = net->ThrSendNetinTmpPerPrjn(j, g)[flat_idx]; 
	g_net_delta += ndval;
      }
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      recv_gp->net_raw += g_net_delta; // note: direct assignment to raw, no time integ
      if(cs->inhib) {
        gi_delta += g_net_delta;
      }
      else {
        net_delta += g_net_delta;
      }
    }
    u->net_raw += net_delta;
  }
  else {
    for(int j=0;j<nt;j++) {
      float& ndval = net->ThrSendNetinTmp(j)[flat_idx];
      net_delta += ndval;
    }
    u->net_raw += net_delta;
  }

  if(net->net_misc.inhib_cons) {
    u->gi_raw += gi_delta;
    if(act_fun == SPIKE) {
      u->gi_syn = fmaxf(u->gi_syn, 0.0f);
      Compute_NetinInteg_Spike_i(u, net, thr_no);
    }
    else {
      u->gi_syn += dt.integ * dt.net_dt * (u->gi_raw - u->gi_syn);
      u->gi_syn = fmaxf(u->gi_syn, 0.0f); // negative netin doesn't make any sense
    }
  }
  else {
    // clear so automatic inhibition can add to these values!
    // this is an unnec memory write!
    // u->gi_syn = 0.0f;
    // u->gi_raw = 0.0f;
  }
}

void LeabraUnitSpec::DeepModNetin_Integ(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  int flat_idx = u->flat_idx;
  int nt = net->n_thrs_built;
  float net_delta = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendDeepModNetTmp(j)[flat_idx];
    net_delta += ndval;
  }
  u->deep_mod_net += net_delta;
}

void LeabraUnitSpec::Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->deep.mod_net) {       // if anyone's doing it, we need to integrate!
    DeepModNetin_Integ(u, net, thr_no);
  }
  
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(lay->hard_clamped) {
    return;
  }

  Compute_NetinRaw(u, net, thr_no);
  // u->net_raw and u->gi_syn now have proper values integrated from deltas

  float net_syn = act_misc.net_gain * u->net_raw;
  float net_ex = 0.0f;
  if(deep.IsTRC() && Quarter_DeepRawNow(net->quarter)) {
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    if(lay->am_deep_raw_net.max > 0.1f) { // have to get some input to clamp
      if(trc.p_only_m) {
        LeabraUnGpData* ugd = lay->UnGpDataUn(un);
        if(ugd->acts_prvq.max > 0.1f) {
          // only activate if we got prior and current activation
          net_syn = trc.TRCClampNet(u->deep_raw_net, net_syn); // u->net_prv_q); 
        }
        else {
          net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
        }
      }
      else {                       // always do it
        net_syn = trc.TRCClampNet(u->deep_raw_net, net_syn); // u->net_prv_q); 
      }
    }
    else {
      net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);
    }
  }
  else {
    net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);  // this could modify net_syn if it wants..
  }
  
  float net_tot = net_syn + net_ex;

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = fmaxf(net_tot, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u, net, thr_no);
  }
  else {
    u->net += dt.integ * dt.net_dt * (net_tot - u->net);
    if(u->net < 0.0f) u->net = 0.0f; // negative netin doesn't make any sense
  }

  // first place noise is required -- generate here!
  if(noise_type.type != LeabraNoiseSpec::NO_NOISE && !noise_type.trial_fixed &&
     (noise.type != Random::NONE)) {
    u->noise = noise.Gen(thr_no);
  }
  
  // add after all the other stuff is done..
  if((noise_type.type == LeabraNoiseSpec::NETIN_NOISE) && (noise.type != Random::NONE)) {
    u->net += u->noise;
  }
  else if((noise_type.type == LeabraNoiseSpec::NET_MULT_NOISE) && (noise.type != Random::NONE)) {
    u->net *= u->noise;
  }
}

float LeabraUnitSpec::Compute_DaModNetin(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no, float& net_syn) {
  return Compute_DaModNetin_impl(u, net_syn, (net->phase == LeabraNetwork::PLUS_PHASE));
}

float LeabraUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  float net_ex = init.netin;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitVars::EXT)) {
    if(ls->clamp.avg)
      net_syn = ls->clamp.ClampAvgNetin(u->ext, net_syn);
    else
      net_ex += u->ext * ls->clamp.gain;
  }
  if(deep.ApplyDeepCtxt()) {
    net_ex += u->deep_ctxt;
  }
  if(da_mod.on) {
    net_ex += Compute_DaModNetin(u, net, thr_no, net_syn);
  }
  return net_ex;
}


void LeabraUnitSpec::Compute_NetinInteg_Spike_e(LeabraUnitVars* u, LeabraNetwork* net,
                                                int thr_no) {
  // netin gets added at the end of the spike_buf -- 0 time is the end
  CircBufferIndex::CircAddShift_float
    (u->net, u->spike_e_buf, u->spike_e_st, u->spike_e_len, spike.window);
  int mx = u->spike_e_len;
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      sum += CircBufferIndex::CircEl_float(t, u->spike_e_buf, u->spike_e_st, spike.window);
    }
    sum /= (float)spike.window; // normalize over window -- todo: what if it is smaller!?
    u->net += dt.integ * (spike.gg_decay * sum - (u->net * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = CircBufferIndex::CircEl_float(t, u->spike_e_buf, u->spike_e_st,
                                                  spike.window);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    // from compute_netinavg
    u->net += dt.integ * dt.net_dt * (sum - u->net);
  }
  u->net = fmaxf(u->net, 0.0f); // negative netin doesn't make any sense
}

void LeabraUnitSpec::Compute_NetinInteg_Spike_i(LeabraUnitVars* u, LeabraNetwork* net,
                                                int thr_no) {
  // netin gets added at the end of the spike_i_buf -- 0 time is the end
  CircBufferIndex::CircAddShift_float
    (u->gi_raw, u->spike_i_buf, u->spike_i_st, u->spike_i_len, spike.window);
  int mx = u->spike_i_len;
  float sum = 0.0f;
  if(spike.rise == 0.0f && spike.decay > 0.0f) {
    // optimized fast recursive exp decay: note: does NOT use dt.net_dt
    for(int t=0;t<mx;t++) {
      sum += CircBufferIndex::CircEl_float(t, u->spike_i_buf, u->spike_i_st, spike.window);
    }
    sum /= (float)spike.window; // normalize over window
    u->gi_syn += dt.integ * (spike.gg_decay * sum - (u->gi_syn * spike.oneo_decay));
  }
  else {
    for(int t=0;t<mx;t++) {
      float spkin = CircBufferIndex::CircEl_float(t, u->spike_i_buf, u->spike_i_st,
                                                  spike.window);
      if(spkin > 0.0f) {
        sum += spkin * spike.ComputeAlpha(mx-t-1);
      }
    }
    u->gi_syn += dt.integ * dt.net_dt * (sum - u->gi_syn);
  }
  u->gi_syn = fmaxf(u->gi_syn, 0.0f); // negative netin doesn't make any sense
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: activation -- rate code

void LeabraUnitSpec::Compute_ApplyInhib
(LeabraUnitVars* u, LeabraNetwork* net, int thr_no, LeabraLayer* lay,
 LeabraLayerSpec* lspec, LeabraInhib* thr, float ival) {
  
  Compute_SelfInhib_impl(u, lspec->inhib_misc.self_fb, lspec->inhib_misc.self_dt);
  float gi_ex = 0.0f;
  if(lspec->del_inhib.on) {
    gi_ex = lspec->del_inhib.prv_trl * u->net_prv_trl + 
      lspec->del_inhib.prv_q * u->net_prv_q;
  }
  Compute_ApplyInhib_impl(u, gi_ex, ival, lay->adapt_gi);
}

void LeabraUnitSpec::SaveGatingAct(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->cycle == net->times.thal_gate_cycle+1) { // happened last trial
    u->act_g = GetRecAct(u);
  }
}

void LeabraUnitSpec::Compute_DeepMod(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  Compute_DeepMod_impl(u, lay->am_deep_mod_net.max);
}

void LeabraUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  SaveGatingAct(u, net, thr_no);
  
  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

  if(deep.on) {
    Compute_DeepMod(u, net, thr_no);
  }

  if((net->cycle >= 0) && lay->hard_clamped) {
    // Compute_HardClamp happens before deep_mod is available due to timing of updates
    if(deep.ApplyDeepMod() && net->cycle == 0) {
      // sync this with Compute_HardClamp:
      Compute_DeepModClampAct_impl(u);
    }
    return; // don't re-compute
  }

  // first, apply inhibition 
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraInhib* thr = ((LeabraUnitSpec*)u->unit_spec)->GetInhib(un);
  if(thr) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    Compute_ApplyInhib(u, net, thr_no, lay, ls, thr, thr->i_val.g_i);
  }

  Compute_Vm(u, net, thr_no);
  Compute_ActFun_Rate(u, net, thr_no);
  Compute_SelfReg_Cycle(u, net, thr_no);

  // u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::Compute_ActFun_Rate(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no) {
  Compute_ActFun_Rate_impl(u, net->cycle, net->tot_cycle);
}

void LeabraUnitSpec::Compute_RateCodeSpike(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  Compute_RateCodeSpike_impl(u, net->tot_cycle, net->times.time_inc);
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: activation -- spiking

void LeabraUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  SaveGatingAct(u, net, thr_no);
  // if(syn_delay.on && !u->act_buf) Init_ActBuff(u);

  if(deep.on) {
    Compute_DeepMod(u, net, thr_no);
  }

  if((net->cycle >= 0) && lay->hard_clamped) {
    Compute_ClampSpike(u, net, thr_no, u->ext * spike_misc.clamp_max_p);
      // u->AddToActBuf(syn_delay);
    return; // don't re-compute
  }

  // first, apply inhibition 
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraInhib* thr = ((LeabraUnitSpec*)u->unit_spec)->GetInhib(un);
  if(thr) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    Compute_ApplyInhib(u, net, thr_no, lay, ls, thr, thr->i_val.g_i);
  }

  Compute_Vm(u, net, thr_no);
  Compute_ActFun_Spike(u, net, thr_no);
  Compute_SelfReg_Cycle(u, net, thr_no);

  // u->AddToActBuf(syn_delay);
}

void LeabraUnitSpec::Compute_ActFun_Spike(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no) {
  Compute_ActFun_Spike_impl(u, net->tot_cycle, net->cycle);
}

void LeabraUnitSpec::Compute_ClampSpike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                                        float spike_p) {
  bool fire_now = false;
  switch(spike_misc.clamp_type) {
  case SpikeMiscSpec::POISSON:
    if(Random::Poisson(spike_p, thr_no) > 0.0f) fire_now = true;
    break;
  case SpikeMiscSpec::UNIFORM:
    fire_now = Random::BoolProb(spike_p, thr_no);
    break;
  case SpikeMiscSpec::REGULAR: {
    if(spike_p > 0.0f) {
      int cyc_int = (int)((1.0f / spike_p) + 0.5f);
      fire_now = (net->cycle % cyc_int == 0);
    }
    break;
  }
  case SpikeMiscSpec::CLAMPED:
    return;                     // do nothing further
  }
  if(fire_now) {
    u->v_m = spike_misc.eff_spk_thr + 0.1f; // make it fire
  }
  else {
    u->v_m = e_rev.l;           // make it not fire
  }

  Compute_ActFun_Spike(u, net, thr_no); // then do normal spiking computation
}

///////////////////////////////////////////////////////////////////////
//      Cycle Step 3.2: membrane potential

void LeabraUnitSpec::Compute_Vm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Vm_impl(u, net->tot_cycle, net->cycle);
}

////////////////////////////////////////////////////////////////////
//              Self reg / adapt / depress

void LeabraUnitSpec::Compute_ActAdapt_Cycle(LeabraUnitVars* u, LeabraNetwork* net,
                                            int thr_no) {
  Compute_ActAdapt_Cycle_impl(u);
}

void LeabraUnitSpec::Compute_ShortPlast_Cycle(LeabraUnitVars* u, LeabraNetwork* net,
                                              int thr_no) {
  Compute_ShortPlast_Cycle_impl(u);
}

void LeabraUnitSpec::Compute_SelfReg_Cycle(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  Compute_ActAdapt_Cycle(u, net, thr_no);
  Compute_ShortPlast_Cycle(u, net, thr_no);
}

void LeabraUnitSpec::Compute_Act_Post(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_SRAvg(u, net, thr_no);
  Compute_Margin(u, net, thr_no);
}

void LeabraUnitSpec::Compute_SRAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_SRAvg_impl(u);
}

void LeabraUnitSpec::Compute_Margin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  Compute_Margin_impl(u, lay->margin_low_thr, lay->margin_med_thr, lay->margin_hi_thr);
}


///////////////////////////////////////////////////////////////////////
//      Cycle Stats


///////////////////////////////////////////////////////////////////////
//      Deep layer updating

void LeabraUnitSpec::Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  // must use act_raw to compute deep_raw because deep_raw is then the input to deep_norm
  // -- if we use act which is already modulated by deep_norm then we get a compounding
  // effect which doesn't work well at all in practice -- does not allow for dynamic
  // deep_mod updating -- just gets stuck in its own positive feedback cycle.

  // todo: revisit this!!
  
  float thr_cmp = lay->acts_raw.avg +
    deep.raw_thr_rel * (lay->acts_raw.max - lay->acts_raw.avg);
  thr_cmp = fmaxf(thr_cmp, deep.raw_thr_abs);
  float draw = 0.0f;
  if(u->act_raw >= thr_cmp) {
    draw = u->act_raw;
  }
  u->deep_raw = draw;
}

void LeabraUnitSpec::Send_DeepRawNetin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  float act_ts = u->deep_raw; // note: no delay for deep

  if(act_ts > opt_thresh.send) {
    float act_delta = act_ts - u->deep_raw_sent;
    if(fabsf(act_delta) > opt_thresh.delta) {
      const int nsg = u->NSendConGps(net, thr_no); 
      for(int g=0; g< nsg; g++) {
        LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
        if(send_gp->NotActive()) continue;
        // LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
        // if(tol->hard_clamped)      continue;
        LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
        if(cs->IsDeepRawCon()) {
          SendDeepRawConSpec* sp = (SendDeepRawConSpec*)cs;
          sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
        }
      }
      u->deep_raw_sent = act_ts;     // cache the last sent value
    }
  }
  else if(u->deep_raw_sent > opt_thresh.send) {
    float act_delta = - u->deep_raw_sent; // un-send the last above-threshold activation to get back to 0
    const int nsg = u->NSendConGps(net, thr_no); 
    for(int g=0; g< nsg; g++) {
      LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
      if(send_gp->NotActive()) continue;
      // LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      // if(tol->hard_clamped)        continue;
      LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
      if(cs->IsDeepRawCon()) {
        SendDeepRawConSpec* sp = (SendDeepRawConSpec*)cs;
        sp->Send_DeepRawNetDelta(send_gp, net, thr_no, act_delta);
      }
    }
    u->deep_raw_sent = 0.0f;         // now it effectively sent a 0..
  }
}

void LeabraUnitSpec::DeepRawNetin_Integ(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // note: no tests here -- ALWAYS run this (network already filters on general need)
  // so that delta can always be tracked!
  int flat_idx = u->flat_idx;
  int nt = net->n_thrs_built;
#ifdef CUDA_COMPILE
  nt = 1;                       // cuda is always 1 thread for this..
#endif
  float net_delta = 0.0f;
  for(int j=0;j<nt;j++) {
    float& ndval = net->ThrSendDeepRawNetTmp(j)[flat_idx];
    net_delta += ndval;
  }
  u->deep_raw_net += net_delta;
}

void LeabraUnitSpec::ClearDeepActs(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  ClearDeepActs_impl(u);
}

///////////////////////////////////////////////////////////////////////
//      Phase and Trial Activation Updating

void LeabraUnitSpec::Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Quarter_Final_RecVals(u, net, thr_no);
}

void LeabraUnitSpec::Quarter_Final_RecVals(LeabraUnitVars* u, LeabraNetwork* net,
                                           int thr_no) {
  Quarter_Final_RecVals_impl(u, net->quarter);
}

void LeabraUnitSpec::Compute_ActTimeAvg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_ActTimeAvg_impl(u);
}

///////////////////////////////////////////////////////////////////////
//      Stats

float LeabraUnitSpec::Compute_SSE(UnitVars* ru, Network* rnet, int thr_no, bool& has_targ) {
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  return Compute_SSE_impl(u, has_targ);
}

bool LeabraUnitSpec::Compute_PRerr
(UnitVars* ru, Network* rnet, int thr_no,
 float& true_pos, float& false_pos, float& false_neg, float& true_neg) {
  // just replaces act_m for act in original
  LeabraUnitVars* u = (LeabraUnitVars*)ru;
  return Compute_PRerr_impl(u, true_pos, false_pos, false_neg, true_neg);
}

float LeabraUnitSpec::Compute_NormErr(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                                      bool& targ_active) {
  return Compute_NormErr_impl(u, targ_active, net->lstats.on_errs, net->lstats.off_errs);
}



//////////////////////////////////////////
//       Misc Functions                 //
//////////////////////////////////////////

void LeabraUnitSpec::BioParams(float norm_sec, float norm_volt, float volt_off, float norm_amp,
          float C_pF, float gbar_l_nS, float gbar_e_nS, float gbar_i_nS,
          float erev_l_mV, float erev_e_mV, float erev_i_mV,
          float act_thr_mV, float spk_thr_mV, float exp_slope_mV,
       float adapt_tau_ms, float adapt_vm_gain_nS, float adapt_spk_gain_nA)
{
  // derived units
  float norm_siemens = norm_amp / norm_volt;
  float norm_farad = (norm_sec * norm_amp) / norm_volt;

  dt.vm_tau = ((C_pF * 1.0e-12f) / norm_farad);

  g_bar.l = (gbar_l_nS * 1.0e-9f) / norm_siemens;
  g_bar.e = (gbar_e_nS * 1.0e-9f) / norm_siemens;
  g_bar.i = (gbar_i_nS * 1.0e-9f) / norm_siemens;

  e_rev.l = ((erev_l_mV * 1.0e-3f) - volt_off) / norm_volt;
  e_rev.e = ((erev_e_mV * 1.0e-3f) - volt_off) / norm_volt;
  e_rev.i = ((erev_i_mV * 1.0e-3f) - volt_off) / norm_volt;

  act.thr = ((act_thr_mV* 1.0e-3f) - volt_off) / norm_volt;
  spike_misc.spk_thr = ((spk_thr_mV * 1.0e-3f) - volt_off) / norm_volt;
  spike_misc.exp_slope = ((exp_slope_mV * 1.0e-3f)) / norm_volt; // no off!
  spike_misc.vm_r = e_rev.l;                                     // go back to leak

  adapt.tau = (adapt_tau_ms * 1.0e-3f) / norm_sec;
  adapt.vm_gain = (adapt_vm_gain_nS * 1.0e-9f) / norm_siemens;
  adapt.spike_gain = (adapt_spk_gain_nA * 1.0e-9f) / norm_amp;

  init.v_m = e_rev.l;
  vm_range.min = 0.0f;
  vm_range.max = 2.0f;

  if(act_fun != SPIKE) {
    dt.vm_tau = 3.3f;
    act.gain = 100;
  }

  UpdateAfterEdit();
}

void LeabraUnitSpec::GraphVmFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_VmFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Vm", idx, VT_FLOAT);
  
  incr = fmaxf(0.001f, incr);	// must be pos

  for(float x = min; x <= max; x += incr) {
    float y = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
      ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(y, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::GraphActFmNetFun(DataTable* graph_data, float g_i, float min, float max,
                                      float incr, float g_e_thr, float lin_gain) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmNetFun", true);
  }
  
  FunLookup        nxx1_fun;
  FunLookup        noise_conv;

  nxx1_fun.OwnTempObj();
  noise_conv.OwnTempObj();

  CreateNXX1Fun(act, nxx1_fun, noise_conv);
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* av = graph_data->FindMakeColName("Act", idx, VT_FLOAT);
  DataCol* lin = graph_data->FindMakeColName("Linear", idx, VT_FLOAT);
  DataCol* cnv = graph_data->FindMakeColName("Convolved", idx, VT_FLOAT);

  av->SetUserData("PLOT_1", true);
  av->SetUserData("MIN", 0.0f);
  av->SetUserData("MAX", 1.0f);

  lin->SetUserData("PLOT_2", true);

//  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  incr = fmaxf(0.001f, incr);	// must be pos
  float x;
  for(x = min; x <= max; x += incr) {
    float aval;
    aval = Compute_ActFun_Rate_fun(x - g_e_thr);
    float val_sub_thr = x - g_e_thr;
    float ln = val_sub_thr;
    if(ln < 0.0f) ln = 0.0f;
    ln *= lin_gain;
    if(ln > 1.0f) ln = 1.0f;
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    av->SetValAsFloat(aval, -1);
    lin->SetValAsFloat(ln, -1);

    float cnval = 0.0f;
    if(val_sub_thr >= nxx1_fun.x_range.max) {
      float tmp = val_sub_thr * act.gain;
      cnval = tmp / (tmp + 1.0f);
    }
    else if(val_sub_thr > nxx1_fun.x_range.min) {
      cnval = nxx1_fun.Eval(val_sub_thr);
    }
    cnv->SetValAsFloat(cnval, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void LeabraUnitSpec::TimeExp(int mode, int nreps) {

  const char* modes[6] = {"double sum", "double exp", "double exp_fast",
                          "float sum", "float exp", "float exp_fast"};

  TimeUsedHR tu;
  tu.StartTimer(true);          // start, w/reset
  double dsum = 0.0;
  float  fsum = 0.0f;
  switch (mode) {
  case 0: {
    for(int i=0;i<nreps; i++) {
      dsum += Random::UniformMinMax(-50.0, 50.0) - 0.99 * dsum;
    }
    break;
  }
  case 1: {
    for(int i=0;i<nreps; i++) {
      dsum += taMath_double::exp(Random::UniformMinMax(-50.0, 50.0)) - 0.99 * dsum;
    }
    break;
  }
  case 2: {
    for(int i=0;i<nreps; i++) {
      dsum += taMath_double::exp_fast(Random::UniformMinMax(-50.0, 50.0)) - 0.99 * dsum;
    }
    break;
  }
  case 3: {
    for(int i=0;i<nreps; i++) {
      fsum += Random::UniformMinMax(-50.0, 50.0) - 0.99f * fsum;
    }
    break;
  }
  case 4: {
    for(int i=0;i<nreps; i++) {
      fsum += taMath_float::exp(Random::UniformMinMax(-50.0, 50.0)) - 0.99f * fsum;
    }
    break;
  }
  case 5: {
    for(int i=0;i<nreps; i++) {
      fsum += taMath_float::exp_fast(Random::UniformMinMax(-50.0, 50.0)) - 0.99f * fsum;
    }
    break;
  }
  }
  tu.EndTimer();
  String msg;
  msg << "mode: " << mode << " " << modes[mode] << " seconds used: " << tu.s_used
      << " dsum: " << dsum << " fsum: " << fsum;
  taMisc::Info(msg);
}

void LeabraUnitSpec::GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_SpikeAlphFun", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* t = graph_data->FindMakeColName("time_fm_spike", idx, VT_FLOAT);
  DataCol* g = graph_data->FindMakeColName("conductance", idx, VT_FLOAT);

  float tmax = fmaxf(spike.window, 2.0f);

  float sumg = 0.0f;
  float x;
  if(!force_alpha && spike.rise == 0.0f && spike.decay > 0.0f) {
    float net = 0.0f;
    float input;
    tmax = 20.0f;
    for(x = 0.0f; x <= tmax; x += 1.0f) {
      if(x < spike.window)
        input = spike.gg_decay / (float)spike.window;
      else
        input = 0.0f;
      net = net + input - net / spike.decay;
      graph_data->AddBlankRow();
      t->SetValAsFloat(x, -1);
      g->SetValAsFloat(net, -1);
      sumg += net;
    }
  }
  else {
    for(x = 0.0f; x <= tmax; x += 1.0f) {
      float y = spike.ComputeAlpha(x);
      graph_data->AddBlankRow();
      t->SetValAsFloat(x, -1);
      g->SetValAsFloat(y, -1);
      sumg += y;
    }
  }
  graph_data->AddBlankRow();
  t->SetValAsFloat(x, -1);
  g->SetValAsFloat(0.0f, -1);
  graph_data->AddBlankRow();
  t->SetValAsFloat(x+1.0f, -1);
  g->SetValAsFloat(sumg, -1);
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

// void LeabraUnitSpec::GraphSLNoiseAdaptFun(DataTable* graph_data, float incr) {
//   taProject* proj = GetMyProj();
//   if(!graph_data) {
//     graph_data = proj->GetNewAnalysisDataTable(name + "_SLNoiseAdaptFun", true);
//   }
//   int idx;
//   graph_data->StructUpdate(true);
//   graph_data->ResetData();
//   DataCol* lt = graph_data->FindMakeColName("LongTerm", idx, VT_FLOAT);
//   lt->SetUserData("X_AXIS", true);
//   DataCol* st = graph_data->FindMakeColName("ShortTerm", idx, VT_FLOAT);
//   st->SetUserData("Z_AXIS", true);
//   DataCol* nv = graph_data->FindMakeColName("NoiseVarMult", idx, VT_FLOAT);
//   nv->SetUserData("PLOT_1", true);
//   nv->SetUserData("MIN", 0.0f);
//   nv->SetUserData("MAX", 1.0f);

//   LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

//   for(float ltv = 0.0f; ltv <= 1.0f; ltv += incr) {
//     for(float stv = 0.0f; stv <= 1.0f; stv += incr) {
//       float nvar = noise_adapt.SLNoiseFun(stv, ltv);
//       graph_data->AddBlankRow();
//       lt->SetValAsFloat(ltv, -1);
//       st->SetValAsFloat(stv, -1);
//       nv->SetValAsFloat(nvar, -1);
//     }
//   }
//   taDataAnal::Matrix3DGraph(graph_data, "LongTerm", "ShortTerm");
//   graph_data->SetUserData("NEG_DRAW", false);
//   graph_data->SetUserData("NEG_DRAW_Z", false);
//   graph_data->StructUpdate(false);
//   graph_data->FindMakeGraphView();
// }


