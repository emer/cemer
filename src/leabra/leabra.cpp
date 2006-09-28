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



#include "leabra.h"

#include "pdplog.h"
#include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

// module initialization
InitProcRegistrar mod_init_leabra(ta_Init_leabra);

//////////////////////////
//  	Con, Spec	//
//////////////////////////

void LeabraCon::Copy_(const LeabraCon& cp) {
  dwt = cp.dwt;
  pdw = cp.pdw;
}

void WtScaleSpec::Initialize() {
  rel = 1.0f;
  abs = 1.0f;
}

void WtSigSpec::Initialize() {
  gain = 6.0f;
  off = 1.25f;
}

void LearnMixSpec::Initialize() {
  hebb = .001f;
  err = 1.0f - hebb;
  err_sb = true;
}

void LearnMixSpec::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();
  err = 1.0f - hebb;
}

void SAvgCorSpec::Initialize() {
  cor = .4f;
  thresh = .001f;
  norm_con_n = false;
}

void LeabraConSpec::Initialize() {
  min_obj_type = &TA_LeabraCon_Group;
  min_con_type = &TA_LeabraCon;
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.sym = true;
  wt_limits.type = WeightLimits::MIN_MAX;
  inhib = false;

  wt_sig_fun.x_range.min = 0.0f;
  wt_sig_fun.x_range.max = 1.0f;
  wt_sig_fun.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun.UpdateAfterEdit();

  wt_sig_fun_inv.x_range.min = 0.0f;
  wt_sig_fun_inv.x_range.max = 1.0f;
  wt_sig_fun_inv.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun_inv.UpdateAfterEdit();

  wt_sig_fun_lst.off = -1;   wt_sig_fun_lst.gain = -1; // trigger an update
  wt_sig_fun_res = -1.0;

  rnd.mean = .5f;
  rnd.var = .25f;
  lrate = .01f;
  cur_lrate = .01f;
  lrs_value = EPOCH;
  lrate_sched.interpolate = false;
}

void LeabraConSpec::InitLinks() {
  ConSpec::InitLinks();
  taBase::Own(wt_scale, this);
  taBase::Own(wt_sig, this);
  taBase::Own(lrate_sched, this);
  taBase::Own(lmix, this);
  taBase::Own(savg_cor, this);
  taBase::Own(wt_sig_fun, this);
  taBase::Own(wt_sig_fun_inv, this);
  taBase::Own(wt_sig_fun_lst, this);
  CreateWtSigFun();
}

void LeabraConSpec::UpdateAfterEdit() {
  ConSpec::UpdateAfterEdit();
  lrate_sched.UpdateAfterEdit();
  CreateWtSigFun();
  lmix.UpdateAfterEdit();
}

void LeabraConSpec::Defaults() {
  wt_scale.Initialize();
  wt_sig.Initialize();
  lmix.Initialize();
  savg_cor.Initialize();
  Initialize();
}

void LeabraConSpec::SetCurLrate(int epoch, LeabraNetwork* net) {
  cur_lrate = lrate;		// as a backup..
  if(lrs_value == NO_LRS) return;

  if(lrs_value == EXT_REW_AVG) {
    LeabraLayer* er_lay = LeabraLayerSpec::FindLayerFmSpecNet(net, &TA_ExtRewLayerSpec);
    if(er_lay != NULL) {
      LeabraUnit* un = (LeabraUnit*)er_lay->units.Leaf(0);
      float avg_rew = un->act_avg;
      int ar_pct = (int)(100.0f * avg_rew);
      cur_lrate = lrate * lrate_sched.GetVal(ar_pct);
      return;
    }
    else {
      taMisc::Error("*** Warning: LeabraConSpec::SetCurLrate(): appropriate ExtRew layer not found for EXT_REW_AVG, reverting to EPOCH! for:",
		    name);
      SetUnique("lrs_value", true);
      lrs_value = EPOCH;
      UpdateAfterEdit();
    }
  }
  if(lrs_value == EXT_REW_STAT) {
    int arval = 0;
    if(net->epoch < 1) {
      arval = lrate_sched.last_ctr;
    }
    else {
      arval = (int)(100.0f * net->avg_ext_rew);
    }
    cur_lrate = lrate * lrate_sched.GetVal(arval);
  }

  // this is no longer relevant
//   if(lrs_value == SE_STAT) {
//     int seval = 0;
//     if(net->epoch < 1) {
//       seval = lrate_sched.last_ctr;
//     }
//     else {
//       seval = (int);
//     }
//     cur_lrate = lrate * lrate_sched.GetVal(seval);
//   }

  if(lrs_value == EPOCH) {
    cur_lrate = lrate * lrate_sched.GetVal(epoch);
  }
}

void LeabraConSpec::CreateWtSigFun() {
  if((wt_sig_fun_lst.gain == wt_sig.gain) && (wt_sig_fun_lst.off == wt_sig.off)
     && (wt_sig_fun_res == wt_sig_fun.res))
    return;
  wt_sig_fun.AllocForRange();
  int i;
  for(i=0; i<wt_sig_fun.size; i++) {
    float w = wt_sig_fun.Xval(i);
    wt_sig_fun[i] = WtSigSpec::SigFun(w, wt_sig.gain, wt_sig.off);
  }
  wt_sig_fun_inv.AllocForRange();
  for(i=0; i<wt_sig_fun_inv.size; i++) {
    float w = wt_sig_fun_inv.Xval(i);
    wt_sig_fun_inv[i] = WtSigSpec::SigFunInv(w, wt_sig.gain, wt_sig.off);
  }
  // prevent needless recomputation of this lookup table..
  wt_sig_fun_lst.gain = wt_sig.gain; wt_sig_fun_lst.off = wt_sig.off;
  wt_sig_fun_res = wt_sig_fun.res;
}

void LeabraConSpec::GraphWtSigFun(GraphLog* graph_log) {
  // todo:
  taMisc::Error("Graphing not yet supported");
  return;
//   if(graph_log == NULL) {
//     graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
//     if(graph_log == NULL) return;
//   }
//   graph_log->name = name + ": Wt Sig Fun";
//   DataTable* dt = &(graph_log->data);
//   dt->Reset();
//   dt->NewColFloat("lin wt");
//   dt->NewColFloat("eff wt");
//   dt->AddColDispOpt("MIN=0", 1);
//   dt->AddColDispOpt("MAX=1", 1);

//   float x;
//   for(x = 0.0; x <= 1.0; x += .01) {
//     float y = WtSigSpec::SigFun(x, wt_sig.gain, wt_sig.off);
//     dt->AddBlankRow();
//     dt->SetLastFloatVal(x, 0);
//     dt->SetLastFloatVal(y, 1);
//   }
//   dt->UpdateAllRanges();
//   graph_log->ViewAllData();
}

void LeabraCon_Group::Initialize() {
  spec.SetBaseType(&TA_LeabraConSpec);
  SetBaseType(&TA_LeabraCon);
  scale_eff = 0.0f;
  savg_cor = 1.0f;
  net = 0.0;
}

void LeabraCon_Group::Copy_(const LeabraCon_Group& cp) {
  scale_eff = cp.scale_eff;
  savg_cor = cp.savg_cor;
  net = cp.net;
}

void LeabraBiasSpec::Initialize() {
  min_obj_type = &TA_LeabraUnitSpec;
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;
}

bool LeabraBiasSpec::CheckObjectType_impl(TAPtr obj) {
  // don't allow anything to point to a biasspec except the unitspec!
  if(!obj->InheritsFrom(TA_BaseSpec) &&
     !obj->InheritsFrom(TA_LeabraCon)) return false;
  return true;
}

void LeabraBiasSpec::Defaults() {
  LeabraConSpec::Defaults();
  Initialize();
}

//////////////////////////
//  	Unit, Spec	//
//////////////////////////

void LeabraChannels::Initialize() {
  e = l = i = h = a = 0.0f;
}

void LeabraChannels::Copy_(const LeabraChannels& cp) {
  e = cp.e;
  l = cp.l;
  i = cp.i;
  h = cp.h;
  a = cp.a;
}

void VChanSpec::Initialize() {
  on = false;
  b_dt = .01f;
  a_thr = .5f;
  d_thr = .1f;
  g_dt = .1f;
  init = false;
}

void VChanBasis::Initialize() {
  hyst = acc = 0.0f;
  hyst_on = acc_on = false;
  g_h = g_a = 0.0f;
}

void VChanBasis::Copy_(const VChanBasis& cp) {
  hyst = cp.hyst;
  acc = cp.acc;
  hyst_on = cp.hyst_on;
  acc_on = cp.acc_on;
  g_h = cp.g_h;
  g_a = cp.g_a;
}

void ActFunSpec::Initialize() {
  thr = .25f;
  gain = 600.0f;
  nvar = .005;
  avg_dt = .005f;
  send_delta = false;
  i_thr = STD;
}

void SpikeFunSpec::Initialize() {
  decay = 0.05f;
  v_m_r = 0.0f;
  eq_gain = 10.0f;
  eq_dt = 0.02f;
  hard_gain = .4f;
  // vm_dt of .1 should also be used; vm_noise var .002???
}

void DepressSpec::Initialize() {
  p_spike = P_NXX1;
  rec = .2f;
  asymp_act = .5f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  max_amp = (.95f * depl + rec) / rec;
}

void DepressSpec::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();
  if(rec < .00001f) rec = .00001f;
  if(asymp_act < .00001f) asymp_act = .00001f;
  if(asymp_act > 1.0f) asymp_act = 1.0f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  depl = MAX(depl, 0.0f);
  max_amp = (.95f * depl + rec) / rec;
}

void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  learn = 0.01f;
  updt_wts = true;
  phase_dif = 0.0f;		// .8 also useful
}

void DtSpec::Initialize() {
  vm = 0.3f;
  net = 0.7f;
  d_vm_max = 0.025f;
  vm_eq_cyc = 0;
  vm_eq_dt = 1.0f;
}

void ActRegSpec::Initialize() {
  on = false;
  min = 0.0f;
  max = .35f;
  wt_dt = 0.2f;
}

void MaxDaSpec::Initialize() {
  val = INET_DA;
  inet_scale = 1.0;
  lay_avg_thr = 0.01f;
}

void LeabraUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraUnit;
  bias_con_type = &TA_LeabraCon;
  bias_spec.SetBaseType(&TA_LeabraConSpec);

  sse_tol = .5f;

  act_fun = NOISY_XX1;

  clamp_range.min = .0f;
  clamp_range.max = .95f;
  clamp_range.UpdateAfterEdit();

  vm_range.max = 1.0f;
  vm_range.min = 0.0f;
  vm_range.UpdateAfterEdit();

  v_m_init.type = Random::UNIFORM;
  v_m_init.mean = .15f;
  v_m_init.var = 0.0f;

  g_bar.e = 1.0f;
  g_bar.l = 0.1f;
  g_bar.i = 1.0f;
  g_bar.h = 0.01f;
  g_bar.a = 0.03f;
  e_rev.e = 1.0f;
  e_rev.l = 0.15f;
  e_rev.i = 0.15f;
  e_rev.h = 1.0f;
  e_rev.a = 0.0f;

  hyst.b_dt = .05f;
  hyst.a_thr = .8f;
  hyst.d_thr = .7f;

  noise_type = NO_NOISE;
  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit();

  nxx1_fun.x_range.min = -.03f;
  nxx1_fun.x_range.max = .20f;
  nxx1_fun.res = .001f;
  nxx1_fun.UpdateAfterEdit();

  CreateNXX1Fun();
}

void LeabraUnitSpec::Defaults() {
  act.Initialize();
  spike.Initialize();
  depress.Initialize();
  opt_thresh.Initialize();
  dt.Initialize();
  act_reg.Initialize();
  Initialize();
  bias_spec.SetSpec(bias_spec.spec);
}

void LeabraUnitSpec::InitLinks() {
  bias_spec.type = &TA_LeabraBiasSpec;
  UnitSpec::InitLinks();
  taBase::Own(act, this);
  taBase::Own(spike, this);
  taBase::Own(depress, this);
  taBase::Own(opt_thresh, this);
  taBase::Own(clamp_range, this);
  taBase::Own(vm_range, this);
  taBase::Own(v_m_init, this);
  taBase::Own(dt, this);
  taBase::Own(g_bar, this);
  taBase::Own(e_rev, this);
  taBase::Own(hyst, this);
  taBase::Own(acc, this);
  taBase::Own(act_reg, this);
  taBase::Own(maxda, this);
  taBase::Own(noise, this);
  taBase::Own(noise_sched, this);
  taBase::Own(nxx1_fun, this);
  taBase::Own(noise_conv, this);
}

void LeabraUnitSpec::UpdateAfterEdit() {
  UnitSpec::UpdateAfterEdit();
  clamp_range.UpdateAfterEdit();
  vm_range.UpdateAfterEdit();
  depress.UpdateAfterEdit();
  noise_sched.UpdateAfterEdit();
  CreateNXX1Fun();
  if(act_fun == DEPRESS)
    act_range.max = depress.max_amp;
}

bool LeabraUnitSpec::CheckConfig(Unit* un, Layer* lay, Network* net, bool quiet) {
  act.send_delta = ((LeabraNetwork*)net)->send_delta; // always copy from network, so it is global..

  Con_Group* recv_gp;
  int g;
  FOR_ITR_GP(Con_Group, recv_gp, un->recv., g) {
    if(!recv_gp->CheckConfig(lay, un, net, quiet)) return false;
  }

  Con_Group* send_gp;
  FOR_ITR_GP(Con_Group, send_gp, un->send., g) {
    if(send_gp->size < 2) continue;
    Unit* ru = send_gp->Un(0);
    float first_sc = ((LeabraCon_Group*)ru->recv.FastGp(send_gp->other_idx))->scale_eff;
    for(int j=1; j<send_gp->size; j++) {
      float sc = ((LeabraCon_Group*)ru->recv.FastGp(send_gp->other_idx))->scale_eff;
      if(sc != first_sc) {
	if(!quiet) taMisc::Error("Leabra CheckConfig Error: the effective weight scales for\
 different sending connections within a group are not all the same!  Sending Layer:",
				 lay->name, ", Rev Layer:", send_gp->prjn->layer->name,
				 ", first_sc: ", (const char*)String(first_sc), ", sc: ",
				 (const char*)String(sc));
				 //				 ", con:", String(j));
	return false;
      }
    }
  }

  if(opt_thresh.updt_wts &&
      (net->wt_update != Network::ON_LINE) &&
      (net->context != Network::TEST)) {
    if(!quiet) taMisc::Error("LeabraUnitSpec Warning: cannot use opt_thresh.updt_wts when wt_update is not ON_LINE",
			     "I turned this flag off for you in LeabraUnitSpec:", name);
    SetUnique("opt_thresh", true);
    opt_thresh.updt_wts = false;
  }

  if(opt_thresh.updt_wts && act_reg.on && (act_reg.min > 0.0f)) {
    if(!quiet) taMisc::Error("LeabraUnitSpec Warning: cannot use opt_thresh.updt_wts when act_reg is on and min > 0",
			     "I turned this flag off for you in LeabraUnitSpec:", name);
    SetUnique("opt_thresh", true);
    opt_thresh.updt_wts = false;
  }
  return true;
}

void LeabraUnitSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  noise_conv.AllocForRange();
  int i;
  float var = act.nvar * act.nvar;
  for(i=0; i < noise_conv.size; i++) {
    float x = noise_conv.Xval(i);
    noise_conv[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = noise_conv.Sum();
  for(i=0; i < noise_conv.size; i++)
    noise_conv[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fun.x_range.min + noise_conv.x_range.min;
  fun.x_range.max = nxx1_fun.x_range.max + noise_conv.x_range.max;
  fun.res = nxx1_fun.res;
  fun.UpdateAfterEdit();
  fun.AllocForRange();

  if(act_fun == LeabraUnitSpec::NOISY_LINEAR) {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
	val = act.gain * x;
      fun[i] = val;
    }
  }
  else {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
	val = (act.gain * x) / ((act.gain * x) + 1.0f);
      fun[i] = val;
    }
  }

  nxx1_fun.Convolve(fun, noise_conv);
}

void LeabraUnitSpec::InitWtState(Unit* u) {
  UnitSpec::InitWtState(u);
  LeabraUnit* lu = (LeabraUnit*)u;
  lu->act_avg = .5 * (act_reg.max + MAX(act_reg.min, 0.0f));
  lu->misc_1 = 0.0f;
  if(act_fun != DEPRESS)
    lu->spk_amp = 0.0f;
  lu->vcb.hyst = lu->vcb.g_h = 0.0f;
  lu->vcb.hyst_on = false;
  lu->vcb.acc = lu->vcb.g_a = 0.0f;
  lu->vcb.acc_on = false;
}

void LeabraUnitSpec::InitActAvg(LeabraUnit* u) {
  u->act_avg = .5 * (act_reg.max + MAX(act_reg.min, 0.0f));
}  

void LeabraUnitSpec::SetCurLrate(LeabraUnit* u, LeabraNetwork* net, int epoch) {
  ((LeabraConSpec*)bias_spec.spec)->SetCurLrate(epoch, net);
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g)
    recv_gp->SetCurLrate(epoch, net);
}

////////////////////////////////////////////
//	Stage 0: at start of settling	  // 
////////////////////////////////////////////

void LeabraUnitSpec::InitDelta(LeabraUnit* u) {
  if(act.send_delta) {
    u->net_delta = 0.0f;
    u->g_i_delta = 0.0f;
    u->net = 0.0f;		// important for soft-clamped layers
    u->gc.i = 0.0f;
  }
  else {
    u->gc.i = 0.0f;
    u->net = u->clmp_net;
  }

  // not initialized by default.
//   LeabraCon_Group* recv_gp;
//   int g;
//   FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
//     recv_gp->net = 0.0f;
//   } 
}

void LeabraUnitSpec::InitState(LeabraUnit* ru, LeabraLayer*) {
  UnitSpec::InitState(ru);
  //  ru->clmp_net = 0.0f;
  ru->net_scale = 0.0f;
  ru->bias_scale = 0.0f;
  ru->prv_net = 0.0f;
  ru->prv_g_i = 0.0f;
  if(hyst.init) {
    ru->vcb.hyst = ru->vcb.g_h = 0.0f;
    ru->vcb.hyst_on = false;
  }
  if(acc.init) {
    ru->vcb.acc = ru->vcb.g_a = 0.0f;
    ru->vcb.acc_on = false;
  }
  ru->gc.l = 0.0f;
  ru->gc.i = 0.0f;
  ru->gc.h = 0.0f;
  ru->gc.a = 0.0f;
  ru->I_net = 0.0f;
  ru->v_m = v_m_init.Gen();
  ru->da = 0.0f;
  ru->act = 0.0f;
  ru->act_eq = 0.0f;
  ru->act_p = ru->act_m = ru->act_dif = 0.0f;

  ru->act_sent = 0.0f;
  ru->act_delta = 0.0f;
  ru->net_raw = 0.0f;
  ru->net_delta = 0.0f;
  ru->g_i_delta = 0.0f;
  ru->g_i_raw = 0.0f;

  ru->i_thr = 0.0f;
  if(act_fun == DEPRESS)
    ru->spk_amp = act_range.max;
}

void LeabraUnitSpec::Compute_NetScale(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // this is all receiver-based and done only at beginning of settling
  u->net_scale = 0.0f;	// total of scale values for this unit's inputs

  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesion)		continue;
     // this is the normalization value: takes into account target activity of layer
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    WtScaleSpec& wt_scale = cs->wt_scale;
    float savg = lay->kwta.pct;
    if(cs->savg_cor.norm_con_n)	// sometimes it makes sense to just do it by the group n
      recv_gp->scale_eff = wt_scale.NetScale() * (1.0f / ((float)recv_gp->size * savg));
    else
      recv_gp->scale_eff = wt_scale.NetScale() * (1.0f / ((float)lay->units.leaves * savg));
    u->net_scale += wt_scale.rel;
  }
  // add the bias weight into the netinput, scaled by 1/n
  if(u->bias != NULL) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.spec;
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(u->n_recv_cons > 0)
      u->bias_scale /= (float)u->n_recv_cons; // one over n scaling for bias!
  }
  // now renormalize
  if(u->net_scale == 0.0f) return;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesion)		continue;
    recv_gp->scale_eff /= u->net_scale; // normalize by total connection scale
  }
}

void LeabraUnitSpec::Compute_NetRescale(LeabraUnit* u, LeabraLayer*, LeabraNetwork*, float new_scale) {
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesion)		continue;
    recv_gp->scale_eff *= new_scale;
  }
  // rescale existing netins so that the delta is not even noticed!
  u->net *= new_scale;
  u->clmp_net *= new_scale;
  u->net_raw *= new_scale;
}

void LeabraUnitSpec::Init_ClampNet(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  u->clmp_net = 0.0f;
  if(u->bias != NULL) {		// add bias weight
    u->clmp_net += u->bias_scale * u->bias->wt;
  }
}

void LeabraUnitSpec::Send_ClampNet(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(u->act > opt_thresh.send) {
    LeabraCon_Group* send_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
      if(send_gp->prjn->layer->lesion) continue;
      if(((LeabraConSpec*)send_gp->spec.spec)->inhib) {
	taMisc::Error("*** Error: cannot send inhibition from a hard-clamped layer!  Set layerspec clamp.hard off!");
	continue;
      }
      send_gp->Send_ClampNet(u);
    }
  }
}

////////////////////////////////////
//	Stage 1: netinput 	  //
////////////////////////////////////

void LeabraUnitSpec::Send_Net(LeabraUnit* u, LeabraLayer*) {
  // sender-based (and this fun not called on hard_clamped EXT layers)
  if(u->act > opt_thresh.send) {
    LeabraCon_Group* send_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesion || tol->hard_clamped)	continue;
      send_gp->Send_Net(u);
    }
  }
}

void LeabraUnitSpec::Send_NetDelta(LeabraUnit* u, LeabraLayer*) {
  if(u->act > opt_thresh.send) {
    if(fabs(u->act_delta) > opt_thresh.delta) {
      LeabraCon_Group* send_gp;
      int g;
      FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
	LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	if(tol->lesion || tol->hard_clamped)	continue;
	send_gp->Send_NetDelta(u);
      }
      u->act_sent = u->act;	// cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    u->act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    LeabraCon_Group* send_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesion || tol->hard_clamped)	continue;
      send_gp->Send_NetDelta(u);
    }
    u->act_sent = 0.0f;		// now it effectively sent a 0..
  }
}

//////////////////////////////////////////////////////////////////
//	Stage 2: netinput averages and clamping (if necc)	//
//////////////////////////////////////////////////////////////////

void LeabraUnitSpec::Compute_HardClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  u->net = u->prv_net = u->ext * lay->stm_gain;
  u->act_eq = clamp_range.Clip(u->ext);
  if(act_fun == SPIKE)
    u->act = spike.hard_gain * u->act_eq;
  else
    u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  u->net = u->prv_net = u->ext * lay->stm_gain;
  //  u->act_eq = clamp_range.Clip(u->ext);
  u->act_eq = u->ext;
  if(act_fun == SPIKE)
    u->act = spike.hard_gain * u->act_eq;
  else
    u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
}

bool LeabraUnitSpec::Compute_SoftClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  bool inc_gain = false;
  if(u->ext_flag & Unit::EXT) {
    if((u->ext > .5f) && (u->act < .5f))
      inc_gain = true;	// must increase gain because targ unit is < .5..

    u->net += u->ext * lay->stm_gain;
  }
  return inc_gain;
}

//////////////////////////////////////////
//	Stage 3: inhibition		//
//////////////////////////////////////////

float LeabraUnitSpec::Compute_IThreshAll(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias != NULL)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias->wt;

  // including the ga and gh terms
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)
	   + u->gc.a * (e_rev.a - act.thr) + u->gc.h * (e_rev.h - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoA(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias != NULL)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias->wt;

  // NOT including the ga term
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)
	   + u->gc.h * (e_rev.h - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoH(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias != NULL)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias->wt;

  // NOT including the gh terms
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)
	   + u->gc.a * (e_rev.a - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoAH(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias != NULL)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias->wt;

  // NOT including the ga and gh terms
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThresh(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  switch(act.i_thr) {
  case ActFunSpec::STD:
    return Compute_IThreshAll(u, lay, net);
  case ActFunSpec::NO_A:
    return Compute_IThreshNoA(u, lay, net);
  case ActFunSpec::NO_H:
    return Compute_IThreshNoH(u, lay, net);
  case ActFunSpec::NO_AH:
    return Compute_IThreshNoAH(u, lay, net);
  }
  return 0.0f;
} 

//////////////////////////////////////////
//	Stage 4: the final activation 	//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_Act(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  Compute_Conduct(u, lay, thr, net);
  Compute_Vm(u, lay, thr, net);
  Compute_ActFmVm(u, lay, thr, net);
  Compute_SelfReg(u, lay, thr, net);
  u->act_delta = u->act - u->act_sent;
}

void LeabraUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*, LeabraNetwork*) {
  // total conductances
  float g_bar_i_val = g_bar.i;
  float	g_bar_e_val = g_bar.e;
  float	g_bar_l_val = g_bar.l;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->spec.spec;
  if((ls->adapt_i.type == AdaptISpec::G_BAR_I) || (ls->adapt_i.type == AdaptISpec::G_BAR_IL))
    g_bar_i_val = lay->adapt_i.g_bar_i; // adapting value..
  if(ls->adapt_i.type == AdaptISpec::G_BAR_IL)
    g_bar_l_val = lay->adapt_i.g_bar_l; // adapting value..
  u->gc.i *= g_bar_i_val;
  u->net *= g_bar_e_val;
  u->gc.l = g_bar_l_val;
  u->gc.h = g_bar.h * u->vcb.g_h;
  u->gc.a = g_bar.a * u->vcb.g_a;
}

void LeabraUnitSpec::Compute_Vm(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork* net) {
  if(dt.vm_eq_cyc > net->cycle) {
    // directly go to equilibrium value
    float new_v_m= (((u->net * e_rev.e) + (u->gc.l * e_rev.l) + (u->gc.i * e_rev.i) +
		     (u->gc.h * e_rev.h) + (u->gc.a * e_rev.a)) / 
		    (u->net + u->gc.l + u->gc.i + u->gc.h + u->gc.a));
    u->I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += dt.vm_eq_dt * u->I_net;
  }
  else {
    u->I_net = 
      (u->net * (e_rev.e - u->v_m)) + (u->gc.l * (e_rev.l - u->v_m)) + 
      (u->gc.i * (e_rev.i - u->v_m)) + (u->gc.h * (e_rev.h - u->v_m)) +
      (u->gc.a * (e_rev.a - u->v_m));

    float dvm = dt.vm * u->I_net;
    if(dvm > dt.d_vm_max) dvm = dt.d_vm_max;
    else if(dvm < -dt.d_vm_max) dvm = -dt.d_vm_max;
    u->v_m += dvm;
  }

  if((noise_type == VM_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->v_m += noise_sched.GetVal(net->cycle) * noise.Gen();
  }

  u->v_m = vm_range.Clip(u->v_m);
}

void LeabraUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork* net) {
  float new_act = u->v_m - act.thr;
  switch(act_fun) {
  case NOISY_XX1: {
    if(new_act <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(new_act >= nxx1_fun.x_range.max) {
      new_act *= act.gain;
      new_act = new_act / (new_act + 1.0f);
    }
    else {
      new_act = nxx1_fun.Eval(new_act);
    }
  }
  break;
  case XX1: {
    if(new_act < 0.0f)
      new_act = 0.0f;
    else {
      new_act *= act.gain;
      new_act = new_act / (new_act + 1.0f);
    }
  }
  break;
  case NOISY_LINEAR: {
    if(new_act <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(new_act >= nxx1_fun.x_range.max) {
      new_act *= act.gain;
    }
    else {
      new_act = nxx1_fun.Eval(new_act);
    }
  }
  break;
  case LINEAR: {
    if(new_act < 0.0f)
      new_act = 0.0f;
    else
      new_act *= act.gain;
  }
  break;
  case DEPRESS: {
    // new_act = spike probability
    if(depress.p_spike == DepressSpec::P_NXX1) {
      if(new_act <= nxx1_fun.x_range.min)
	new_act = 0.0f;
      else if(new_act >= nxx1_fun.x_range.max) {
	new_act *= act.gain;
	new_act = new_act / (new_act + 1.0f);
      }
      else {
	new_act = nxx1_fun.Eval(new_act);
      }
    }
    else {
      if(new_act < 0.0f)
	new_act = 0.0f;
      else {
	new_act *= act.gain;
	new_act = MIN(new_act, 1.0f);
      }
    }
    new_act *= u->spk_amp; // actual output = probability * amplitude
    u->spk_amp += -new_act * depress.depl + (act_range.max - u->spk_amp) * depress.rec;
    u->spk_amp = act_range.Clip(u->spk_amp);
  }
  break;
  case SPIKE: {
    float spike_act = 0.0f;
    if(u->v_m > act.thr) {
      u->act = 1.0f;
      u->v_m = spike.v_m_r;
      spike_act = 1.0f;
    }
    else {
      u->act *= (1.0f - spike.decay);
    }
    float new_eq = u->act_eq / spike.eq_gain;
    if(spike.eq_dt > 0.0f) {
      new_eq = act_range.Clip(spike.eq_gain * ((1.0f - spike.eq_dt) * new_eq + spike.eq_dt * spike_act));
    }
    else {
      if(net->cycle > 0)
	new_eq *= (float)net->cycle;
      new_eq = act_range.Clip(spike.eq_gain * (new_eq + spike_act) / (float)(net->cycle+1));
    }
    u->da = new_eq - u->act_eq;	// da is on equilibrium activation
    u->act_eq = new_eq;
  }
  break;
  }

  if(act_fun != SPIKE) {
    u->da = new_act - u->act;
    if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
      new_act += noise_sched.GetVal(net->cycle) * noise.Gen();
    }
    u->act = u->act_eq = act_range.Clip(new_act);
  }
}

void LeabraUnitSpec::Compute_SelfReg(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork*) {
  // fast-time scale updated during settling
  hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

void LeabraUnitSpec::Compute_MaxDa(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*, LeabraNetwork* net) {
  float fda;
  if(maxda.val == MaxDaSpec::DA_ONLY)
    fda = fabsf(u->da);
  else if(maxda.val == MaxDaSpec::INET_ONLY)
    fda = fabsf(maxda.inet_scale * u->I_net);
  else {
    if(lay->acts.avg <= maxda.lay_avg_thr)
      fda = fabsf(maxda.inet_scale * u->I_net);
    else
      fda = fabsf(u->da);
  }
  net->maxda = MAX(fda, net->maxda);
}

//////////////////////////////////////////
//	Stage 5: Between Events 	//
//////////////////////////////////////////

void LeabraUnitSpec::PhaseInit(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(!(u->ext_flag & Unit::TARG))
    return;

  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    if(!(lay->ext_flag & Unit::TARG)) {	// layer isn't a target but unit is..
      u->targ = u->ext;
    }
    u->ext = 0.0f;
    u->UnSetExtFlag(Unit::EXT);
  }
  else {
    u->ext = u->targ;
    u->SetExtFlag(Unit::EXT);
  }
}

void LeabraUnitSpec::DecayPhase(LeabraUnit* u, LeabraLayer*, LeabraNetwork*, float decay) {
  u->v_m -= decay * (u->v_m - v_m_init.mean);
  u->act -= decay * u->act;
  u->act_eq -= decay * u->act_eq;
  u->prv_net -= decay * u->prv_net;
  u->prv_g_i -= decay * u->prv_g_i;
  u->vcb.hyst -= decay * u->vcb.hyst;
  u->vcb.acc -= decay * u->vcb.acc;
  if(act_fun == DEPRESS)
    u->spk_amp += (act_range.max - u->spk_amp) * decay;

  // reset the rest of this stuff just for clarity
  u->act_sent = 0.0f;
  u->act_delta = u->act;
  u->net_raw = 0.0f;
  u->net_delta = 0.0f;
  u->g_i_raw = 0.0f;
  u->g_i_delta = 0.0f;
  
  u->net = 0.0f;
  u->net_scale = u->bias_scale = 0.0f;
  u->da = u->I_net = 0.0f;
}

void LeabraUnitSpec::DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay) {
  LeabraUnitSpec::DecayPhase(u, lay, net, decay);
}

void LeabraUnitSpec::ExtToComp(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(!(u->ext_flag & Unit::EXT))
    return;
  u->ext_flag = Unit::COMP;
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::TargExtToComp(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(!(u->ext_flag & Unit::TARG_EXT))
    return;
  u->ext_flag = Unit::COMP;
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::PostSettle(LeabraUnit* u, LeabraLayer*, LeabraInhib*,
				LeabraNetwork* net, bool set_both)
{
  if(set_both) {
    u->act_m = u->act_p = u->act_eq;
    u->act_dif = 0.0f;
    if(act.avg_dt > 0.0f)
      u->act_avg += act.avg_dt * (u->act_eq - u->act_avg);
  }
  else {
    if(net->phase == LeabraNetwork::MINUS_PHASE)
      u->act_m = u->act_eq;
    else if((net->phase == LeabraNetwork::PLUS_PHASE) && (net->phase_no < 2)) {
      // act_p is only for first plus phase: others require something else
      u->act_p = u->act_eq;
      if(act.avg_dt > 0.0f)
	u->act_avg += act.avg_dt * (u->act_eq - u->act_avg);
      u->act_dif = u->act_p - u->act_m;
    }
  }
}

//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if((u->act_p <= opt_thresh.learn) && (u->act_m <= opt_thresh.learn))
    return;
  if(lay->phase_dif_ratio < opt_thresh.phase_dif)
    return;
  Compute_dWt_impl(u, lay, net);
}

void LeabraUnitSpec::Compute_dWt_impl(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // don't adapt bias weights on clamped inputs..: why?  what possible consequence could it have!?
  // furthermore: it is not right for units that are clamped in 2nd plus phase!
  //  if(!((u->ext_flag & Unit::EXT) && !(u->ext_flag & Unit::TARG))) {
  ((LeabraConSpec*)bias_spec.spec)->B_Compute_dWt((LeabraCon*)u->bias, u);
    //  }
  UnitSpec::Compute_dWt(u);
}

void LeabraUnitSpec::Compute_WtFmLin(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesion)	continue;
    recv_gp->Compute_WtFmLin();
  }
}

void LeabraUnitSpec::UpdateWeights(Unit* u) {
  LeabraUnit* lu = (LeabraUnit*)u;
  // see above commment
  //  if(!((lu->ext_flag & Unit::EXT) && !(lu->ext_flag & Unit::TARG))) {
  ((LeabraConSpec*)bias_spec.spec)->B_UpdateWeights((LeabraCon*)u->bias, lu, this);
    //  }
  if(opt_thresh.updt_wts && 
     ((lu->act_p <= opt_thresh.learn) && (lu->act_m <= opt_thresh.learn)))
    return;
  UnitSpec::UpdateWeights(lu);
}

float LeabraUnitSpec::Compute_SSE(Unit* u) {
  LeabraUnit* lu = (LeabraUnit*)u;
  if(lu->ext_flag & Unit::TARG) {
    float uerr = lu->targ - lu->act_m;
    if(fabs(uerr) < sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  else
    return 0.0f;
}

//////////////////////////////////////////
//	 Misc Functions 		//
//////////////////////////////////////////

void LeabraUnitSpec::GraphVmFun(GraphLog* graph_log, float g_i, float min, float max, float incr) {
  // todo:
  taMisc::Error("Graphing not yet supported");
  return;
//   if(graph_log == NULL) {
//     graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
//     if(graph_log == NULL) return;
//   }
//   graph_log->name = name + ": Vm Fun";
//   DataTable* dt = &(graph_log->data);
//   dt->Reset();
//   dt->NewColFloat("net");
//   dt->NewColFloat("v_m");

//   float x;
//   for(x = min; x <= max; x += incr) {
//     float y = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
//       ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
//     dt->AddBlankRow();
//     dt->SetLastFloatVal(x, 0);
//     dt->SetLastFloatVal(y, 1);
//   }
//   dt->UpdateAllRanges();
//   graph_log->ViewAllData();
}

void LeabraUnitSpec::GraphActFmVmFun(GraphLog* graph_log, float min, float max, float incr) {
  // todo:
  taMisc::Error("GraphActFmVmFun not yet supported");
  return;
//   if(graph_log == NULL) {
//     graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
//     if(graph_log == NULL) return;
//   }
//   graph_log->name = name + ": Act Fm Vm Fun";
//   DataTable* dt = &(graph_log->data);
//   dt->Reset();
//   dt->NewColFloat("v_m");
//   dt->NewColFloat("act");

//   LeabraUnit un;
//   LeabraNetwork net;

//   float x;
//   for(x = min; x <= max; x += incr) {
//     un.v_m = x;
//     Compute_ActFmVm(&un, NULL, NULL, &trl);
//     dt->AddBlankRow();
//     dt->SetLastFloatVal(x, 0);
//     dt->SetLastFloatVal(un.act, 1);
//   }
//   dt->UpdateAllRanges();
//   graph_log->ViewAllData();
}

void LeabraUnitSpec::GraphActFmNetFun(GraphLog* graph_log, float g_i, float min, float max, float incr) {
  // todo:
  taMisc::Error("GraphActFmNetFun not yet supported");
  return;
//   if(graph_log == NULL) {
//     graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
//     if(graph_log == NULL) return;
//   }
//   graph_log->name = name + ": Act Fm Net Fun";
//   DataTable* dt = &(graph_log->data);
//   dt->Reset();
//   dt->NewColFloat("v_m");
//   dt->NewColFloat("act");

//   LeabraUnit un;
//   LeabraNetwork net;

//   float x;
//   for(x = min; x <= max; x += incr) {
//     un.v_m = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
//       ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
//     Compute_ActFmVm(&un, NULL, NULL, &trl);
//     dt->AddBlankRow();
//     dt->SetLastFloatVal(x, 0);
//     dt->SetLastFloatVal(un.act, 1);
//   }
//   dt->UpdateAllRanges();
//   graph_log->ViewAllData();
}

//////////////////////////
//  	Unit 		//
//////////////////////////


void LeabraUnitChans::Initialize() {
  l = i = h = a = 0.0f;
}

void LeabraUnitChans::Copy_(const LeabraUnitChans& cp) {
  l = cp.l;
  i = cp.i;
  h = cp.h;
  a = cp.a;
}

void LeabraUnit::Initialize() {
  spec.SetBaseType(&TA_LeabraUnitSpec);
  recv.spec.SetBaseType(&TA_LeabraConSpec);
  send.spec.SetBaseType(&TA_LeabraConSpec);

  act_eq = 0.0f;
  act_avg = 0.1f;
  act_p = act_m = act_dif = 0.0f;
  da = 0.0f;
  I_net = 0.0f;
  v_m = 0.0f;

  in_subgp = false;
  clmp_net = 0.0f;
  net_scale = 0.0f;
  bias_scale = 0.0f;
  prv_net = 0.0f;
  prv_g_i = 0.0f;

  act_sent = 0.0f;
  act_delta = 0.0f;
  net_raw = 0.0f;
  net_delta = 0.0f;
  g_i_delta = 0.0f;
  g_i_raw = 0.0f;

  i_thr = 0.0f;
  spk_amp = 2.0f;
  misc_1 = 0.0f;
}

void LeabraUnit::InitLinks() {
  Unit::InitLinks();
  taBase::Own(vcb, this);
  taBase::Own(gc, this);
  GetInSubGp();
}

void LeabraUnit::GetInSubGp() {
  Unit_Group* ownr = (Unit_Group*)owner;  
  if((ownr != NULL) && (ownr->owner != NULL) && ownr->owner->InheritsFrom(TA_taSubGroup))
    in_subgp = true;
  else
    in_subgp = false;
}

//////////////////////////
//  	Layer, Spec	//
//////////////////////////

void KWTASpec::Initialize() {
  k_from = USE_PCT;
  k = 12;
  pct = .25f;
  pat_q = .5f;
  diff_act_pct = false;
  act_pct = .1;
  gp_i = false;
  gp_g = .5f;
}

void AdaptISpec::Initialize() {
  type = NONE;
  tol = .02f;			// allow to be off by this amount before taking action
  p_dt = .1f;			// take reasonably quick action..
  mx_d = .9f;			// move this far in either direction
  l = .2f;			// proportion to assign to leak..
  a_dt = .005f;			// time averaging
}

void KwtaTieBreak::Initialize() {
  on = false;
  k_thr = 1.0f;
  diff_thr = 0.2f;
}

void ClampSpec::Initialize() {
  hard = true;
  gain = .5f;
  d_gain = 0.0f;
}

void DecaySpec::Initialize() {
  event = 1.0f;
  phase = 1.0f;
  phase2 = 0.0f;
  clamp_phase2 = false;
}

void LayNetRescaleSpec::Initialize() {
  on = false;
  max_net = 0.6f;
  net_extra = 0.2f;
}

void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  inhib_group = ENTIRE_LAYER;
  compute_i = KWTA_INHIB;
  i_kwta_pt = .25f;
  gp_i_pt = .2f;
}

void LeabraLayerSpec::Defaults() {
  adapt_i.Initialize();
  clamp.Initialize();
  decay.Initialize();
  net_rescale.Initialize();
  Initialize();
}

void LeabraLayerSpec::UpdateAfterEdit() {
  LayerSpec::UpdateAfterEdit();
}

void LeabraLayerSpec::InitLinks() {
  LayerSpec::InitLinks();
  taBase::Own(kwta, this);
  taBase::Own(gp_kwta, this);
  taBase::Own(tie_brk, this);
  taBase::Own(adapt_i, this);
  taBase::Own(clamp, this);
  taBase::Own(decay, this);
  taBase::Own(net_rescale, this);
}

void LeabraLayerSpec::CutLinks() {
  LayerSpec::CutLinks();
}

bool LeabraLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  LeabraUnit* u;
  taLeafItr ui;
  FOR_ITR_EL(LeabraUnit, u, lay->units., ui) {
    if(!u->CheckConfig(lay, net, quiet)) return false;
  }
  return true;
}

void LeabraLayerSpec::HelpConfig() {
  String help = "LeabraLayerSpec Configuration:\n\
The layer spec sets the target level of activity, k, for each layer.  \
Therefore, you must have a different layer spec with an appropriate activity \
level for layers that have different activity levels.  Note that if you set \
the activity level by percent this will work for different sized layers that \
have the same percent activity level.";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
}

void LeabraLayerSpec::InitWtState(LeabraLayer* lay) {
  Compute_Active_K(lay);	// need kwta.pct for init
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->InitWtState();
  if(lay->units.gp.size > 0) {
    int gi;
    for(gi=0;gi<lay->units.gp.size;gi++) {
      LeabraUnit_Group* gp = (LeabraUnit_Group*)lay->units.gp[gi];
      gp->misc_state = gp->misc_state1 = gp->misc_state2 = 0;
    }
  }
  InitInhib(lay);		// initialize inhibition at start..
}

void LeabraLayerSpec::InitActAvg(LeabraLayer* lay) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->InitActAvg();
}

void LeabraLayerSpec::SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->SetCurLrate(net, epoch);
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from;
    if(fmlay->spec.spec->InheritsFrom(layer_spec)) {	// inherits - not excact match!
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
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from;
    if(fmlay->spec.spec->GetTypeDef() == layer_spec) {	// not inherits - excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecNet(Network* net, TypeDef* layer_spec) {
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if(lay->spec.spec->InheritsFrom(layer_spec)) {	// inherits - not excact match!
      return lay;
    }
  }
  return NULL;
}

//////////////////////////////////////////
//	Stage 0: at start of settling	// 
//////////////////////////////////////////

void LeabraLayerSpec::Compute_Active_K(LeabraLayer* lay) {
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int totk = 0;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_Active_K_impl(lay, rugp, (LeabraInhib*)rugp, gp_kwta);
      totk += rugp->kwta.k;
    }
    if(inhib_group == UNIT_GROUPS) {
      if(lay->kwta.k != totk)
	lay->Inhib_ResetSortBuf();
      lay->kwta.k = totk;
      lay->kwta.Compute_Pct(lay->units.leaves);
      if(gp_kwta.diff_act_pct)
	lay->kwta.pct = gp_kwta.act_pct;	// override!!
    }
  }
  if(inhib_group != UNIT_GROUPS) {
    Compute_Active_K_impl(lay, &(lay->units), (LeabraInhib*)lay, kwta);
    if(kwta.diff_act_pct)
      lay->kwta.pct = kwta.act_pct;	// override!!
  }
}

void LeabraLayerSpec::Compute_Active_K_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					    KWTASpec& kwtspec)
{
  int new_k = 0;
  if(kwtspec.k_from == KWTASpec::USE_PCT)
    new_k = (int)(kwtspec.pct * (float)ug->leaves);
  else if((kwtspec.k_from == KWTASpec::USE_PAT_K) && 
	  ((lay->ext_flag & (Unit::TARG | Unit::COMP)) || (lay->ext_flag & Unit::EXT)))
    new_k = Compute_Pat_K(lay, ug, thr);
  else
    new_k = kwtspec.k;

  if(compute_i == KWTA_INHIB)
    new_k = MIN(ug->leaves - 1, new_k);
  else
    new_k = MIN(ug->leaves, new_k);
  new_k = MAX(1, new_k);

  if(thr->kwta.k != new_k)
    thr->Inhib_ResetSortBuf();

  thr->kwta.k = new_k;
  thr->kwta.Compute_Pct(ug->leaves);
}

int LeabraLayerSpec::Compute_Pat_K(LeabraLayer* lay, Unit_Group* ug, LeabraInhib*) {
  bool use_comp = false;
  if(lay->ext_flag & Unit::COMP) // only use comparison vals if entire lay is COMP!
    use_comp = true;
  int pat_k = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    // use either EXT or TARG information...
    if(u->ext_flag & Unit::EXT) {
      if(u->ext >= kwta.pat_q)
	pat_k++;
    }
    else if(u->ext_flag & Unit::TARG) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }
    else if(use_comp && (u->ext_flag | Unit::COMP)) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }	      
  }
  return pat_k;
}

void LeabraLayerSpec::InitState(LeabraLayer* lay) {
  lay->ext_flag = Unit::NO_EXTERNAL;
  lay->stm_gain = clamp.gain;
  lay->hard_clamped = false;
  lay->prv_phase = LeabraNetwork::MINUS_PHASE;
  lay->net_rescale = 1.0f;
  lay->ResetSortBuf();
  Compute_Active_K(lay);	// need kwta.pct for init
  lay->Inhib_InitState(this);
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->Inhib_InitState(this);
    }
  }
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->InitState(lay);
}

void LeabraLayerSpec::Compute_HardClampPhase2(LeabraLayer* lay, LeabraNetwork* net) {
  // special case to speed up processing by clamping 2nd plus phase to prior + phase
  lay->SetExtFlag(Unit::EXT);
  lay->hard_clamped = true;
  lay->Inhib_SetVals(i_kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->SetExtFlag(Unit::EXT);
    u->ext = u->act_p;
    u->Compute_HardClampNoClip(lay, net); // important: uses no clip here: not really external values!
  }
  Compute_ActAvg(lay, net);
}

void LeabraLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(decay.clamp_phase2 && (net->phase != LeabraNetwork::MINUS_PHASE) && (net->phase_no >= 2)) {
    Compute_HardClampPhase2(lay, net);
    return;
  }
  if(!(clamp.hard && (lay->ext_flag & Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }
  lay->hard_clamped = true;	// cache this flag
  lay->Inhib_SetVals(i_kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_HardClamp(lay, net);
  }
  Compute_ActAvg(lay, net);
}

void LeabraLayerSpec::Compute_NetScale(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_NetScale(lay, net);
  }
}

void LeabraLayerSpec::Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_ClampNet(lay, net);
}

void LeabraLayerSpec::Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_ClampNet(lay, net);
}

//////////////////////////////////
//	Stage 1: netinput 	//
//////////////////////////////////

void LeabraLayerSpec::Send_Net(LeabraLayer* lay) {
  // hard-clamped input layers are already computed in the clmp_net value
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_Net(lay);
}

void LeabraLayerSpec::Send_NetDelta(LeabraLayer* lay) {
  // hard-clamped input layers are already computed in the clmp_net value
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_NetDelta(lay);
}

//////////////////////////////////////////////////////////////////
//	Stage 2: netinput averages and clamping (if necc)	//
//////////////////////////////////////////////////////////////////

void LeabraLayerSpec::Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  if(lay->ext_flag & Unit::EXT)
    Compute_SoftClamp(lay, net);

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    lay->netin.avg = 0.0f; lay->netin.max = -FLT_MAX; lay->netin.max_i = -1;
    lay->i_thrs.avg = 0.0f; lay->i_thrs.max = -FLT_MAX; lay->i_thrs.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_NetAvg(lay, rugp, (LeabraInhib*)rugp, net);
      // keep maximums for linking purposes
      lay->netin.avg = MAX(lay->netin.avg, rugp->netin.avg);
      if(rugp->netin.max > lay->netin.max) {
	lay->netin.max = rugp->netin.max; lay->netin.max_i = g;
      }
      lay->i_thrs.avg = MAX(lay->i_thrs.avg, rugp->i_thrs.avg);
      if(rugp->i_thrs.max > lay->i_thrs.max) {
	lay->i_thrs.max = rugp->i_thrs.max; lay->i_thrs.max_i = g;
      }
    }
  }
  else {
    Compute_NetAvg(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_NetAvg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  thr->netin.avg = 0.0f; thr->netin.max = -FLT_MAX; thr->netin.max_i = -1;
  thr->i_thrs.avg = 0.0f; thr->i_thrs.max = -FLT_MAX; thr->i_thrs.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_NetAvg(lay, thr, net);
    thr->netin.avg += u->net;
    if(u->net > thr->netin.max) {
      thr->netin.max = u->net;  thr->netin.max_i = lf;
    }
    thr->i_thrs.avg += u->i_thr;
    if(u->i_thr > thr->i_thrs.max) {
      thr->i_thrs.max = u->i_thr;  thr->i_thrs.max_i = lf;
    }
    lf++;
  }

  if(ug->leaves > 0) {
    thr->netin.avg /= (float)ug->leaves;	// turn it into an average
    thr->i_thrs.avg /= (float)ug->leaves;	// turn it into an average
  }
}

void LeabraLayerSpec::Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  bool inc_gain = false;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    if(!inc_gain)
      inc_gain = u->Compute_SoftClamp(lay, net);
    else
      u->Compute_SoftClamp(lay, net);
  }
  if(inc_gain && (net->cycle > 1)) // only increment after it has a chance to activate
    lay->stm_gain += clamp.d_gain;
}


//////////////////////////////////////////
//	Stage 3: inhibition		//
//////////////////////////////////////////

void LeabraSort::FastInsertLink(void* it, int where) {
  if((where >= size) || (where < 0)) {
    AddEl_(it);
    return;
  }
  if(size > 0)
    AddEl_(NULL);
  int i;
  for(i=size-1; i>where; i--)
    el[i] = el[i-1];
  el[where] = it;
}

int LeabraSort::FindNewNetPos(float nw_net) {
  int st = 0;
  int n = size;
  while(true) {
    if(n <= 2) {
      if(FastEl(st)->i_thr < nw_net)
	return st;
      else if((st+1 < size) && (FastEl(st+1)->i_thr < nw_net))
	return st + 1;
      else if((st+2 < size) && (FastEl(st+2)->i_thr < nw_net))
	return st + 2;
      return st;
    }
    int n2 = n / 2;
    if(FastEl(st + n2)->i_thr < nw_net)
      n = n2;			// search in upper 1/2 of list
    else {
      st += n2; n -= n2;	// search in lower 1/2 of list
    }
  }
}

void LeabraLayerSpec::InitInhib(LeabraLayer* lay) {
  lay->adapt_i.avg_avg = lay->kwta.pct;
  lay->adapt_i.i_kwta_pt = i_kwta_pt;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  lay->adapt_i.g_bar_i = us->g_bar.i;
  lay->adapt_i.g_bar_l = us->g_bar.l;
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->adapt_i.avg_avg = rugp->kwta.pct;
      if((compute_i == AVG_MAX_PT_INHIB) && (inhib_group != ENTIRE_LAYER))
	rugp->adapt_i.i_kwta_pt = gp_i_pt;
      else
	rugp->adapt_i.i_kwta_pt = i_kwta_pt;
      rugp->adapt_i.g_bar_i = us->g_bar.i;
      rugp->adapt_i.g_bar_l = us->g_bar.l;
    }
  }
}

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped)	return;	// say no more..

  if(inhib_group != UNIT_GROUPS) {
    Compute_Inhib_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
  if(lay->units.gp.size > 0) {
    if(inhib_group == UNIT_GROUPS) {
      lay->Inhib_SetVals(0.0f);
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	Compute_Inhib_impl(lay, rugp, (LeabraInhib*)rugp, net);
	float gp_g_i = rugp->i_val.g_i;
	if(gp_kwta.gp_i)
	  gp_g_i *= gp_kwta.gp_g;
	lay->i_val.g_i = MAX(lay->i_val.g_i, gp_g_i);
      }
    }
    else if(inhib_group == LAY_AND_GPS) {
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	Compute_Inhib_impl(lay, rugp, (LeabraInhib*)rugp, net);
      }
    }
    Compute_LayInhibToGps(lay, net);
  }
}

void LeabraLayerSpec::Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork*) {
  if(lay->units.gp.size == 0) return;

  if(inhib_group == ENTIRE_LAYER) {
    // propagate g_i to all subgroups even if doing ENTIRE_LAYER
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i = lay->i_val.g_i;
    }
  }
  else if(inhib_group == UNIT_GROUPS) {
    if(gp_kwta.gp_i) {	// linking groups: get max from layer
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	rugp->i_val.gp_g_i = lay->i_val.g_i;
	rugp->i_val.g_i = MAX(rugp->i_val.g_i, lay->i_val.g_i);
      }
    }
  }
  else if(inhib_group == LAY_AND_GPS) {
    // actual inhibition is max of layer and group
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i = MAX(rugp->i_val.g_i, lay->i_val.g_i);
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  if(compute_i == UNIT_INHIB) {
    thr->i_val.g_i = 0.0f;	// make sure it's zero, cuz this gets added to units.. 
  }
  else {
    if(compute_i == KWTA_INHIB)
      Compute_Inhib_kWTA(lay, ug, thr, net);
    else if(compute_i == KWTA_AVG_INHIB)
      Compute_Inhib_kWTA_Avg(lay, ug, thr, net);
    else if(compute_i == AVG_MAX_PT_INHIB)
      Compute_Inhib_AvgMaxPt(lay, ug, thr, net);
    thr->i_val.g_i = thr->i_val.kwta;
  }

  thr->i_val.g_i_orig = thr->i_val.g_i;	// retain original values..
}

void LeabraLayerSpec::Compute_Inhib_kWTA(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(i_kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k + 1;	// expand cutoff to include N+1th one
  k_plus_1 = MIN(ug->leaves,k_plus_1);

  float net_k1 = FLT_MAX;
  int k1_idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  int j;
  if(thr->active_buf.size != k_plus_1) { // need to fill the sort buf..
    thr->active_buf.size = 0;
    j = 0;
    for(u = (LeabraUnit*)ug->FirstEl(i); u && (j < k_plus_1);
	u = (LeabraUnit*)ug->NextEl(i), j++)
    {
      thr->active_buf.Add(u);		// add unit to the list
      if(u->i_thr < net_k1) {
	net_k1 = u->i_thr;	k1_idx = j;
      }
    }
    thr->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; u; u = (LeabraUnit*)ug->NextEl(i)) {
      if(u->i_thr <=  net_k1) {	// not bigger than smallest one in sort buffer
	thr->inact_buf.Add(u);
	continue;
      }
      thr->inact_buf.Add(thr->active_buf[k1_idx]); // now inactive
      thr->active_buf.Replace(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      for(j=0; j < k_plus_1; j++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[j]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find net_k1
    for(j=0; j < k_plus_1; j++) { 	// these should be the top ones, very fast!!
      float tmp = thr->active_buf[j]->i_thr;
      if(tmp < net_k1) {
	net_k1 = tmp;		k1_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < thr->inact_buf.size; j++) {
      u = thr->inact_buf[j];
      if(u->i_thr <=  net_k1)		// not bigger than smallest one in sort buffer
	continue;
      thr->inact_buf.Replace(j, thr->active_buf[k1_idx]);	// now inactive
      thr->active_buf.Replace(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      int i;
      for(i=0; i < k_plus_1; i++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[i]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = i;
	}
      }
    }
  }

  // active_buf now has k+1 most active units, get the next-highest one
  int k_idx = -1;
  float net_k = FLT_MAX;
  for(j=0; j < k_plus_1; j++) {
    float tmp = thr->active_buf[j]->i_thr;
    if((tmp < net_k) && (j != k1_idx)) {
      net_k = tmp;		k_idx = j;
    }
  }
  if(k_idx == -1) {		// we didn't find the next one
    k_idx = k1_idx;
    net_k = net_k1;
  }

  LeabraUnit* k1_u = (LeabraUnit*)thr->active_buf[k1_idx];
  LeabraUnit* k_u = (LeabraUnit*)thr->active_buf[k_idx];

  float k1_i = k1_u->i_thr;
  float k_i = k_u->i_thr;
  thr->kwta.k_ithr = k_i;
  thr->kwta.k1_ithr = k1_i;

  Compute_Inhib_BreakTie(thr);

  // place kwta inhibition between k and k+1
  float nw_gi = thr->kwta.k1_ithr + i_kwta_pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Avg(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(i_kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k;	// keep cutoff at k

  float net_k1 = FLT_MAX;
  int k1_idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  int j;
  if(thr->active_buf.size != k_plus_1) { // need to fill the sort buf..
    thr->active_buf.size = 0;
    j = 0;
    for(u = (LeabraUnit*)ug->FirstEl(i); u && (j < k_plus_1);
	u = (LeabraUnit*)ug->NextEl(i), j++)
    {
      thr->active_buf.Add(u);		// add unit to the list
      if(u->i_thr < net_k1) {
	net_k1 = u->i_thr;	k1_idx = j;
      }
    }
    thr->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; u; u = (LeabraUnit*)ug->NextEl(i)) {
      if(u->i_thr <=  net_k1) {	// not bigger than smallest one in sort buffer
	thr->inact_buf.Add(u);
	continue;
      }
      thr->inact_buf.Add(thr->active_buf[k1_idx]); // now inactive
      thr->active_buf.Replace(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      for(j=0; j < k_plus_1; j++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[j]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find net_k1
    for(j=0; j < k_plus_1; j++) { 	// these should be the top ones, very fast!!
      float tmp = thr->active_buf[j]->i_thr;
      if(tmp < net_k1) {
	net_k1 = tmp;		k1_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < thr->inact_buf.size; j++) {
      u = thr->inact_buf[j];
      if(u->i_thr <=  net_k1)		// not bigger than smallest one in sort buffer
	continue;
      thr->inact_buf.Replace(j, thr->active_buf[k1_idx]);	// now inactive
      thr->active_buf.Replace(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      int i;
      for(i=0; i < k_plus_1; i++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[i]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = i;
	}
      }
    }
  }

  // active_buf now has k most active units, get averages of both groups
  float k_avg = 0.0f;
  for(j=0; j < k_plus_1; j++)
    k_avg += thr->active_buf[j]->i_thr;
  k_avg /= (float)k_plus_1;

  float oth_avg = 0.0f;
  for(j=0; j < thr->inact_buf.size; j++)
    oth_avg += thr->inact_buf[j]->i_thr;
  if(thr->inact_buf.size > 0)
    oth_avg /= (float)thr->inact_buf.size;

  // place kwta inhibition between two averages
  // this uses the adapting point!
  float pt = i_kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;

  Compute_Inhib_BreakTie(thr);

  float nw_gi = thr->kwta.k1_ithr + pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_BreakTie(LeabraInhib* thr) {
  thr->kwta.ithr_diff = (thr->kwta.k_ithr - thr->kwta.k1_ithr) / thr->kwta.k_ithr;
  thr->kwta.tie_brk = 0;
  if(tie_brk.on && (thr->kwta.k_ithr > tie_brk.k_thr)) {
    if(thr->kwta.ithr_diff < tie_brk.diff_thr) {
      // we now have an official tie: break it by reducing firing of "others"
      thr->kwta.k1_ithr = (1.0f - tie_brk.diff_thr) * thr->kwta.k_ithr;
      thr->kwta.tie_brk = 1;
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Gps(LeabraLayer* lay, LeabraNetwork* net) {
  // computing the top *groups*, not units here!
  int k_plus_1 = lay->kwta.k;	// only get top k

  float net_k1 = FLT_MAX;
  int k1_idx = 0;
  LeabraUnit_Group* u;
  int i;
  int j;
  if(lay->active_buf.size != k_plus_1) { // need to fill the sort buf..
    lay->active_buf.size = 0;
    for(i = 0; i < k_plus_1; i++) {
      u = (LeabraUnit_Group*)lay->units.gp[i], 
      lay->active_buf.Add((LeabraUnit*)u);		// add unit to the list
      if(u->i_val.g_i < net_k1) {
	net_k1 = u->i_val.g_i;	k1_idx = i;
      }
    }
    lay->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; i<lay->units.gp.size; i++) {
      u = (LeabraUnit_Group*)lay->units.gp[i];
      if(u->i_val.g_i <=  net_k1) {	// not bigger than smallest one in sort buffer
	lay->inact_buf.Add((LeabraUnit*)u);
	continue;
      }
      lay->inact_buf.Add(lay->active_buf[k1_idx]); // now inactive
      lay->active_buf.Replace(k1_idx, (LeabraUnit*)u);// replace the smallest with it
      net_k1 = u->i_val.g_i;		// assume its the smallest
      for(j=0; j < k_plus_1; j++) { 	// and recompute the actual smallest
	float tmp = ((LeabraUnit_Group*)lay->active_buf[j])->i_val.g_i;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find net_k1
    for(j=0; j < k_plus_1; j++) { 	// these should be the top ones, very fast!!
      float tmp = ((LeabraUnit_Group*)lay->active_buf[j])->i_val.g_i;
      if(tmp < net_k1) {
	net_k1 = tmp;		k1_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < lay->inact_buf.size; j++) {
      u = (LeabraUnit_Group*)lay->inact_buf[j];
      if(u->i_val.g_i <=  net_k1)		// not bigger than smallest one in sort buffer
	continue;
      lay->inact_buf.Replace(j, lay->active_buf[k1_idx]);	// now inactive
      lay->active_buf.Replace(k1_idx, (LeabraUnit*)u);// replace the smallest with it
      net_k1 = u->i_val.g_i;		// assume its the smallest
      int i;
      for(i=0; i < k_plus_1; i++) { 	// and recompute the actual smallest
	float tmp = ((LeabraUnit_Group*)lay->active_buf[i])->i_val.g_i;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = i;
	}
      }
    }
  }

  // active_buf now has k most active groups - go through groups and set large inhib in 
  // inactive ones!
  for(j=0; j < lay->inact_buf.size; j++) {
    u = (LeabraUnit_Group*)lay->inact_buf[j];
    // set inhib to more than what is needed to keep *max* unit below threshold..
    LeabraUnit* un = (LeabraUnit*)u->Leaf(0);// get first unit 
    float tnet = un->net;
    un->net = u->netin.max;	// set to max
    u->i_val.g_i = 1.2f * un->Compute_IThresh(lay, net);
    un->net = tnet;
  }
}

void LeabraLayerSpec::Compute_Inhib_AvgMaxPt(LeabraLayer* lay, Unit_Group*, LeabraInhib* thr, LeabraNetwork*) {
  float pt = i_kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  else if((inhib_group != ENTIRE_LAYER) && ((LeabraInhib*)lay != thr))
    pt = gp_i_pt;		// use sub-group version for sub-groups..
  
  float oth_avg = thr->i_thrs.avg; // put between the avg
  float k_avg = thr->i_thrs.max; // and the max..

  float nw_gi = oth_avg + pt * (k_avg - oth_avg);
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;
  thr->kwta.Compute_IThrR();
}


//////////////////////////////////////////
//	Stage 3.5: Inhib Avg	 	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if(lay->units.gp.size > 0) {	// even if ENTIRE_LAYER, do it by sub-group to get stats..
    lay->un_g_i.avg = 0.0f;
    lay->un_g_i.max = -FLT_MAX;
    lay->un_g_i.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_InhibAvg_impl(lay, rugp, (LeabraInhib*)rugp, net);
      lay->un_g_i.avg += rugp->un_g_i.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->un_g_i.max > lay->un_g_i.max) {
	lay->un_g_i.max = rugp->un_g_i.max;
	lay->un_g_i.max_i = g;
      }
    }
  }
  else {
    Compute_InhibAvg_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_InhibAvg_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  // now actually go through and set the gc.i for each unit
  thr->un_g_i.avg = 0.0f;
  thr->un_g_i.max = -FLT_MAX;
  thr->un_g_i.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_InhibAvg(lay, thr, net);
    thr->un_g_i.avg += u->gc.i;
    if(u->gc.i > thr->un_g_i.max) {
      thr->un_g_i.max = u->gc.i;
      thr->un_g_i.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0)
    thr->un_g_i.avg /= (float)ug->leaves;	// turn it into an average
}

//////////////////////////////////////////
//	Stage 4: the final activation 	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_ActAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  thr->acts.avg = 0.0f;
  thr->acts.max = -FLT_MAX;
  thr->acts.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts.avg += u->act_eq;
    if(u->act_eq > thr->acts.max) {
      thr->acts.max = u->act_eq;  thr->acts.max_i = lf;
    }
    u->Compute_MaxDa(lay, thr, net);
    lf++;
  }
  if(ug->leaves > 0) thr->acts.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts.avg = 0.0f;
    lay->acts.max = -FLT_MAX;
    lay->acts.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts.avg += rugp->acts.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts.max > lay->acts.max) {
	lay->acts.max = rugp->acts.max; lay->acts.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts_m.avg += u->act_m;
    if(u->act_m > thr->acts_m.max) {
      thr->acts_m.max = u->act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_m.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActMAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts_m.avg = 0.0f;
    lay->acts_m.max = -FLT_MAX;
    lay->acts_m.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActMAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts_m.avg += rugp->acts_m.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts_m.max > lay->acts_m.max) {
	lay->acts_m.max = rugp->acts_m.max; lay->acts_m.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts_m.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActMAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts_p.avg += u->act_p;
    if(u->act_p > thr->acts_p.max) {
      thr->acts_p.max = u->act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_p.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActPAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts_p.avg = 0.0f;
    lay->acts_p.max = -FLT_MAX;
    lay->acts_p.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActPAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts_p.avg += rugp->acts_p.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts_p.max > lay->acts_p.max) {
	lay->acts_p.max = rugp->acts_p.max; lay->acts_p.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts_p.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActPAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_Act_impl(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_Act_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }

  Compute_ActAvg(lay, net);
  Compute_NetRescale(lay, net);
  if(lay->ext_flag & Unit::TARG) {
    net->trg_max_act = MAX(net->trg_max_act, lay->acts.max);
  }
}

void LeabraLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_Act(lay,thr,net);
  }
}

void LeabraLayerSpec::Compute_NetRescale(LeabraLayer* lay, LeabraNetwork* net) {
  if(!net_rescale.on) return;
  if(lay->netin.max <= net_rescale.max_net) return;
  float old_scale = lay->net_rescale;
  lay->net_rescale *= net_rescale.max_net / (lay->netin.max + net_rescale.net_extra);
  float new_scale = lay->net_rescale / old_scale;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_NetRescale(lay, net, new_scale);
  }
}

//////////////////////////////////////////
//	Stage 5: Between Events 	//
//////////////////////////////////////////

void LeabraLayerSpec::PhaseInit(LeabraLayer* lay, LeabraNetwork* net) {
  lay->net_rescale = 1.0f;
  if(lay->ext_flag & Unit::TARG) {	// only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      lay->SetExtFlag(Unit::EXT);
  }
  else {
    if(clamp.hard) return;	// not target and hard-clamped -- bail
  }
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->PhaseInit(lay, net);
  }
}

void LeabraLayerSpec::DecayEvent(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayEvent(lay, net, decay.event);
  }
}
  
void LeabraLayerSpec::DecayPhase(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayPhase(lay, net, decay.phase);
  }
}

void LeabraLayerSpec::DecayPhase2(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayPhase(lay, net, decay.phase2);
  }
}
  
void LeabraLayerSpec::ExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::EXT))	// only process ext
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->ExtToComp(lay, net);
}

void LeabraLayerSpec::TargExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::TARG_EXT))	// only process w/ external input
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->TargExtToComp(lay, net);
}

void LeabraLayerSpec::AdaptGBarI(LeabraLayer* lay, LeabraNetwork*) {
  float diff = lay->kwta.pct - lay->acts.avg;
  if(fabs(diff) > adapt_i.tol) {
    float p_i = 1.0f;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      p_i = 1.0f - adapt_i.l;
    }
    lay->adapt_i.g_bar_i -= p_i * adapt_i.p_dt * diff;
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
    float minv = us->g_bar.i * (1.0 - adapt_i.mx_d);
    float maxv = us->g_bar.i * (1.0 + adapt_i.mx_d);
    if(lay->adapt_i.g_bar_i < minv) lay->adapt_i.g_bar_i = minv;
    if(lay->adapt_i.g_bar_i > maxv) lay->adapt_i.g_bar_i = maxv;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      lay->adapt_i.g_bar_l -= adapt_i.l * adapt_i.p_dt * diff;
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
      float minv = us->g_bar.l * (1.0 - adapt_i.mx_d);
      float maxv = us->g_bar.l * (1.0 + adapt_i.mx_d);
      if(lay->adapt_i.g_bar_l < minv) lay->adapt_i.g_bar_l = minv;
      if(lay->adapt_i.g_bar_l > maxv) lay->adapt_i.g_bar_l = maxv;
    }
  }
}

void LeabraLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both) {
  if(set_both) {
    lay->acts_m = lay->acts;
    lay->acts_p = lay->acts;
    lay->phase_dif_ratio = 1.0f;

    if(lay->units.gp.size > 0) {
      int g;
      for(g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	rugp->acts_m = rugp->acts;
	rugp->acts_p = rugp->acts;
	rugp->phase_dif_ratio = 1.0f;
      }
    }
  }
  else {
    if(net->phase == LeabraNetwork::MINUS_PHASE)
      lay->acts_m = lay->acts;
    else
      lay->acts_p = lay->acts;
    if(lay->acts_p.avg > 0.0f)
      lay->phase_dif_ratio = lay->acts_m.avg / lay->acts_p.avg;
    else
      lay->phase_dif_ratio = 1.0f;

    if(lay->units.gp.size > 0) {
      int g;
      for(g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	if(net->phase == LeabraNetwork::MINUS_PHASE)
	  rugp->acts_m = rugp->acts;
	else
	  rugp->acts_p = rugp->acts;
	if(rugp->acts_p.avg > 0.0f)
	  rugp->phase_dif_ratio = rugp->acts_m.avg / rugp->acts_p.avg;
	else
	  rugp->phase_dif_ratio = 1.0f;
      }
    }
  }

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      LeabraUnit* u;
      taLeafItr i;
      FOR_ITR_EL(LeabraUnit, u, rugp->, i)
	u->PostSettle(lay, (LeabraInhib*)rugp, net, set_both);
    }
  }
  else {
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i)
      u->PostSettle(lay, (LeabraInhib*)lay, net, set_both);
  }

  if((adapt_i.type == AdaptISpec::G_BAR_I) || (adapt_i.type == AdaptISpec::G_BAR_IL)) {
    AdaptGBarI(lay, net);
  }

  lay->prv_phase = net->phase;	// record previous phase
}

//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraLayerSpec::AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork*) {
  if((lay->ext_flag & Unit::EXT) && !(lay->ext_flag & Unit::TARG))
    return;			// don't adapt points for input-only layers
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      // use minus phase activations to adapt
      rugp->adapt_i.avg_avg += adapt_i.a_dt * (rugp->acts_m.avg - rugp->adapt_i.avg_avg);
      float dif = rugp->adapt_i.avg_avg - rugp->kwta.pct;
      if(dif < -adapt_i.tol) {	// average is less than target
	// so reduce the point towards lower value
	rugp->adapt_i.i_kwta_pt += adapt_i.p_dt * 
	  ((i_kwta_pt - adapt_i.mx_d) - rugp->adapt_i.i_kwta_pt);
      }
      else if(dif > adapt_i.tol) {	// average is more than target
	// so increase point towards higher value
	rugp->adapt_i.i_kwta_pt += adapt_i.p_dt * 
	  ((i_kwta_pt + adapt_i.mx_d) - rugp->adapt_i.i_kwta_pt);
      }
    }
  }
  lay->adapt_i.avg_avg += adapt_i.a_dt * (lay->acts_m.avg - lay->adapt_i.avg_avg);
  float dif = lay->adapt_i.avg_avg - lay->kwta.pct;
  if(dif < -adapt_i.tol) {	// average is less than target
    // so reduce the point towards lower value
    lay->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((i_kwta_pt - adapt_i.mx_d) - lay->adapt_i.i_kwta_pt);
  }
  else if(dif > adapt_i.tol) {	// average is more than target
    // so increase point towards higher value
    lay->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((i_kwta_pt + adapt_i.mx_d) - lay->adapt_i.i_kwta_pt);
  }
}

void LeabraLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_dWt(lay, net);
  AdaptKWTAPt(lay, net);
}

void LeabraLayerSpec::Compute_WtFmLin(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_WtFmLin(lay, net);
}

void LeabraLayer::UpdateWeights() {
  Layer::UpdateWeights();
}

//////////////////////////
// 	LeabraLayer	//
//////////////////////////
  
void AvgMaxVals::Initialize() {
  avg = max = 0.0f; max_i = -1;
}

void AvgMaxVals::Copy_(const AvgMaxVals& cp) {
  avg = cp.avg; max = cp.max; max_i = cp.max_i;
}

void KWTAVals::Initialize() {
  k = 12;
  pct = .25f;
  pct_c = .75f;
  adth_k = 1;
  k_ithr = 0.0f;
  k1_ithr = 0.0f;
  ithr_r = 0.0f;
  ithr_diff = 0.0f;
  tie_brk = 0;
}

void KWTAVals::Copy_(const KWTAVals& cp) {
  k = cp.k;
  pct = cp.pct;
  pct_c = cp.pct_c;
  adth_k = cp.adth_k;
  k_ithr = cp.k_ithr;
  k1_ithr = cp.k1_ithr;
  ithr_r = cp.ithr_r;
  ithr_diff = cp.ithr_diff;
  tie_brk = cp.tie_brk;
}

void KWTAVals::Compute_Pct(int n_units) {
  if(n_units > 0)
    pct = (float)k / (float)n_units;
  else
    pct = 0.0f;
  pct_c = 1.0f - pct;
}

void KWTAVals::Compute_IThrR() {
  if(k1_ithr <= 0.0f)
    ithr_r = 0.0f;
  else 
    ithr_r = logf(k_ithr / k1_ithr);
}

void AdaptIVals::Initialize() {
  avg_avg = 0.0f;
  i_kwta_pt = 0.0f;
  g_bar_i = 1.0f;
  g_bar_l = .1f;
}

void AdaptIVals::Copy_(const AdaptIVals& cp) {
  avg_avg = cp.avg_avg;
  i_kwta_pt = cp.i_kwta_pt;
  g_bar_i = cp.g_bar_i;
  g_bar_l = cp.g_bar_l;
}

void InhibVals::Initialize() {
  kwta = 0.0f;
  g_i = 0.0f;
  gp_g_i = 0.0f;
  g_i_orig = 0.0f;
}

void InhibVals::Copy_(const InhibVals& cp) {
  kwta = cp.kwta;
  g_i = cp.g_i;
  gp_g_i = cp.gp_g_i;
  g_i_orig = cp.g_i_orig;
}

void LeabraInhib::Inhib_Initialize() {
  kwta.k = 1;
  kwta.pct = .25;
  kwta.pct_c = .75;
  i_val.Initialize();
  phase_dif_ratio = 1.0f;
}

void LeabraInhib::Inhib_InitState(LeabraLayerSpec*) {
  i_val.Initialize();
  netin.Initialize();
  i_thrs.Initialize();
  acts.Initialize();
  un_g_i.Initialize();
}

void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  units.SetBaseType(&TA_LeabraUnit);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Inhib_Initialize();
  stm_gain = .5f;
  hard_clamped = false;
  prv_phase = LeabraNetwork::MINUS_PHASE;
  dav = 0.0f;
  da_updt = false;
  net_rescale = 1.0f;
}  

void LeabraLayer::InitLinks() {
  Layer::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);

  taBase::Own(misc_iar, this);
  spec.SetDefaultSpec(this);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
}

void LeabraLayer::CutLinks() {
  Layer::CutLinks();
  spec.CutLinks();
  misc_iar.CutLinks();
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  netin = cp.netin;
  i_thrs = cp.i_thrs;
  acts = cp.acts;
  acts_p = cp.acts_p;
  acts_m = cp.acts_m;
  phase_dif_ratio = cp.phase_dif_ratio;
  kwta = cp.kwta;
  i_val = cp.i_val;
  un_g_i = cp.un_g_i;
}

void LeabraLayer::Copy_(const LeabraLayer& cp) {
  Inhib_Copy_(cp);
  spec = cp.spec;
  stm_gain = cp.stm_gain;
  hard_clamped = cp.hard_clamped;
  misc_iar = cp.misc_iar;
}

void LeabraLayer::ResetSortBuf() {
  Inhib_ResetSortBuf();		// reset sort buf after any edit..
  if(units.gp.size > 0) {
    int g;
    for(g=0; g<units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)units.gp[g];
      rugp->Inhib_ResetSortBuf();
    }
  }
}

void LeabraLayer::UpdateAfterEdit() {
  Layer::UpdateAfterEdit();
  spec.CheckSpec();
  ResetSortBuf();
}

bool LeabraLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)	return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((LeabraLayerSpec*)sp);
  else
    return false;
  return true;
} 

bool LeabraLayer::CheckTypes() {
  if(!spec.CheckSpec()) return false;
  return Layer::CheckTypes();
}

void LeabraLayer::Build() {
  ResetSortBuf();
  Layer::Build();
}

void LeabraUnit_Group::Initialize() {
  Inhib_Initialize();
  misc_state = 0;
  misc_state1 = 0;
  misc_state2 = 0;
}

void LeabraUnit_Group::InitLinks() {
  Unit_Group::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);
}

void LeabraUnit_Group::Copy_(const LeabraUnit_Group& cp) {
  Inhib_Copy_(cp);
  misc_state = cp.misc_state;
  misc_state1 = cp.misc_state1;
  misc_state2 = cp.misc_state2;
}


//////////////////////////
//  	Network		//
//////////////////////////

void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);

  phase_order = MINUS_PLUS;
  no_plus_test = true;
  trial_init = DECAY_STATE;
  sequence_init = DO_NOTHING;
  first_plus_dwt = ONLY_FIRST_DWT;
  phase = MINUS_PHASE;
  phase_no = 0;
  phase_max = 2;

  cycle_max = 60;
  min_cycles = 15;
  min_cycles_phase2 = 15;
  netin_mod = 1;
  send_delta = false;

  maxda_stopcrit = .005f;
  maxda = 0.0f;
  
  trg_max_act_stopcrit = 1.0f;	// disabled
  trg_max_act = 0.0f;

  ext_rew = 0.0f;
  avg_ext_rew = 0.0f;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::InitCounters() {
  inherited::InitCounters();
  phase = MINUS_PHASE;
  phase_no = 0;
}

void LeabraNetwork::InitStats() {
  inherited::InitStats();
  maxda = 0.0f;
  trg_max_act = 0.0f;
  ext_rew = 0.0f;
  avg_ext_rew = 0.0f;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_LeabraCon;
  prjn->con_gp_type = &TA_LeabraCon_Group;
  prjn->con_spec.SetBaseType(&TA_LeabraConSpec);
}

//////////////////////////////////
// 	Cycle-Level Functions	//
//////////////////////////////////

void LeabraNetwork::Compute_Net() {
  InitDelta();		// this is done first because of sender-based net
  if(send_delta) {
    LeabraLayer* l;
    taLeafItr i;
    FOR_ITR_EL(LeabraLayer, l, layers., i) {
      if(!l->lesion)
	l->Send_NetDelta();
    }
#ifdef DMEM_COMPILE
    dmem_share_units.Sync(3);
#endif
  }
  else {
    Send_Net();
  }
}

void LeabraNetwork::Compute_Clamp_NetAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    lay->Compute_Clamp_NetAvg(this);
  }
}

void LeabraNetwork::Compute_Inhib() {
  bool do_lay_gp = false;
  if(layers.gp.size == 0) do_lay_gp = true; // falsely set to true to prevent further checking
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    lay->Compute_Inhib(this);
    if(!do_lay_gp) {
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.spec;
      if(laysp->kwta.gp_i) do_lay_gp = true;
    }
  }
  if(layers.gp.size == 0) do_lay_gp = false; // now override anything
  if(do_lay_gp) {
    for(int lgi = 0; lgi < layers.gp.size; lgi++) {
      Layer_Group* lg = (Layer_Group*)layers.gp[lgi];
      float lay_gp_g_i = 0.0f;
      for(int li = 0; li < lg->size; li++) {
	lay = (LeabraLayer*)lg->FastEl(li);
	LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.spec;
	if(lay->lesion || !laysp->kwta.gp_i) continue;
	float lay_val = laysp->kwta.gp_g * lay->i_val.g_i;
	lay_gp_g_i = MAX(lay_val, lay_gp_g_i);
      }
      if(lay_gp_g_i > 0.0f) {	// actually got something
	for(int li = 0; li < lg->size; li++) {
	  lay = (LeabraLayer*)lg->FastEl(li);
	  LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.spec;
	  if(lay->lesion || !laysp->kwta.gp_i) continue;
	  lay->i_val.gp_g_i = lay_gp_g_i;
	  if(laysp->kwta.gp_g > 1.0)
	    lay->i_val.gp_g_i /= laysp->kwta.gp_g; // if > 1, need to renorm otherwise its own inhib will cancel itself out!!
	  lay->i_val.g_i = MAX(lay->i_val.gp_g_i, lay->i_val.g_i);

	  if(lay->units.gp.size > 0) {
	    lay->Compute_LayInhibToGps(this);
	  }
	}
      }
    }
  }
}

void LeabraNetwork::Compute_InhibAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    lay->Compute_InhibAvg(this);
  }
}

void LeabraNetwork::Compute_Act() {
  maxda = 0.0f;		// initialize
  trg_max_act = 0.0f;
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    lay->Compute_Act(this);
  }
}

void LeabraNetwork::Cycle_Run() {
  if((cycle % netin_mod) == 0) {
    Compute_Net();
    Compute_Clamp_NetAvg();
    Compute_Inhib();
    Compute_InhibAvg();
  }
  Compute_Act();
}

//////////////////////////////////
// 	Settle-Level Functions	//
//////////////////////////////////

void LeabraNetwork::Compute_Active_K() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    lay->Compute_Active_K();	// this gets done at the outset..
  }
}
  
void LeabraNetwork::DecayEvent() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->DecayEvent(this);
  }
}

void LeabraNetwork::DecayPhase() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->DecayPhase(this);
  }
}

void LeabraNetwork::DecayPhase2() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->DecayPhase2(this);
  }
}

void LeabraNetwork::PhaseInit() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->PhaseInit(this);
  }
}

void LeabraNetwork::ExtToComp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->ExtToComp(this);
  }
}

void LeabraNetwork::TargExtToComp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->TargExtToComp(this);
  }
}

void LeabraNetwork::Compute_HardClamp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->Compute_HardClamp(this);
  }
}

void LeabraNetwork::Compute_NetScale() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->Compute_NetScale(this);
  }
}

void LeabraNetwork::Send_ClampNet() {
  LeabraLayer* lay;
  taLeafItr l;
  // first init
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->Init_ClampNet(this);
  }
  // then send
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->Send_ClampNet(this);
  }
#ifdef DMEM_COMPILE
  if(send_delta)
    dmem_share_units.Sync(4);
#endif
}

void LeabraNetwork::PostSettle() {
  bool set_both = false;
  if((phase_order == PLUS_ONLY) || (no_plus_test && (context == TEST))) {
    set_both = true;
  }
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->PostSettle(this, set_both);
  }
}

void LeabraNetwork::PostSettle_NStdLay() {
  bool set_both = false;
  if((phase_order == PLUS_ONLY) || (no_plus_test && (context == TEST))) {
    set_both = true;
  }
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion && (lay->spec.spec->GetTypeDef() != &TA_LeabraLayerSpec))
      lay->PostSettle(this, set_both);
  }
}

void LeabraNetwork::Settle_Init() {
  int tmp_cycle = cycle;
  cycle = -2;			// special signal for settle init

  if(phase_no >= 3) { // second plus phase or more: use phase2..
    DecayPhase2();
  }
  else if(phase_no == 2) {
    DecayPhase2();		// decay before 2nd phase set
    if(phase_order == MINUS_PLUS_NOTHING) {
      TargExtToComp();		// all external input is now 'comparison'
    }
  }
  else if(phase_no == 1) {
    if(phase_order == PLUS_NOTHING) { // actually a nothing phase
      DecayPhase2();
      TargExtToComp();
    }
    else
      DecayPhase();		    // prepare for next phase
  }

  Compute_Active_K();		// compute here because could depend on pat_n
  PhaseInit();
  
  Compute_HardClamp();		// first clamp all hard-clamped input acts
  Compute_NetScale();		// and then compute net scaling
  Send_ClampNet();		// and send net from clamped inputs
  cycle = tmp_cycle;
}	

void LeabraNetwork::Settle_Final() {
  // compute weight changes at end of second settle..
  PostSettle();
  if((phase_order == MINUS_PLUS_NOTHING) && (phase_no == 1)) {
    if(context != TEST)
      Compute_dWt();
  }
  else if((phase_order == MINUS_PLUS_PLUS) && (phase_no == 1)) {
    if(context != TEST) {
      if(first_plus_dwt == NO_FIRST_DWT)
	Compute_dWt_NStdLay();	// only do non-standard ones
      else if(first_plus_dwt == ALL_DWT)
	Compute_dWt();
      else if(first_plus_dwt == ONLY_FIRST_DWT) {
	Compute_dWt();
      }
    }
  }
  else if(phase_order == MINUS_PLUS_2) {
    if(phase_no == 1)
      Compute_dWt();
    else if(phase_no == 2)
      Compute_dWt_NStdLay();	// only do non-standard ones
  }
}
  
//////////////////////////////////
// 	Trial-Level Functions	//
//////////////////////////////////

// todo: currently called in Trial_Init -- could be in a more efficient location, but
// cost is minimal, so not bothering for now..
void LeabraNetwork::SetCurLrate() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->SetCurLrate(this, epoch);
  }
}

void LeabraNetwork::DecayState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesion)
      lay->DecayEvent(this);
  }
}

void LeabraNetwork::EncodeState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)     continue;
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i)
      u->EncodeState(lay, this);
  }
}

void LeabraNetwork::Compute_dWt() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    lay->Compute_dWt(this);
  }
}

void LeabraNetwork::Compute_dWt_NStdLay() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    if(lay->spec.spec->GetTypeDef() != &TA_LeabraLayerSpec)
      lay->Compute_dWt(this);
  }
}

void LeabraNetwork::Trial_Init() {
  SetCurLrate();		// todo: this is excessive but not clear where else to call it..

  cycle = -1;
  phase = MINUS_PHASE;
  phase_max = 2;
  bool is_testing = false;

  if(no_plus_test && (context == TEST)) {
    phase_max = 1;		// just do one loop (the minus phase)
    is_testing = true;
  }

  // todo: this is now the responsibility of the prog
//   if(cur_event != NULL) {
//     if(cur_event->spec->InheritsFrom(TA_PhaseOrderEventSpec)) {
//       PhaseOrderEventSpec* es = (PhaseOrderEventSpec*)cur_event->spec.spec;
//       if(es->phase_order == PhaseOrderEventSpec::MINUS_PLUS)
// 	phase = MINUS_PHASE;
//       else if(es->phase_order == PhaseOrderEventSpec::MINUS_ONLY) {
// 	phase = MINUS_PHASE;
// 	phase_no.SetMax(1);
//       }
//       else if(es->phase_order == PhaseOrderEventSpec::PLUS_MINUS) {
// 	phase = PLUS_PHASE;
// 	if(is_testing)
// 	  phase_no.SetMax(2);	// need to present plus phase first, then minus..
//       }
//       else if(es->phase_order == PhaseOrderEventSpec::PLUS_ONLY) {
// 	phase = PLUS_PHASE;
// 	phase_no.SetMax(1);
//       }
//     }
//   }

  if(!is_testing) {
    if(phase_order == PLUS_ONLY) {
      phase_max = 1;
      phase = PLUS_PHASE;
    }
    else if((phase_order == MINUS_PLUS_NOTHING) || (phase_order == MINUS_PLUS_PLUS)) {
      phase_max = 3;
    }
    else if(phase_order == MINUS_PLUS_2) {
      phase_max = 4;
    }
    else if(phase_order == PLUS_NOTHING) {
      phase_max = 2;
      phase = PLUS_PHASE;
    }
  }

  // todo: this seems kinda silly and should perhaps just be put in the prog: not worth the extra state vars
  if(trial_init == INIT_STATE)
    InitState();
  else if(trial_init == DECAY_STATE)
    DecayState();
}

void LeabraNetwork::Trial_UpdatePhase() {
  // todo: prog needs to deal
//   if((cur_event != NULL) && (cur_event->spec->InheritsFrom(TA_PhaseOrderEventSpec))) {
//     PhaseOrderEventSpec* es = (PhaseOrderEventSpec*)cur_event->spec.spec;
//     if(es->phase_order == PhaseOrderEventSpec::MINUS_PLUS)
//       phase = PLUS_PHASE;
//     else if(es->phase_order == PhaseOrderEventSpec::PLUS_MINUS)
//       phase = MINUS_PHASE;
//   }
//   else {

  if(phase_order == PLUS_NOTHING) {
    phase = MINUS_PHASE;
  }
  else {
    if(phase_no == 1)
      phase = PLUS_PHASE;
    else {
      if(phase_order == MINUS_PLUS_NOTHING)
	phase = MINUS_PHASE;
      else if(phase_order == MINUS_PLUS_PLUS)
	phase = PLUS_PHASE;
      else if(phase_order == MINUS_PLUS_2) {
	if(phase_no == 2) phase = MINUS_2;
	else phase = PLUS_2;
      }	    
    }
  }
}

void LeabraNetwork::Trial_Final() {
  if(context != TEST) {
    if((phase_order == MINUS_PLUS_PLUS) && (first_plus_dwt == ONLY_FIRST_DWT))
      Compute_dWt_NStdLay();	// only update the non-standard layers, which will have checks
    else if(phase_order == MINUS_PLUS_2)
      Compute_dWt_NStdLay();	// only update the non-standard layers, which will have checks
    else {
      if(!((phase_max == 1) && (phase == MINUS_PHASE))) // if only minus phase, don't do it!
	Compute_dWt();		// get them all
    }
  }
  EncodeState();
}

void LeabraNetwork::Compute_ExtRew() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesion)	continue;
    if(lay->spec.spec->GetTypeDef() != &TA_ExtRewLayerSpec) continue;
    LeabraUnit* eru = (LeabraUnit*)lay->units.Leaf(0);
    if(eru->misc_1 == 0.0f) { // indication of no reward available
      ext_rew = -1.1f;
    }
    else {
      ext_rew = eru->act_eq;	// just set it!
      avg_ext_rew_sum += ext_rew;
      avg_ext_rew_n++;
    }
  }
}

void LeabraNetwork::Compute_SSE() {
  inherited::Compute_SSE();
  Compute_ExtRew();
}

void LeabraNetwork::Compute_AvgExtRew() {
  if(avg_ext_rew_n > 0) {
    avg_ext_rew = avg_ext_rew_sum / (float)avg_ext_rew_n;
  }
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::Compute_EpochSSE() {
  inherited::Compute_EpochSSE();
  Compute_AvgExtRew();
}

void LeabraProject::Initialize() {
  networks.SetBaseType(&TA_LeabraNetwork);
  wizards.SetBaseType(&TA_LeabraWizard);
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

bool LeabraContextLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;
  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("LeabraContextLayerSpec requires trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  return true;
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

bool ScalarValLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  if(lay->n_units < 3) {
    if(!quiet) taMisc::Error("ScalarValLayerSpec: coarse-coded scalar representation requires at least 3 units, I just set n_units");
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
    compute_i = UNIT_INHIB;
    if(!quiet) taMisc::Error("ScalarValLayerSpec: SUM_BAR rep type requires compute_i = UNIT_INHIB, because it sets gc.i individually");
  }

  if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k = 1;		// localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == ScalarValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
    if(us->hyst.init) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
      if(!quiet) taMisc::Error("ScalarValLayerSpec: bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
    if(us->acc.init) {
      us->SetUnique("acc", true);
      us->acc.init = false;
      if(!quiet) taMisc::Error("ScalarValLayerSpec: bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  if(u == NULL) {
    taMisc::Error("Error: ScalarValLayerSpec: scalar val layer doesn't have any units:", lay->name);
    return false;
  }
    
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(cs->wt_scale.rel > 0.5f) {
	cs->SetUnique("wt_scale", true);
	cs->wt_scale.rel = 0.1f;
	if(!quiet) taMisc::Error("ScalarValLayerSpec: scalar val self connections should have wt_scale < .5, I just set it to .1 for you in spec:",
		      cs->name,"(make sure this is appropriate for all connections that use this spec!)");
      }
      if(cs->lrate > 0.0f) {
	cs->SetUnique("lrate", true);
	cs->lrate = 0.0f;
	if(!quiet) taMisc::Error("ScalarValLayerSpec: scalar val self connections should have lrate = 0, I just set it for you in spec:",
		      cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    else {
      if((scalar.rep == ScalarValSpec::SUM_BAR) && cs->lmix.err_sb) {
	cs->SetUnique("lmix", true);
	cs->lmix.err_sb = false;
	if(!quiet) taMisc::Error("ScalarValLayerSpec: scalar val cons for SUM_BAR should have lmix.err_sb = false (are otherwise biased!), I just set it for you in spec:",
		      cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
  }
  return true;
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

bool TwoDValLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  if(lay->n_units < 3) {
    if(!quiet) taMisc::Error("TwoDValLayerSpec: coarse-coded twod representation requires at least 3 units, I just set n_units");
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
      us->SetUnique("hyst", true);
      us->hyst.init = false;
      if(!quiet) taMisc::Error("TwoDValLayerSpec: bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
    if(us->acc.init) {
      us->SetUnique("acc", true);
      us->acc.init = false;
      if(!quiet) taMisc::Error("TwoDValLayerSpec: bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:",
			       us->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  if(u == NULL) {
    taMisc::Error("Error: TwoDValLayerSpec: twod val layer doesn't have any units:", lay->name);
    return false;
  }
    
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.spec == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(recv_gp->prjn->spec.spec->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(cs->wt_scale.rel > 0.5f) {
	cs->SetUnique("wt_scale", true);
	cs->wt_scale.rel = 0.1f;
	if(!quiet) taMisc::Error("TwoDValLayerSpec: twod val self connections should have wt_scale < .5, I just set it to .1 for you in spec:",
		      cs->name,"(make sure this is appropriate for all connections that use this spec!)");
      }
      if(cs->lrate > 0.0f) {
	cs->SetUnique("lrate", true);
	cs->lrate = 0.0f;
	if(!quiet) taMisc::Error("TwoDValLayerSpec: twod val self connections should have lrate = 0, I just set it for you in spec:",
		      cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return true;
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

/////////////////////////////////////////////////////////////////////////////////////
// 			PFC / BG code
/////////////////////////////////////////////////////////////////////////////////////

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

void DaModUnitSpec::InitLinks() {
  LeabraUnitSpec::InitLinks();
  taBase::Own(da_mod, this);
}

void DaModUnitSpec::Defaults() {
  LeabraUnitSpec::Defaults();
  Initialize();
}

bool DaModUnitSpec::CheckConfig(Unit* un, Layer* lay, Network* net, bool quiet) {
  LeabraUnitSpec::CheckConfig(un, lay, net, quiet);
  return true;
}

void DaModUnitSpec::InitState(LeabraUnit* u, LeabraLayer* lay) {
  LeabraUnitSpec::InitState(u, lay);
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

  LeabraUnitSpec::Compute_Conduct(u, lay, thr, net);
}

void DaModUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(!da_mod.p_dwt) {
    LeabraUnitSpec::Compute_dWt(u, lay, net);
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

void DaModUnitSpec::UpdateWeights(Unit* u) {
  if(!da_mod.p_dwt) {
    LeabraUnitSpec::UpdateWeights(u);
    return;
  }
  DaModUnit* lu = (DaModUnit*)u;
  ((LeabraConSpec*)bias_spec.spec)->B_UpdateWeights((LeabraCon*)u->bias, lu, this);
  if(opt_thresh.updt_wts && 
     ((lu->act_p <= opt_thresh.learn) && (lu->act_m <= opt_thresh.learn)) &&
      ((lu->p_act_p <= opt_thresh.learn) && (lu->p_act_m <= opt_thresh.learn)))
    return;
  UnitSpec::UpdateWeights(lu);
}

void DaModUnitSpec::EncodeState(LeabraUnit* u, LeabraLayer*, LeabraNetwork* net) {
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
  LeabraUnitSpec::DecayEvent(u, lay, net, decay);
  DaModUnit* lu = (DaModUnit*)u;
  lu->dav = 0.0f;
}

void DaModUnitSpec::PostSettle(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr,
			       LeabraNetwork* net, bool set_both)
{
  LeabraUnitSpec::PostSettle(u, lay, thr, net, set_both);
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
  ScalarValLayerSpec::Defaults();
  rew.Initialize();
  avg_rew.Initialize();
  out_err.Initialize();
  Initialize();
}

void ExtRewLayerSpec::InitLinks() {
  ScalarValLayerSpec::InitLinks();
  taBase::Own(rew, this);
  taBase::Own(avg_rew, this);
  taBase::Own(out_err, this);
}

void ExtRewLayerSpec::UpdateAfterEdit() {
  ScalarValLayerSpec::UpdateAfterEdit();
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
  taMisc::Choice(help, "Ok");
  ScalarValLayerSpec::HelpConfig();
}

bool ExtRewLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!ScalarValLayerSpec::CheckConfig(lay, net, quiet))
    return false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("ExtRewLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("ExtRewLayerSpec: must have DaModUnits!");
    return false;
  }

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = true;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("ExtRewLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }
  if(us->act.avg_dt != 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
    if(!quiet) taMisc::Error("ExtRewLayerSpec: requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  bool got_marker = false;
  LeabraLayer* rew_targ_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      if(recv_gp->prjn->from->name == "RewTarg")
	rew_targ_lay = (LeabraLayer*)recv_gp->prjn->from;
      else
	got_marker = true;
      continue;
    }
  }
  if(!got_marker) {
    if(rew_type == DA_REW) {
      taMisc::Error("ExtRewLayerSpec: requires at least one recv MarkerConSpec connection from DA layer",
		    "to get reward based on performance.  This was not found -- please fix!");
      return false;
    }
    else if(rew_type == OUT_ERR_REW) {
      taMisc::Error("ExtRewLayerSpec: requires at least one recv MarkerConSpec connection from output/response layer(s) to compute",
		    "reward based on performance.  This was not found -- please fix!");
      return false;
    }
  }
  if(rew_type == OUT_ERR_REW) {
    if(rew_targ_lay == NULL) {
      taMisc::Error("ExtRewLayerSpec: requires a recv MarkerConSpec connection from layer called RewTarg",
		    "that signals (act > .5) when output error should be used for computing rewards.  This was not found -- please fix!");
      return false;
    }
    if(rew_targ_lay->units.size == 0) {
      taMisc::Error("ExtRewLayerSpec: RewTarg layer must have one unit (has zero) -- please fix!");
      return false;
    }
    int myidx = lay->own_net->layers.FindLeaf(lay);
    int rtidx = lay->own_net->layers.FindLeaf(rew_targ_lay);
    if(rtidx > myidx) {
      taMisc::Error("ExtRewLayerSpec: reward target (RewTarg) layer must be *before* this layer in list of layers -- it is now after, won't work");
      return false;
    }
  }
  return true;
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
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from;
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
  LeabraCon_Group* recv_gp;
  int g;

  // first pass: find the layers: use COMP if no TARG is found
  int	n_targs = 0;		// number of target layers
  int	n_comps = 0;		// number of comp layers
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(!recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec))
      continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from;
    if(rew_lay->name == "RewTarg") continue;

    if(rew_lay->ext_flag & Unit::TARG) n_targs++;
    else if(rew_lay->ext_flag & Unit::COMP) n_comps++;
  }

  int rew_chk_flag = Unit::TARG;
  if(n_targs == 0) rew_chk_flag |= Unit::COMP; // also check comp if no targs!

  float totposs = 0.0f;		// total possible error (unitwise)
  float toterr = 0.0f;		// total error
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(!recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec))
      continue;
    LeabraLayer* rew_lay = (LeabraLayer*)recv_gp->prjn->from;
    if(rew_lay->name == "RewTarg") continue;

    if(!(rew_lay->ext_flag & rew_chk_flag)) continue; // only proceed if valid 

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
  if(out_err.seq_all_cor) {
    taMisc::Error("ExtRewLayerSpec Error: out_err.seq_all_cor is not yet supported!");
    return;
    // todo!  not supported
//     bool old_graded = out_err.graded;
//     out_err.graded = false;		// prevent graded sig here!
//     float itm_er = GetOutErrRew(lay, net);
//     out_err.graded = old_graded;

//     lay->misc_iar.EnforceSize(3); // 0 = addr of eg; 1 = # tot; 2 = # cor
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
  ScalarValLayerSpec::Defaults();
  Initialize();
}

void TDRewPredLayerSpec::InitLinks() {
  ScalarValLayerSpec::InitLinks();
}

void TDRewPredLayerSpec::UpdateAfterEdit() {
  ScalarValLayerSpec::UpdateAfterEdit();
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
  taMisc::Choice(help, "Ok");
  ScalarValLayerSpec::HelpConfig();
}

bool TDRewPredLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!ScalarValLayerSpec::CheckConfig(lay, net, quiet))
    return false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("TDRewPredLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(net->no_plus_test) {
    if(!quiet) taMisc::Error("TDRewPredLayerSpec: requires LeabraNetwork no_plus_test = false, I just set it for you");
    net->no_plus_test = false;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("TDRewPredLayerSpec: must have DaModUnits!");
    return false;
  }

  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("TDRewPredLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = true; // do need prior state dwt
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->prjn->from == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(!cs->InheritsFrom(TA_TDRewPredConSpec)) {
      taMisc::Error("TDRewPredLayerSpec: requires recv connections to be of type TDRewPredConSpec");
      return false;
    }
    if(cs->wt_limits.sym != false) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
      if(!quiet) taMisc::Error("TDRewPredLayerSpec: requires recv connections to have wt_limits.sym=false, I just set it for you in spec:",
		    cs->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
  }
  return true;
}

void TDRewPredLayerSpec::InitState(LeabraLayer* lay) {
  ScalarValLayerSpec::InitState(lay);
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
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
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
  ScalarValLayerSpec::PostSettle(lay, net, set_both); 
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
    lay->InitExterns();
  }
}

void TDRewPredLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  // doing second because act_p is computed only at end of settling!
  // this is better than clamping the value in the middle of everything
  // and then continuing with settling..
  if(net->phase_no < net->phase_max-1)
    return; // only do FINAL dwt!
  ScalarValLayerSpec::Compute_dWt(lay, net);
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
  ScalarValLayerSpec::Defaults();
  Initialize();
}

void TDRewIntegLayerSpec::InitLinks() {
  ScalarValLayerSpec::InitLinks();
  taBase::Own(rew_integ, this);
}

void TDRewIntegLayerSpec::UpdateAfterEdit() {
  ScalarValLayerSpec::UpdateAfterEdit();
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
  taMisc::Choice(help, "Ok");
  ScalarValLayerSpec::HelpConfig();
}

bool TDRewIntegLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!ScalarValLayerSpec::CheckConfig(lay, net, quiet))
    return false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("TDRewIntegLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("TDRewIntegLayerSpec: must have DaModUnits!");
    return false;
  }

  SetUnique("decay", true);
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("TDRewIntegLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* rew_pred_lay = NULL;
  LeabraLayer* ext_rew_lay = NULL;

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->prjn->from == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from;
    LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.spec;
    if(fls->InheritsFrom(&TA_TDRewPredLayerSpec)) {
      rew_pred_lay = flay;
    }
    else if(fls->InheritsFrom(&TA_ExtRewLayerSpec)) {
      ext_rew_lay = flay;
    }
  }

  if(rew_pred_lay == NULL) {
    taMisc::Error("TDRewIntegLayerSpec: requires recv projection from TDRewPredLayerSpec!");
    return false;
  }
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int rpidx = lay->own_net->layers.FindLeaf(rew_pred_lay);
  if(rpidx > myidx) {
    taMisc::Error("TDRewIntegLayerSpec: reward prediction layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  if(ext_rew_lay == NULL) {
    taMisc::Error("TDRewIntegLayerSpec: TD requires recv projection from ExtRewLayerSpec!");
    return false;
  }
  int eridx = lay->own_net->layers.FindLeaf(ext_rew_lay);
  if(eridx > myidx) {
    taMisc::Error("TDRewIntegLayerSpec: external reward layer must be *before* this layer in list of layers -- it is now after, won't work");
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
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(!recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
    LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from;
    LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.spec;
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
  SetUnique("compute_i", true);
  compute_i = KWTA_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .25;
}

void TdLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
}

void TdLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
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
  taMisc::Choice(help, "Ok");
}

bool TdLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("TdLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f)) {
    us->SetUnique("act_range", true);
    us->act_range.max = 2.0f;
    us->act_range.min = -2.0f;
    us->act_range.UpdateAfterEdit();
    if(!quiet) taMisc::Error("TdLayerSpec: requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f)) {
    us->SetUnique("clamp_range", true);
    us->clamp_range.max = 2.0f;
    us->clamp_range.min = -2.0f;
    us->clamp_range.UpdateAfterEdit();
    if(!quiet) taMisc::Error("TdLayerSpec: requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }

  // check recv connection
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraLayer* rewinteg_lay = NULL;
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from;
    if(fmlay == NULL) {
      taMisc::Error("TdLayerSpec: null from layer in recv projection:", (String)g);
      return false;
    }
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)
	&& fmlay->spec.spec->InheritsFrom(TA_TDRewIntegLayerSpec)) {
      rewinteg_lay = fmlay;
      if(recv_gp->size <= 0) {
	taMisc::Error("TdLayerSpec: requires one recv projection with at least one unit!");
	return false;
      }
      if(!recv_gp->Un(0)->InheritsFrom(TA_DaModUnit)) {
	taMisc::Error("TdLayerSpec: I need to receive from a DaModUnit!");
	return false;
      }
    }
  }

  if(rewinteg_lay == NULL) {
    taMisc::Error("TdLayerSpec: did not find TDRewInteg layer to get Td from!");
    return false;
  }

  int myidx = lay->own_net->layers.FindLeaf(lay);
  int rpidx = lay->own_net->layers.FindLeaf(rewinteg_lay);
  if(rpidx > myidx) {
    taMisc::Error("TdLayerSpec: reward integration layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  // check sending layer projections for appropriate unit types
  int si;
  for(si=0;si<lay->send_prjns.size;si++) {
    Projection* prjn = (Projection*)lay->send_prjns[si];
    if(!prjn->from->units.el_typ->InheritsFrom(TA_DaModUnit)) {
      taMisc::Error("TdLayerSpec: all layers I send to must have DaModUnits!, layer:",
		    prjn->from->GetPath(),"doesn't");
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
    LeabraCon_Group* cg = (LeabraCon_Group*)u->recv.gp[ri_prjn_idx];
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
    LeabraCon_Group* send_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesion)	continue;
      int j;
      for(j=0;j<send_gp->size; j++) {
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
    lay->Inhib_SetVals(i_kwta_pt); // assume 0 - 1 clamped inputs
    Compute_ZeroAct(lay, net);	// can't do anything during settle anyway -- just zero it
  }
  else {
    // run "free" in plus phase: compute act = td
    lay->hard_clamped = false;
    lay->UnSetExtFlag(Unit::EXT);
  }
  LeabraLayerSpec::Compute_HardClamp(lay, net);
}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////
// 	PV Con Spec			//
//////////////////////////////////////////

void PVConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("rnd", true);
  rnd.mean = 0.1f;
  rnd.var = 0.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;

  SetUnique("lrate", true);
  lrate = .01f;
  cur_lrate = .01f;

  SetUnique("lrate_sched", true); // not to have any lrate schedule!!
  SetUnique("lrs_value", true); // not to have any lrate schedule!!
  lrs_value = NO_LRS;
}

//////////////////////////////////
//	PV (NAc) Layer Spec	//
//////////////////////////////////

void PVDetectSpec::Initialize() {
  thr_min = 0.2f;
  thr_max = 0.8f;
}

void PViLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = .5f;		// default is no-information case; extrew = .5
}

void PViLayerSpec::Defaults() {
  ScalarValLayerSpec::Defaults();
  Initialize();
}

void PViLayerSpec::InitLinks() {
  ScalarValLayerSpec::InitLinks();
  taBase::Own(pv_detect, this);
}

void PViLayerSpec::UpdateAfterEdit() {
  ScalarValLayerSpec::UpdateAfterEdit();
  pv_detect.UpdateAfterEdit();
}

void PViLayerSpec::HelpConfig() {
  String help = "PViLayerSpec Primary Value (inhibitory) Computation:\n\
 Continuously learns about primary rewards, and cancels (inhibits) them in DA computation.\
 It is always trained on the current primary reward (PVe) value from the ExtRew layer\
 (0 = no reward or none avail).\n\
 - Activation is always expectation of reward for current trial\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nPViLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - All units I recv from must be DaModUnit/Spec units\n\
 - Recv cons from relevant network state layers (as PVConSpec)\n\
 - Marker recv con from PVe/ExtRew layer to get external rewards\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
  ScalarValLayerSpec::HelpConfig();
}

bool PViLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!ScalarValLayerSpec::CheckConfig(lay, net, quiet))
    return false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("PViLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(net->no_plus_test) {
    if(!quiet) taMisc::Error("PViLayerSpec: requires LeabraNetwork no_plus_test = false, I just set it for you");
    net->no_plus_test = false;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("PViLayerSpec: must have DaModUnits!");
    return false;
  }

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("PViLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }
  if((us->opt_thresh.learn >= 0.0f) || us->opt_thresh.updt_wts) {
    if(!quiet) taMisc::Error("PViLayerSpec: UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
			     "I just set it for you in spec:", us->name,
			     "(make sure this is appropriate for all layers that use this spec!)");
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = false; // don't need prior state dwt
  if(us->act.avg_dt != 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
    if(!quiet) taMisc::Error("PViLayerSpec: requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* ext_rew_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->prjn->from == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from;
      LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.spec;
      if(fls->InheritsFrom(TA_ExtRewLayerSpec)) ext_rew_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(!cs->InheritsFrom(TA_PVConSpec)) {
      taMisc::Error("PViLayerSpec: requires recv connections to be of type PVConSpec");
      return false;
    }
  }
  
  if(ext_rew_lay == NULL) {
    taMisc::Error("PViLayerSpec: requires MarkerConSpec connection from PVe/ExtRewLayerSpec layer to get external rewards!");
    return false;
  }
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int eridx = lay->own_net->layers.FindLeaf(ext_rew_lay);
  if(eridx > myidx) {
    taMisc::Error("PViLayerSpec: PVe/ExtRew layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  return true;
}

float PViLayerSpec::Compute_PVe(LeabraLayer* lay, LeabraNetwork*, bool& actual_er_avail, bool& pv_detected) {
  float pve_val = 0.0f;
  actual_er_avail = true;

  int pve_prjn_idx = 0;
  LeabraLayer* pve_lay = FindLayerFmSpec(lay, pve_prjn_idx, &TA_ExtRewLayerSpec);
  if(pve_lay != NULL) {
    LeabraUnit* pveu = (LeabraUnit*)pve_lay->units.Leaf(0);
    pve_val = pveu->act_eq;
    if(pveu->misc_1 == 0.0f) { // indication of no reward available!
      actual_er_avail = false;
    }
  }

  pv_detected = false;
  LeabraUnit* pviu = (LeabraUnit*)lay->units.Leaf(0);
  if((MAX(pviu->act_m, pve_val) > pv_detect.thr_max) ||
     (MIN(pviu->act_m, pve_val) < pv_detect.thr_min))
    pv_detected = true;

  return pve_val;
}

void PViLayerSpec::Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
  }
}

void PViLayerSpec::Compute_PVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = Compute_PVe(lay, net, actual_er_avail, pv_detected);

  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);
     u->ext = pve_val;		// clamp to pve value
     ClampValue(ugp, net); 	// apply new value
     Compute_ExtToPlus(ugp, net); // copy ext values to act_p
     Compute_dWtUgp(ugp, lay, net);
     );
}

void PViLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return; // only do FINAL dwt!
  Compute_PVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
}

//////////////////////////////////////////
// 	LV Con Spec			//
//////////////////////////////////////////

void LVConSpec::Initialize() {
//   min_con_type = &TA_PVLVCon;

  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;

  SetUnique("rnd", true);
  rnd.mean = 0.1f;
  rnd.var = 0.0f;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;

  SetUnique("lrate", true);
  lrate = .05f;
  cur_lrate = .05f;

//   decay = 0.0f;

  syn_dep.depl = 1.1f;
  SetUnique("lrate_sched", true); // not to have any lrate schedule!!
  SetUnique("lrs_value", true); // not to have any lrate schedule!!
  lrs_value = NO_LRS;
}

//////////////////////////////////////////
//	LV Layer Spec: Perceived Value	//
//////////////////////////////////////////

void LVSpec::Initialize() {
  discount = 0.0f;
  use_actual_er = false;
}

void LVeLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  bias_val.un = ScalarValBias::GC;
  bias_val.val = 0.0f;
}

void LVeLayerSpec::Defaults() {
  ScalarValLayerSpec::Defaults();
  Initialize();
}

void LVeLayerSpec::InitLinks() {
  ScalarValLayerSpec::InitLinks();
  taBase::Own(lv, this);
}

void LVeLayerSpec::UpdateAfterEdit() {
  ScalarValLayerSpec::UpdateAfterEdit();
  lv.UpdateAfterEdit();
}

void LVeLayerSpec::HelpConfig() {
  String help = "LVeLayerSpec Learned Value computation (excitatory/fast and inhibitory/slow):\n\
 Learns values (LV) according to the PVLV algorithm: looks at current network state\
 and computes how much it resembles states that have been associated with primary value (PV) in the past\n\
 - Activation is always learned values\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - (training only occurs when primary reward is detected by PVi as either present or expected\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nLVeLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - All units I recv from must be DaModUnit/Spec units\n\
 - Recv cons from relevant network state layers (must be LVConSpec)\n\
 - Marker recv con from PVi layer to get training (PVe, pv_detected) signal\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
  ScalarValLayerSpec::HelpConfig();
}

bool LVeLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!ScalarValLayerSpec::CheckConfig(lay, net, quiet))
    return false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("LVeLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(net->no_plus_test) {
    if(!quiet) taMisc::Error("LVeLayerSpec: requires LeabraNetwork no_plus_test = false, I just set it for you");
    net->no_plus_test = false;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("LVeLayerSpec: must have DaModUnits!");
    return false;
  }

  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("LVeLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }
  if((us->opt_thresh.learn >= 0.0f) || us->opt_thresh.updt_wts) {
    if(!quiet) taMisc::Error("LVeLayerSpec: UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
			     "I just set it for you in spec:", us->name,
			     "(make sure this is appropriate for all layers that use this spec!)");
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = false; // don't need prior state dwt
  us->UpdateAfterEdit();

  // check for conspecs with correct params
  LeabraLayer* pvi_lay = NULL;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    if(recv_gp->prjn->from == recv_gp->prjn->layer) { // self projection, skip it
      continue;
    }
    if(recv_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
      LeabraLayer* flay = (LeabraLayer*)recv_gp->prjn->from;
      LeabraLayerSpec* fls = (LeabraLayerSpec*)flay->spec.spec;
      if(fls->InheritsFrom(TA_PViLayerSpec)) pvi_lay = flay;
      continue;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(!cs->InheritsFrom(TA_LVConSpec)) {
      taMisc::Error("LVeLayerSpec: requires recv connections to be of type LVConSpec");
      return false;
    }
  }
  
  if(pvi_lay == NULL) {
    taMisc::Error("LVeLayerSpec: requires MarkerConSpec connection from PViLayerSpec layer to get DA values!");
    return false;
  }

  return true;
}

void LVeLayerSpec::Compute_ExtToPlus(Unit_Group* ugp, LeabraNetwork*) {
  if(ugp->size < 3) return;
  int i;
  for(i=0;i<ugp->size;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->spec.spec;
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
  }
}

void LVeLayerSpec::Compute_DepressWt(Unit_Group* ugp, LeabraLayer*, LeabraNetwork*) {
  int ui;
  for(ui=1;ui<ugp->size;ui++) {	// don't bother with first unit!
    DaModUnit* u = (DaModUnit*)ugp->FastEl(ui);
    LeabraCon_Group* recv_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
      if(!recv_gp->spec.spec->InheritsFrom(TA_LVConSpec)) continue;
      LVConSpec* cs = (LVConSpec*)recv_gp->spec.spec;
      cs->Depress_Wt(recv_gp, u);
    }
  }
}

void LVeLayerSpec::Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  int pvi_prjn_idx = 0;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.spec;
  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = pvils->Compute_PVe(pvi_lay, net, actual_er_avail, pv_detected);

  bool er_avail = pv_detected;
  if(lv.use_actual_er) er_avail = actual_er_avail; // cheat..

  UNIT_GP_ITR
    (lay, 
     LeabraUnit* u = (LeabraUnit*)ugp->FastEl(0);

     if(er_avail) {
       u->ext = (1.0f - lv.discount) * pve_val;
       ClampValue(ugp, net); 		// apply new value
       Compute_ExtToPlus(ugp, net); 	// copy ext values to act_p
       Compute_dWtUgp(ugp, lay, net);
     }
     else {
       Compute_DepressWt(ugp, lay, net); // always depress!!
     }
     );
}

void LVeLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  // doing second because act_p is computed only at end of settling!
  // this is better than clamping the value in the middle of everything
  // and then continuing with settling..
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_LVPlusPhaseDwt(lay, net);
  AdaptKWTAPt(lay, net);
}

//////////////////////////////////
//	PVLVDa Layer Spec	//
//////////////////////////////////

void PVLVDaSpec::Initialize() {
  mode = LV_PLUS_IF_PV;
  tonic_da = 0.0f;
  min_lvi = 0.1f;
  use_actual_er = false;
}

void PVLVDaLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.clamp_phase2 = false;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("compute_i", true);
  compute_i = KWTA_INHIB;
  SetUnique("i_kwta_pt", true);
  i_kwta_pt = .25;
}

void PVLVDaLayerSpec::InitLinks() {
  LeabraLayerSpec::InitLinks();
  taBase::Own(da, this);
}

void PVLVDaLayerSpec::Defaults() {
  LeabraLayerSpec::Defaults();
  da.Initialize();
  Initialize();
}

void PVLVDaLayerSpec::HelpConfig() {
  String help = "PVLVDaLayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers.\n\
 - No Learning\n\
 \nPVLVDaLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons marked with a MarkerConSpec from PVLV\n\
 - Sending cons to units of type DaModUnit/Spec; puts into their da value\n\
 - This layer must be after recv layers in list of layers\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  cerr << help << endl << flush;
  taMisc::Choice(help, "Ok");
}

bool PVLVDaLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("PVLVDaLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f)) {
    us->SetUnique("act_range", true);
    us->act_range.max = 2.0f;
    us->act_range.min = -2.0f;
    us->act_range.UpdateAfterEdit();
    if(!quiet) taMisc::Error("PVLVDaLayerSpec: requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f)) {
    us->SetUnique("clamp_range", true);
    us->clamp_range.max = 2.0f;
    us->clamp_range.min = -2.0f;
    us->clamp_range.UpdateAfterEdit();
    if(!quiet) taMisc::Error("PVLVDaLayerSpec: requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if(us->act.avg_dt != 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
    if(!quiet) taMisc::Error("PVLVDaLayerSpec: requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }

  // check recv connection
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraLayer* lve_lay = NULL;
  LeabraLayer* lvi_lay = NULL;
  LeabraLayer* pvi_lay = NULL;
  LeabraCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(LeabraCon_Group, recv_gp, u->recv., g) {
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from;
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.spec;
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      if(recv_gp->size <= 0) {
	taMisc::Error("PVLVDaLayerSpec: requires one recv projection with at least one unit!");
	return false;
      }
      if(!recv_gp->Un(0)->InheritsFrom(TA_DaModUnit)) {
	taMisc::Error("PVLVDaLayerSpec: I need to receive from a DaModUnit!");
	return false;
      }
      if(fls->InheritsFrom(TA_LVeLayerSpec)) lve_lay = fmlay;
      if(fls->InheritsFrom(TA_LViLayerSpec)) lvi_lay = fmlay;
      if(fls->InheritsFrom(TA_PViLayerSpec)) pvi_lay = fmlay;
    }
  }

  if(lve_lay == NULL) {
    taMisc::Error("PVLVDaLayerSpec: did not find LVe layer to get Da from!");
    return false;
  }
  if(lvi_lay == NULL) {
    taMisc::Error("PVLVDaLayerSpec: did not find LVi layer to get Da from!");
    return false;
  }
  if(pvi_lay == NULL) {
    taMisc::Error("PVLVDaLayerSpec: did not find PVi layer to get Da from!");
    return false;
  }

  int myidx = lay->own_net->layers.FindLeaf(lay);
  int lvidx = lay->own_net->layers.FindLeaf(lve_lay);
  if(lvidx > myidx) {
    taMisc::Error("PVLVDaLayerSpec: LVe layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }
  lvidx = lay->own_net->layers.FindLeaf(lvi_lay);
  if(lvidx > myidx) {
    taMisc::Error("PVLVDaLayerSpec: LVi layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }
  lvidx = lay->own_net->layers.FindLeaf(pvi_lay);
  if(lvidx > myidx) {
    taMisc::Error("PVLVDaLayerSpec: PVi layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  // check sending layer projections for appropriate unit types
  int si;
  for(si=0;si<lay->send_prjns.size;si++) {
    Projection* prjn = (Projection*)lay->send_prjns[si];
    if(!prjn->from->units.el_typ->InheritsFrom(TA_DaModUnit)) {
      taMisc::Error("PVLVDaLayerSpec: all layers I send to must have DaModUnits!, layer:",
		    prjn->from->GetPath(),"doesn't");
      return false;
    }
  }
  return true;
}

void PVLVDaLayerSpec::Compute_ZeroAct(LeabraLayer* lay, LeabraNetwork*) {
  lay->dav = 0.0f;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->ext = da.tonic_da;
    u->SetExtFlag(Unit::EXT);
  }      
}

void PVLVDaLayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
  int lve_prjn_idx;
  FindLayerFmSpec(lay, lve_prjn_idx, &TA_LVeLayerSpec);
  int lvi_prjn_idx;
  FindLayerFmSpec(lay, lvi_prjn_idx, &TA_LViLayerSpec);
  int pvi_prjn_idx;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.spec;
  bool actual_er_avail = false;
  bool pv_detected = false;
  float pve_val = pvils->Compute_PVe(pvi_lay, net, actual_er_avail, pv_detected);

  bool er_avail = pv_detected;
  if(da.use_actual_er) er_avail = actual_er_avail; // cheat..

  lay->dav = 0.0f;
  DaModUnit* u;
  taLeafItr i;
  FOR_ITR_EL(DaModUnit, u, lay->units., i) {
    LeabraCon_Group* lvecg = (LeabraCon_Group*)u->recv.gp[lve_prjn_idx];
    DaModUnit* lvesu = (DaModUnit*)lvecg->Un(0);
    LeabraCon_Group* lvicg = (LeabraCon_Group*)u->recv.gp[lvi_prjn_idx];
    DaModUnit* lvisu = (DaModUnit*)lvicg->Un(0);
    LeabraCon_Group* pvicg = (LeabraCon_Group*)u->recv.gp[pvi_prjn_idx];
    DaModUnit* pvisu = (DaModUnit*)pvicg->Un(0);
    float eff_lvi = MAX(lvisu->act_eq, da.min_lvi); // effective lvi value
    float lv_da = lvesu->act_eq - eff_lvi; 
    float pv_da = pve_val - pvisu->act_m; 

    if(net->phase_no == 0) {	// not used at this point..
      u->dav = lv_da; 		// lviu->act_eq - avgbl;
    }
    else {
      if(da.mode == PVLVDaSpec::LV_PLUS_IF_PV) {
	u->dav = lv_da;
	if(er_avail)
	  u->dav += pv_da;
      }
      else if(da.mode == PVLVDaSpec::IF_PV_ELSE_LV) {
	if(er_avail)
	  u->dav = pv_da;
	else
	  u->dav = lv_da;
      }
      else if(da.mode == PVLVDaSpec::PV_PLUS_LV) {
	u->dav = pv_da + lv_da;
      }
    }
    u->ext = da.tonic_da + u->dav;
    u->act_eq = u->act = u->net = u->ext;
    lay->dav += u->dav;
  }
  if(lay->units.leaves > 0) lay->dav /= (float)lay->units.leaves;
}

void PVLVDaLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork*) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    LeabraCon_Group* send_gp;
    int g;
    FOR_ITR_GP(LeabraCon_Group, send_gp, u->send., g) {
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesion)	continue;
      if(send_gp->spec.spec->InheritsFrom(TA_MarkerConSpec)) {
	int j;
	for(j=0;j<send_gp->size; j++) {
	  ((DaModUnit*)send_gp->Un(j))->dav = u->act;
	}
      }
    }
  }
}

void PVLVDaLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing
  Compute_Da(lay, net);	// now get the da and clamp it to layer
  Send_Da(lay, net);
  Compute_ActAvg(lay, net);
}

void PVLVDaLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = false;
  lay->UnSetExtFlag(Unit::EXT);
  LeabraLayerSpec::Compute_HardClamp(lay, net);
}

void PVLVDaLayerSpec::Compute_dWt(LeabraLayer*, LeabraNetwork*) {
  return;
}

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

bool SNcLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!PVLVDaLayerSpec::CheckConfig(lay, net, quiet)) return false;

  int myidx = lay->own_net->layers.FindLeaf(lay);

  int pc_prjn_idx;
  LeabraLayer* pclay = FindLayerFmSpec(lay, pc_prjn_idx, &TA_PatchLayerSpec);
  if(pclay != NULL) {
    int patchidx = lay->own_net->layers.FindLeaf(pclay);
    if(patchidx > myidx) {
      taMisc::Error("SNcLayerSpec: Patch layer must be *before* this layer in list of layers -- it is now after, won't work");
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

bool MatrixLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet))
    return false;

  SetUnique("decay", true);
  decay.phase = 0.0f;
  decay.phase2 = 0.0f;
  decay.clamp_phase2 = false;

  if(lay->units.gp.size == 1) {
    taMisc::Error("MatrixLayerSpec: layer must contain multiple unit groups (= stripes) for indepent searching of gating space!");
    return false;
  }

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("MatrixLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("MatrixLayerSpec: must have DaModUnits!");
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_MatrixUnitSpec)) {
    taMisc::Error("MatrixLayerSpec: UnitSpec must be MatrixUnitSpec!");
    return false;
  }
  ((DaModUnitSpec*)us)->da_mod.p_dwt = false; // don't need prior state dwt
  if((us->opt_thresh.learn >= 0.0f) || us->opt_thresh.updt_wts) {
    if(!quiet) taMisc::Error("MatrixLayerSpec: UnitSpec opt_thresh.learn must be -1 to allow proper learning of all units",
			     "I just set it for you in spec:", us->name,
			     "(make sure this is appropriate for all layers that use this spec!)");
    us->SetUnique("opt_thresh", true);
    us->opt_thresh.learn = -1.0f;
    us->opt_thresh.updt_wts = false;
  }
  if(us->act.avg_dt <= 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.005f;
    if(!quiet) taMisc::Error("MatrixLayerSpec: requires UnitSpec act.avg_dt > 0, I just set it to .005 for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->SetUnique("g_bar", true);
  // must have these not initialized every trial!
  if(us->hyst.init) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
    if(!quiet) taMisc::Error("MatrixLayerSpec: requires UnitSpec hyst.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if(us->acc.init) {
    us->SetUnique("acc", true);
    us->acc.init = false;
    if(!quiet) taMisc::Error("MatrixLayerSpec: requires UnitSpec acc.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->UpdateAfterEdit();

  LeabraBiasSpec* bs = (LeabraBiasSpec*)us->bias_spec.spec;
  if(bs == NULL) {
    taMisc::Error("MatrixLayerSpec: Error: null bias spec in unit spec", us->name);
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
      taMisc::Error("MatrixLayerSpec:  Receiving connections must be of type MatrixConSpec!");
      return false;
    }
    if(cs->wt_limits.sym != false) {
      cs->SetUnique("wt_limits", true);
      cs->wt_limits.sym = false;
      if(!quiet) taMisc::Error("MatrixLayerSpec: requires recv connections to have wt_limits.sym=false, I just set it for you in spec:",
			       cs->name,"(make sure this is appropriate for all layers that use this spec!)");
    }
    if(bg_type == MatrixLayerSpec::OUTPUT) {
      if((cs->learn_rule != MatrixConSpec::OUTPUT_DELTA) && (cs->learn_rule != MatrixConSpec::OUTPUT_CHL)) {
	cs->SetUnique("learn_rule", true);
	cs->learn_rule = MatrixConSpec::OUTPUT_DELTA;
	if(!quiet) taMisc::Error("MatrixLayerSpec: OUTPUT BG requires MatrixConSpec learn_rule of OUTPUT type, I just set it for you in spec:",
				 cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
    else {			// pfc
      if((cs->learn_rule == MatrixConSpec::OUTPUT_DELTA) || (cs->learn_rule == MatrixConSpec::OUTPUT_CHL)) {
	cs->SetUnique("learn_rule", true);
	cs->learn_rule = MatrixConSpec::MAINT;
	if(!quiet) taMisc::Error("MatrixLayerSpec: BG_pfc requires MatrixConSpec learn_rule of MAINT type, I just set it for you in spec:",
				 cs->name,"(make sure this is appropriate for all layers that use this spec!)");
      }
    }
  }
  if(da_lay == NULL) {
    taMisc::Error("MatrixLayerSpec: Could not find DA layer (PVLVDaLayerSpec, VTA or SNc) -- must receive MarkerConSpec projection from one!");
    return false;
  }
  if(snr_lay == NULL) {
    taMisc::Error("MatrixLayerSpec: Could not find SNrThal layer -- must receive MarkerConSpec projection from one!");
    return false;
  }
  // vta/snc must be before matrix!  good.
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int daidx = lay->own_net->layers.FindLeaf(da_lay);
  if(daidx > myidx) {
    taMisc::Error("MatrixLayerSpec: DA layer (PVLVDaLayerSpec, VTA or SNc) must be *before* this layer in list of layers -- it is now after, won't work");
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

bool SNrThalLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  SetUnique("decay", true);
  decay.clamp_phase2 = false;

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("SNrThalLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }

  // must have the appropriate ranges for unit specs..
  //  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;

  // check recv connection
  int mtx_prjn_idx = 0;
  LeabraLayer* matrix_lay = FindLayerFmSpec(lay, mtx_prjn_idx, &TA_MatrixLayerSpec);

  if(matrix_lay == NULL) {
    taMisc::Error("SNrThalLayerSpec: did not find Matrix layer to recv from!");
    return false;
  }

  if(matrix_lay->units.gp.size != lay->units.gp.size) {
    taMisc::Error("SNrThalLayerSpec: MatrixLayer unit groups must = SNrThalLayer unit groups!");
    lay->geom.z = matrix_lay->units.gp.size;
    return false;
  }

  int myidx = lay->own_net->layers.FindLeaf(lay);
  int matidx = lay->own_net->layers.FindLeaf(matrix_lay);
  if(matidx > myidx) {
    taMisc::Error("SNrThalLayerSpec: Matrix layer must be *before* this layer in list of layers -- it is now after, won't work");
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

bool PFCLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  if(lay->units.gp.size == 1) {
    taMisc::Error("PFCLayerSpec: layer must contain multiple unit groups (= stripes) for indepent searching of gating space!");
    return false;
  }

  if(net->phase_order != LeabraNetwork::MINUS_PLUS_PLUS) {
    if(!quiet) taMisc::Error("PFCLayerSpec: requires LeabraNetwork phase_oder = MINUS_PLUS_PLUS, I just set it for you");
    net->phase_order = LeabraNetwork::MINUS_PLUS_PLUS;
  }
  if(net->first_plus_dwt != LeabraNetwork::ONLY_FIRST_DWT) {
    if(!quiet) taMisc::Error("PFCLayerSpec: requires LeabraNetwork first_plus_dwt = ONLY_FIRST_DWT, I just set it for you");
    net->first_plus_dwt = LeabraNetwork::ONLY_FIRST_DWT;
  }

  if(net->trial_init != LeabraNetwork::DECAY_STATE) {
    if(!quiet) taMisc::Error("PFCLayerSpec: requires LeabraNetwork trial_init = DECAY_STATE, I just set it for you");
    net->trial_init = LeabraNetwork::DECAY_STATE;
  }
  if(net->no_plus_test) {
    if(!quiet) taMisc::Error("PFCLayerSpec: requires LeabraNetwork no_plus_test = false, I just set it for you");
    net->no_plus_test = false;
  }

  if(net->min_cycles_phase2 < 35) {
    if(!quiet) taMisc::Error("PFCLayerSpec: requires LeabraNetwork min_cycles_phase2 >= 35, I just set it for you");
    net->min_cycles_phase2 = 35;
  }

  if(net->sequence_init != LeabraNetwork::DO_NOTHING) {
    if(!quiet) taMisc::Error("PFCLayerSpec: requires network sequence_init = DO_NOTHING, I just set it for you");
    net->sequence_init = LeabraNetwork::DO_NOTHING;
  }

  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("PFCLayerSpec: must have DaModUnits!");
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("PFCLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }

  if(us->act.avg_dt <= 0.0f) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.005f;
    if(!quiet) taMisc::Error("PFCLayerSpec: requires UnitSpec act.avg_dt > 0, I just set it to .005 for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  us->SetUnique("g_bar", true);
  if(us->hyst.init) {
    us->SetUnique("hyst", true);
    us->hyst.init = false;
    if(!quiet) taMisc::Error("PFCLayerSpec: requires UnitSpec hyst.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }
  if(us->acc.init) {
    us->SetUnique("acc", true);
    us->acc.init = false;
    if(!quiet) taMisc::Error("PFCLayerSpec: requires UnitSpec acc.init = false, I just set it for you in spec:",
		  us->name,"(make sure this is appropriate for all layers that use this spec!)");
  }

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);	// taking 1st unit as representative
  LeabraCon_Group* recv_gp;
  int g;
  for(g=0;g<u->recv.gp.size; g++) {
    recv_gp = (LeabraCon_Group*)u->recv.gp[g];
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from;
    if(fmlay == NULL) {
      taMisc::Error("*** PFCLayerSpec: null from layer in recv projection:", (String)g);
      return false;
    }
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->spec.spec;
    if(cs->InheritsFrom(TA_MarkerConSpec)) continue;
    // could check the conspec parameters here..
  }

  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  if(snrthal_lay == NULL) {
    taMisc::Error("*** PFCLayerSpec: Warning: no projection from SNrThal Layer found: must have MarkerConSpec!");
    return false;
  }
  if(snrthal_lay->units.gp.size != lay->units.gp.size) {
    taMisc::Error("PFCLayerSpec: Gating Layer unit groups must = PFCLayer unit groups!");
    snrthal_lay->geom.z = lay->units.gp.size;
    return false;
  }

  // check for ordering of layers!
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int gateidx = lay->own_net->layers.FindLeaf(snrthal_lay);
  if(gateidx > myidx) {
    taMisc::Error("PFCLayerSpec: SNrThal Layer must be *before* this layer in list of layers -- it is now after, won't work");
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

bool PFCOutLayerSpec::CheckConfig(LeabraLayer* lay, LeabraNetwork* net, bool quiet) {
  if(!LeabraLayerSpec::CheckConfig(lay, net, quiet)) return false;

  if(decay.clamp_phase2) {
    SetUnique("decay", true);
    decay.event = 0.0f;
    decay.phase = 0.0f;
    decay.phase2 = 0.1f;
    decay.clamp_phase2 = false;
  }

  if(lay->units.gp.size == 1) {
    taMisc::Error("PFCOutLayerSpec: layer must contain multiple unit groups (= stripes) for indepent searching of gating space!");
    return false;
  }

  if(!lay->units.el_typ->InheritsFrom(TA_DaModUnit)) {
    taMisc::Error("PFCOutLayerSpec: must have DaModUnits!");
    return false;
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.spec;
  if(!us->InheritsFrom(TA_DaModUnitSpec)) {
    taMisc::Error("PFCOutLayerSpec: UnitSpec must be DaModUnitSpec!");
    return false;
  }

  int snrthal_prjn_idx;
  LeabraLayer* snrthal_lay = FindLayerFmSpec(lay, snrthal_prjn_idx, &TA_SNrThalLayerSpec);
  if(snrthal_lay == NULL) {
    taMisc::Error("*** PFCOutLayerSpec: Warning: no projection from SNrThal Layer found: must have MarkerConSpec!");
    return false;
  }
  if(snrthal_lay->units.gp.size != lay->units.gp.size) {
    taMisc::Error("PFCOutLayerSpec: Gating Layer unit groups must = PFCOutLayer unit groups!");
    snrthal_lay->geom.z = lay->units.gp.size;
    return false;
  }

  int pfc_prjn_idx;
  LeabraLayer* pfc_lay = FindLayerFmSpec(lay, pfc_prjn_idx, &TA_PFCLayerSpec);
  if(pfc_lay == NULL) {
    taMisc::Error("*** PFCOutLayerSpec: Warning: no projection from PFC Layer found: must have MarkerConSpec!");
    return false;
  }
  if(pfc_lay->units.gp.size != lay->units.gp.size) {
    if(!quiet)
      taMisc::Error("PFCOutLayerSpec: PFC Layer unit groups must = PFCOutLayer unit groups, copiped from PFC Layer; Please do a Build of network");
    lay->geom.z = pfc_lay->units.gp.size;
  }
  if(pfc_lay->units.leaves != lay->units.leaves) {
    if(!quiet)
      taMisc::Error("PFCOutLayerSpec: PFC Layer units must = PFCOutLayer units, copied from PFC Layer; Please do a Build of network");
    lay->geom = pfc_lay->geom;
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
    taMisc::Error("PFCOutLayerSpec: SNrThal Layer must be *before* this layer in list of layers -- it is now after, won't work");
    return false;
  }

  // check for ordering of layers!
  int pfcidx = lay->own_net->layers.FindLeaf(pfc_lay);
  if(pfcidx > myidx) {
    taMisc::Error("PFCOutLayerSpec: PFC Layer must be *before* this layer in list of layers -- it is now after, won't work");
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

void LeabraWizard::Initialize() {
  connectivity = BIDIRECTIONAL;
}

void LeabraWizard::StdNetwork(Network* net) {
  if(net == NULL)
    net = pdpMisc::GetNewNetwork(GET_MY_OWNER(LeabraProject));
  if(net == NULL) return;
  Wizard::StdNetwork(net);
  StdLayerSpecs((LeabraNetwork*)net);
}

void LeabraWizard::StdLayerSpecs(LeabraNetwork* net) {
  if(net == NULL) return;
  LeabraLayerSpec* hid = (LeabraLayerSpec*)net->FindMakeSpec(NULL, &TA_LeabraLayerSpec);
  hid->name = "HiddenLayer";
  LeabraLayerSpec* inout = (LeabraLayerSpec*)hid->children.FindMakeSpec("Input_Output", &TA_LeabraLayerSpec);
  hid->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB;
  hid->i_kwta_pt = .6f;
  inout->SetUnique("compute_i", true);
  inout->SetUnique("i_kwta_pt", true);
  inout->SetUnique("kwta", true);
  inout->compute_i = LeabraLayerSpec::KWTA_INHIB;
  inout->i_kwta_pt = .25f;
  inout->kwta.k_from = KWTASpec::USE_PAT_K;
  // todo: !!!
//   winbMisc::DelayedMenuUpdate(hid);

  int i;
  if(net->layers.size == layer_cfg.size) {	// likely to be using specs
    for(i=0;i<layer_cfg.size;i++) {
      LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
      Layer* lay = (Layer*)net->layers.FindName(el->name);
      if(lay != NULL) {
	if(el->io_type == LayerWizEl::HIDDEN)
	  lay->SetLayerSpec(hid);
	else
	  lay->SetLayerSpec(inout);
      }
    }
  }
  else {
    for(i=0;i<net->layers.size;i++) {
      Layer* lay = (Layer*)net->layers[i];
      if(lay->layer_type == Layer::HIDDEN)
	lay->SetLayerSpec(hid);
      else
	lay->SetLayerSpec(inout);
    }
  }

  // move the bias spec under the con spec
  LeabraBiasSpec* bs = (LeabraBiasSpec*)net->specs.FindType(&TA_LeabraBiasSpec);
  if(bs != NULL) {
    LeabraConSpec* ps = (LeabraConSpec*)bs->FindParent();
    if(ps != NULL) return;
    ps = (LeabraConSpec*)net->specs.FindSpecTypeNotMe(&TA_LeabraConSpec, bs);
    if(ps != NULL) {
      ps->children.Transfer(bs);
      // todo: !!!
//       winbMisc::DelayedMenuUpdate(ps);
    }
  }
}

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

///////////////////////////////////////////////////////////////
//			Unit Inhib
///////////////////////////////////////////////////////////////

void LeabraWizard::UnitInhib(LeabraNetwork* net, int n_inhib_units) {
  net->RemoveUnits();
  
  LeabraUnitSpec* basic_us = (LeabraUnitSpec*)net->FindSpecType(&TA_LeabraUnitSpec);
  if(basic_us == NULL) {
    taMisc::Error("ConfigUnitInhib: basic LeabraUnitSpec not found, bailing!");
    return;
  }
  LeabraUnitSpec* inhib_us = (LeabraUnitSpec*)basic_us->children.FindMakeSpec("InhibUnits", &TA_LeabraUnitSpec);
  if(inhib_us == NULL) return;

  LeabraConSpec* basic_cs = (LeabraConSpec*)net->FindSpecType(&TA_LeabraConSpec);
  if(basic_cs == NULL) {
    taMisc::Error("ConfigUnitInhib: basic LeabraConSpec not found, bailing!");
    return;
  }
  LeabraConSpec* inhib_cs = (LeabraConSpec*)basic_cs->children.FindMakeSpec("InhibCons", &TA_LeabraConSpec);
  if(inhib_cs == NULL) return;

  LeabraConSpec* fb_inhib_cs = (LeabraConSpec*)basic_cs->children.FindMakeSpec("FBtoInhib", &TA_LeabraConSpec);
  if(fb_inhib_cs == NULL) return;
  LeabraConSpec* ff_inhib_cs = (LeabraConSpec*)fb_inhib_cs->children.FindMakeSpec("FFtoInhib", &TA_LeabraConSpec);
  if(ff_inhib_cs == NULL) return;

  LeabraLayerSpec* basic_ls = (LeabraLayerSpec*)net->FindSpecType(&TA_LeabraLayerSpec);
  if(basic_ls == NULL) {
    taMisc::Error("ConfigUnitInhib: basic LeabraLayerSpec not found, bailing!");
    return;
  }
  LeabraLayerSpec* inhib_ls = (LeabraLayerSpec*)basic_ls->children.FindMakeSpec("InhibLayers", &TA_LeabraLayerSpec);
  if(inhib_ls == NULL) return;

  FullPrjnSpec* fullprjn = (FullPrjnSpec*)net->FindSpecType(&TA_FullPrjnSpec);
  if(fullprjn == NULL) {
    taMisc::Error("ConfigUnitInhib: basic FullPrjnSpec not found, bailing!");
    return;
  }

  // todo: optimize these params..
  basic_us->dt.vm = .04f;
  basic_us->g_bar.i = 10.0f;
  inhib_us->SetUnique("dt", true);
  inhib_us->dt.vm = .07f;

  inhib_cs->SetUnique("rnd", true);
  inhib_cs->rnd.mean = 1.0f;  inhib_cs->rnd.var = 0.0f;
  inhib_cs->SetUnique("wt_limits", true);
  inhib_cs->wt_limits.sym = false;
  inhib_cs->SetUnique("inhib", true);
  inhib_cs->inhib = true;

  fb_inhib_cs->SetUnique("wt_limits", true);
  fb_inhib_cs->wt_limits.sym = false;
  fb_inhib_cs->SetUnique("rnd", true);
  fb_inhib_cs->rnd.mean = .5f;  fb_inhib_cs->rnd.var = 0.05f;
  fb_inhib_cs->SetUnique("lrate", true);
  fb_inhib_cs->lrate = 0.0f;

  // todo: optimize
  ff_inhib_cs->SetUnique("wt_scale", true);
  ff_inhib_cs->wt_scale.abs = .4f;

  basic_ls->compute_i = LeabraLayerSpec::UNIT_INHIB;
  basic_ls->adapt_i.type = AdaptISpec::G_BAR_I;
  basic_ls->adapt_i.tol = .02f;	// these params sometimes get off..
  basic_ls->adapt_i.p_dt = .1f;

  inhib_ls->SetUnique("kwta", true);
  inhib_ls->kwta.k_from = KWTASpec::USE_PCT;
  inhib_ls->kwta.pct = .34f;

  basic_us->UpdateAfterEdit();
  basic_cs->UpdateAfterEdit();
  basic_ls->UpdateAfterEdit();

  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    if(lay->layer_type == Layer::INPUT) continue;
    String nm = lay->name;
    nm.downcase();
    if(nm.contains("_inhib")) continue;

    String inm = lay->name + "_Inhib";
    LeabraLayer* ilay = (LeabraLayer*)net->layers.FindName(inm);
    if(ilay == NULL) {
      ilay = (LeabraLayer*)net->layers.NewEl(1);
      ilay->name = inm;
      ilay->pos.z = lay->pos.z;
      ilay->pos.x = lay->pos.x + lay->act_geom.x + 1;
      ilay->n_units = n_inhib_units;
      if(n_inhib_units <= 20) {
	ilay->geom.x = 2; ilay->geom.y = n_inhib_units / 2;
	while(ilay->geom.x * ilay->geom.y < n_inhib_units) ilay->geom.y++;
      }
      else if(n_inhib_units <= 40) {
	ilay->geom.x = 4; ilay->geom.y = n_inhib_units / 4;
	while(ilay->geom.x * ilay->geom.y < n_inhib_units) ilay->geom.y++;
      }
    }
    ilay->SetLayerSpec(inhib_ls);
    ilay->SetUnitSpec(inhib_us);

    int j;
    for(j=0;j<lay->projections.size;j++) {
      LeabraLayer* fmlay = (LeabraLayer*)((Projection*)lay->projections[j])->from;
      if(fmlay->name.contains("_Inhib")) continue;
      if(fmlay == lay) continue;
      net->FindMakePrjn(ilay, fmlay, fullprjn, ff_inhib_cs);
    }
    net->FindMakePrjn(ilay, lay, fullprjn, fb_inhib_cs);
    net->FindMakePrjn(lay, ilay, fullprjn, inhib_cs);
    net->FindMakePrjn(ilay, ilay, fullprjn, inhib_cs);
  }

  net->UpdateAfterEdit();
  // todo:
//   net->InitAllViews();

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  // set settle cycles to 300
  net->cycle_max = 300;
  net->min_cycles = 150;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = pdpMisc::FindSelectEdit(proj);
  if(edit != NULL) {
    basic_us->SelectForEditNm("dt", edit, "excite");
    inhib_us->SelectForEditNm("dt", edit, "inhib");
    basic_us->SelectForEditNm("g_bar", edit, "excite");
    ff_inhib_cs->SelectForEditNm("wt_scale", edit, "ff_inhib");
    fb_inhib_cs->SelectForEditNm("rnd", edit, "to_inhib");
    inhib_cs->SelectForEditNm("rnd", edit, "fm_inhib");
    basic_ls->SelectForEditNm("adapt_i", edit, "layers");

    net->SelectForEditNm("cycle_max", edit, net->name);
    net->SelectForEditNm("min_cycles", edit, net->name);
  }
}

///////////////////////////////////////////////////////////////
//			TD
///////////////////////////////////////////////////////////////

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

void LeabraWizard::TD(LeabraNetwork* net, bool bio_labels, bool td_mod_all) {
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
  taMisc::Choice(msg,"Ok");

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
  if(rew_targ_lay == NULL || tdrp == NULL || extrew == NULL || tdint == NULL || tdda == NULL) return;
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
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.spec;
    lay->SetUnitType(&TA_DaModUnit);
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != tdrp && lay != extrew && lay != tdint && lay != tdda
       && !laysp->InheritsFrom(&TA_PFCLayerSpec) && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_PatchLayerSpec) 
       && !laysp->InheritsFrom(&TA_SNcLayerSpec) && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
      other_lays.Link(lay);
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

  String gpprfx = "PFC_BG_";
  if(!bio_labels)
    gpprfx = "TD_";

  BaseSpec_Group* units = net->FindMakeSpecGp(gpprfx + "Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp(gpprfx + "Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp(gpprfx + "Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp(gpprfx + "Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return;

  LeabraUnitSpec* rewpred_units = (LeabraUnitSpec*)units->FindMakeSpec("TDRewPredUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* td_units = (LeabraUnitSpec*)units->FindMakeSpec("TdUnits", &TA_DaModUnitSpec);
  if(rewpred_units == NULL || td_units == NULL) return;

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(learn_cons == NULL) return;

  TDRewPredConSpec* rewpred_cons = (TDRewPredConSpec*)learn_cons->FindMakeChild("TDRewPredCons", &TA_TDRewPredConSpec);
  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return;
  LeabraConSpec* fixed_bias = (LeabraConSpec*)bg_bias->FindMakeChild("FixedBias", &TA_LeabraBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  if(rewpred_cons == NULL || marker_cons == NULL || fixed_bias == NULL)
    return;

  ExtRewLayerSpec* ersp = (ExtRewLayerSpec*)layers->FindMakeSpec("ExtRewLayer", &TA_ExtRewLayerSpec);
  TDRewPredLayerSpec* tdrpsp = (TDRewPredLayerSpec*)layers->FindMakeSpec(tdrpnm + "Layer", &TA_TDRewPredLayerSpec);
  TDRewIntegLayerSpec* tdintsp = (TDRewIntegLayerSpec*)layers->FindMakeSpec(tdintnm + "Layer", &TA_TDRewIntegLayerSpec);
  TdLayerSpec* tdsp = (TdLayerSpec*)layers->FindMakeSpec(tddanm + "Layer", &TA_TdLayerSpec);
  if(tdrpsp == NULL || ersp == NULL || tdintsp == NULL || tdsp == NULL) return;

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  if(fullprjn == NULL || onetoone == NULL) return;

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

  if(tdrp->n_units != n_rp_u) { tdrp->n_units = n_rp_u; tdrp->geom.x = n_rp_u; tdrp->geom.y = 1; }
  if(extrew->n_units != 8) { extrew->n_units = 8; extrew->geom.x = 8; extrew->geom.y = 1; }
  if(tdint->n_units != n_rp_u) { tdint->n_units = n_rp_u; tdint->geom.x = n_rp_u; tdint->geom.y = 1; }
  tdda->n_units = 1;
  rew_targ_lay->n_units = 1;

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
  net->Connect();

  bool ok = tdrpsp->CheckConfig(tdrp, net, true) && tdintsp->CheckConfig(tdint, net, true)
    && tdsp->CheckConfig(tdda, net, true) && ersp->CheckConfig(extrew, net, true);

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
  taMisc::Choice(msg,"Ok");

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
  SelectEdit* edit = pdpMisc::FindSelectEdit(proj);
  if(edit != NULL) {
    rewpred_cons->SelectForEditNm("lrate", edit, "rewpred");
    ersp->SelectForEditNm("rew", edit, "extrew");
    tdrpsp->SelectForEditNm("rew_pred", edit, "tdrp");
    tdintsp->SelectForEditNm("rew_integ", edit, "tdint");
  }
}

///////////////////////////////////////////////////////////////
//			PVLV
///////////////////////////////////////////////////////////////

// todo: set td_mod.on = true for td_mod_all; need to get UnitSpec..

void LeabraWizard::PVLV(LeabraNetwork* net, bool bio_labels, bool localist_val, bool fm_hid_cons, bool fm_out_cons, bool da_mod_all) {
  String msg = "Configuring Pavlov (PVLV) Layers:\n\n\
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
  taMisc::Choice(msg,"Ok");

  net->RemoveUnits();

  //////////////////////////////////////////////////////////////////////////////////
  // make layers

  bool	lve_new = false;
  String pvenm = "PVe_LHA";  String pvinm = "PVi_VSpatch";
  String lvenm = "LVe_CNA";  String lvinm = "LVi_VSpatch";
  String vtanm = "VTA";
  String alt_pvenm = "PVe";  String alt_pvinm = "PVi";
  String alt_lvenm = "LVe";  String alt_lvinm = "LVi";
  String alt_vtanm = "DA";
  if(!bio_labels) {
    pvenm = "PVe"; 	pvinm = "PVi";    lvenm = "LVe";    lvinm = "LVi";    vtanm = "DA";
    alt_pvenm = "PVe_LHA"; alt_pvinm = "PVi_VSpatch";
    alt_lvenm = "LVe_CNA"; alt_lvinm = "LVi_VSpatch";
    alt_vtanm = "VTA";
  }

  bool dumbo;
  LeabraLayer* rew_targ_lay = (LeabraLayer*)net->FindMakeLayer("RewTarg");
  LeabraLayer* pve = (LeabraLayer*)net->FindMakeLayer(pvenm, NULL, dumbo, alt_pvenm);
  LeabraLayer* pvi = (LeabraLayer*)net->FindMakeLayer(pvinm, NULL, dumbo, alt_pvinm);
  LeabraLayer* lve = (LeabraLayer*)net->FindMakeLayer(lvenm, NULL, lve_new, alt_lvenm);
  LeabraLayer* lvi = (LeabraLayer*)net->FindMakeLayer(lvinm, NULL, dumbo, alt_lvinm);
  LeabraLayer* vta = (LeabraLayer*)net->FindMakeLayer(vtanm, NULL, dumbo, alt_vtanm);
  if(rew_targ_lay == NULL || lve == NULL || pve == NULL || pvi == NULL || vta == NULL) return;

  //////////////////////////////////////////////////////////////////////////////////
  // sort layers

  rew_targ_lay->name = "0000";  pve->name = "0001"; pvi->name = "0002";  
  lve->name = "0003";  lvi->name = "0004";    vta->name = "0005";

  net->layers.Sort();

  rew_targ_lay->name = "RewTarg";  pve->name = pvenm; pvi->name = pvinm;
  lve->name = lvenm; lvi->name = lvinm; vta->name = vtanm;

  //////////////////////////////////////////////////////////////////////////////////
  // collect layer groups

  Layer_Group other_lays;
  Layer_Group hidden_lays;
  Layer_Group output_lays;
  Layer_Group input_lays;
  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.spec;
    lay->SetUnitType(&TA_DaModUnit);
    // todo: add any new bg layer exclusions here!
    if(lay != rew_targ_lay && lay != lve && lay != pve && lay != pvi && lay != lvi && lay != vta
       && !laysp->InheritsFrom(&TA_PFCLayerSpec) && !laysp->InheritsFrom(&TA_MatrixLayerSpec)
       && !laysp->InheritsFrom(&TA_PatchLayerSpec) 
       && !laysp->InheritsFrom(&TA_SNcLayerSpec) && !laysp->InheritsFrom(&TA_SNrThalLayerSpec)) {
      other_lays.Link(lay);
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

  String gpprfx = "PFC_BG_";
//   if(!bio_labels)
//     gpprfx = "DA_";

  BaseSpec_Group* units = net->FindMakeSpecGp(gpprfx + "Units");
  BaseSpec_Group* cons = net->FindMakeSpecGp(gpprfx + "Cons");
  BaseSpec_Group* layers = net->FindMakeSpecGp(gpprfx + "Layers");
  BaseSpec_Group* prjns = net->FindMakeSpecGp(gpprfx + "Prjns");
  if(units == NULL || cons == NULL || layers == NULL || prjns == NULL) return;

  LeabraUnitSpec* pv_units = (LeabraUnitSpec*)units->FindMakeSpec("PVUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* lv_units = (LeabraUnitSpec*)pv_units->FindMakeChild("LVUnits", &TA_DaModUnitSpec);
  LeabraUnitSpec* da_units = (LeabraUnitSpec*)units->FindMakeSpec("DaUnits", &TA_DaModUnitSpec);
  if(lv_units == NULL || pv_units == NULL || da_units == NULL) return;

  LeabraConSpec* learn_cons = (LeabraConSpec*)cons->FindMakeSpec("LearnCons", &TA_LeabraConSpec);
  if(learn_cons == NULL) return;

  LVConSpec* pvi_cons = (LVConSpec*)learn_cons->FindMakeChild("PVi", &TA_PVConSpec);
  LVConSpec* lve_cons = (LVConSpec*)pvi_cons->FindMakeChild("LVe", &TA_LVConSpec);
  LVConSpec* lvi_cons = (LVConSpec*)lve_cons->FindMakeChild("LVi", &TA_LVConSpec);
  LeabraConSpec* bg_bias = (LeabraConSpec*)learn_cons->FindMakeChild("BgBias", &TA_LeabraBiasSpec);
  if(bg_bias == NULL) return;
  LeabraConSpec* fixed_bias = (LeabraConSpec*)bg_bias->FindMakeChild("FixedBias", &TA_LeabraBiasSpec);

  LeabraConSpec* marker_cons = (LeabraConSpec*)cons->FindMakeSpec("MarkerCons", &TA_MarkerConSpec);
  if(lve_cons == NULL || marker_cons == NULL || fixed_bias == NULL)
    return;

  ExtRewLayerSpec* pvesp = (ExtRewLayerSpec*)layers->FindMakeSpec(pvenm + "Layer", &TA_ExtRewLayerSpec);
  PViLayerSpec* pvisp = (PViLayerSpec*)layers->FindMakeSpec(pvinm + "Layer", &TA_PViLayerSpec, dumbo, alt_pvinm + "Layer");
  LVeLayerSpec* lvesp = (LVeLayerSpec*)layers->FindMakeSpec(lvenm + "Layer", &TA_LVeLayerSpec, dumbo, alt_lvenm + "Layer");
  LVeLayerSpec* lvisp = (LVeLayerSpec*)lvesp->FindMakeChild(lvinm + "Layer", &TA_LViLayerSpec, dumbo, alt_lvinm + "Layer");
  PVLVDaLayerSpec* dasp = (PVLVDaLayerSpec*)layers->FindMakeSpec(vtanm + "Layer", &TA_PVLVDaLayerSpec, dumbo, alt_vtanm + "Layer");
  if(lvesp == NULL || pvesp == NULL || pvisp == NULL || dasp == NULL) return;

  ProjectionSpec* fullprjn = (ProjectionSpec*)prjns->FindMakeSpec("FullPrjn", &TA_FullPrjnSpec);
  ProjectionSpec* onetoone = (ProjectionSpec*)prjns->FindMakeSpec("OneToOne", &TA_OneToOnePrjnSpec);
  if(fullprjn == NULL || onetoone == NULL) return;

  //////////////////////////////////////////////////////////////////////////////////
  // set default spec parameters

  //  learn_cons->lmix.hebb = .01f; // .01 hebb on learn cons
//   learn_cons->not_used_ok = true;
  learn_cons->UpdateAfterEdit();
  bg_bias->SetUnique("lrate", true);
  bg_bias->lrate = 0.0f;
  fixed_bias->SetUnique("lrate", true);
  fixed_bias->lrate = 0.0f;

  pvi_cons->SetUnique("lrate", true);
  lve_cons->SetUnique("lrate", true);
  lvi_cons->SetUnique("lrate", true);

  // NOT unique: inherit from pvi:
  lve_cons->SetUnique("rnd", false);
  lve_cons->SetUnique("wt_limits", false);
  lve_cons->SetUnique("wt_sig", false);
  lve_cons->SetUnique("lmix", false);
  lve_cons->SetUnique("lrate_sched", false);
  lve_cons->SetUnique("lrs_value", false);

  // NOT unique: inherit from lve:
  lvi_cons->SetUnique("rnd", false);
  lvi_cons->SetUnique("wt_limits", false);
  lvi_cons->SetUnique("wt_sig", false);
  lvi_cons->SetUnique("lmix", false);
  lvi_cons->SetUnique("lrate_sched", false);
  lvi_cons->SetUnique("lrs_value", false);

  // NOT unique: inherit from lve
  lvisp->SetUnique("decay", false);
  lvisp->SetUnique("kwta", false);
  lvisp->SetUnique("inhib_group", false);
  lvisp->SetUnique("compute_i", false);
  lvisp->SetUnique("i_kwta_pt", false);

  pv_units->SetUnique("g_bar", true);

  if(localist_val) {
    pvesp->scalar.rep = ScalarValSpec::LOCALIST;
    pvisp->scalar.rep = ScalarValSpec::LOCALIST;
    lvesp->scalar.rep = ScalarValSpec::LOCALIST;

    pvesp->scalar.min_sum_act = .2f;
    pvisp->scalar.min_sum_act = .2f;
    lvesp->scalar.min_sum_act = .2f;

    dasp->da.mode = PVLVDaSpec::LV_PLUS_IF_PV;

    lvesp->bias_val.un = ScalarValBias::GC;
    lvesp->bias_val.wt = ScalarValBias::NO_WT;
    lvesp->bias_val.val = 0.0f;
    pvisp->bias_val.un = ScalarValBias::GC;
    pvisp->bias_val.wt = ScalarValBias::NO_WT;
    pvisp->bias_val.val = 0.5f;
    pvisp->pv_detect.thr_min = .2f;
    pvisp->pv_detect.thr_max = .8f;

    pvesp->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB; pvesp->i_kwta_pt = 0.9f;
    pvisp->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB; pvisp->i_kwta_pt = 0.9f;
    lvesp->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB; lvesp->i_kwta_pt = 0.9f;

    pvesp->kwta.k_from = KWTASpec::USE_K; pvesp->gp_kwta.k_from = KWTASpec::USE_K;
    pvesp->kwta.k = 1; 	pvesp->gp_kwta.k = 1; 
    pvisp->kwta.k_from = KWTASpec::USE_K; pvisp->gp_kwta.k_from = KWTASpec::USE_K;
    pvisp->kwta.k = 1; 	pvisp->gp_kwta.k = 1; 
    lvesp->kwta.k_from = KWTASpec::USE_K; lvesp->gp_kwta.k_from = KWTASpec::USE_K;
    lvesp->kwta.k = 1; 	lvesp->gp_kwta.k = 1; 

    pvesp->unit_range.min = 0.0f;  pvesp->unit_range.max = 1.0f;
    pvesp->unit_range.UpdateAfterEdit();
    pvesp->val_range = pvesp->unit_range;

    pvisp->unit_range.min = 0.0f;  pvisp->unit_range.max = 1.0f;
    pvisp->unit_range.UpdateAfterEdit();
    pvisp->val_range = pvisp->unit_range;

    lvesp->unit_range.min = 0.0f;  lvesp->unit_range.max = 1.0f;
    lvesp->unit_range.UpdateAfterEdit();
    lvesp->val_range = lvesp->unit_range;

    pv_units->SetUnique("act", true);
    pv_units->SetUnique("act_fun", true);
    pv_units->SetUnique("dt", true);
    pv_units->act_fun = LeabraUnitSpec::NOISY_LINEAR;
    pv_units->act.thr = .17f;
    pv_units->act.gain = 220.0f;
    pv_units->act.nvar = .01f;
    pv_units->g_bar.l = .1f;
    pv_units->g_bar.h = .03f;  pv_units->g_bar.a = .09f;
    pv_units->dt.vm = .05f;
    pv_units->dt.vm_eq_cyc = 100; // go straight to equilibrium!

    pvi_cons->SetUnique("lmix", true);
    pvi_cons->lmix.err_sb = false; // key for linear mapping w/out overshoot!
    pvi_cons->SetUnique("rnd", true);
    pvi_cons->rnd.mean = 0.1f;
    pvi_cons->rnd.var = 0.0f;

    pvi_cons->lrate = .01f;
    lve_cons->lrate = .05f;
    lvi_cons->lrate = .001f;
  }
  else {			// GAUSSIAN
    pvesp->scalar.rep = ScalarValSpec::GAUSSIAN;
    pvisp->scalar.rep = ScalarValSpec::GAUSSIAN;
    lvesp->scalar.rep = ScalarValSpec::GAUSSIAN;

    lvesp->bias_val.un = ScalarValBias::GC;
    lvesp->bias_val.wt = ScalarValBias::NO_WT;
    lvesp->bias_val.val = 0.0f;
    pvisp->bias_val.un = ScalarValBias::GC;
    pvisp->bias_val.wt = ScalarValBias::NO_WT;
    pvisp->bias_val.val = 0.5f;

    pvesp->compute_i = LeabraLayerSpec::KWTA_INHIB; pvesp->i_kwta_pt = 0.25f;
    pvisp->compute_i = LeabraLayerSpec::KWTA_INHIB; pvisp->i_kwta_pt = 0.25f;
    lvesp->compute_i = LeabraLayerSpec::KWTA_INHIB; lvesp->i_kwta_pt = 0.25f;

    pvesp->kwta.k = 3; 	pvesp->gp_kwta.k = 3; 
    pvisp->kwta.k = 3; 	pvisp->gp_kwta.k = 3; 
    lvesp->kwta.k = 3; 	lvesp->gp_kwta.k = 3; 

    pvesp->unit_range.min = -0.50f;  pvesp->unit_range.max = 1.5f;
    pvesp->unit_range.UpdateAfterEdit();

    pvisp->unit_range.min = -0.5f;  pvisp->unit_range.max = 1.5f;
    pvisp->unit_range.UpdateAfterEdit();

    lvesp->unit_range.min = -0.5f;  lvesp->unit_range.max = 1.5f;
    lvesp->unit_range.UpdateAfterEdit();

    pv_units->SetUnique("act", true);
    pv_units->SetUnique("act_fun", true);
    pv_units->SetUnique("dt", true);
    pv_units->act_fun = LeabraUnitSpec::NOISY_XX1;
    pv_units->act.thr = .25f;
    pv_units->act.gain = 600.0f;
    pv_units->act.nvar = .005f;
    pv_units->g_bar.l = .1f;
    pv_units->g_bar.h = .015f;  pv_units->g_bar.a = .045f;
    pv_units->dt.vm = .2f;

    pvi_cons->SetUnique("lmix", true);
    pvi_cons->lmix.err_sb = true; // key for linear mapping w/out overshoot!
    pvi_cons->SetUnique("rnd", true);
    pvi_cons->rnd.mean = 0.1f;
    pvi_cons->rnd.var = 0.0f;

    pvi_cons->lrate = .01f;
    lve_cons->lrate = .05f;
    lvi_cons->lrate = .001f;
  }

  if(output_lays.size > 0)
    pvesp->rew_type = ExtRewLayerSpec::OUT_ERR_REW;
  else
    pvesp->rew_type = ExtRewLayerSpec::EXT_REW;

  int n_lv_u;		// number of lvpv-type units
  if(pvisp->scalar.rep == ScalarValSpec::LOCALIST)
    n_lv_u = 4;
  else if(pvisp->scalar.rep == ScalarValSpec::GAUSSIAN)
    n_lv_u = 12;
  else
    n_lv_u = 21;

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
  // set positions & geometries

  if(lve_new) {
    pve->pos.z = 0; pve->pos.y = 6; pve->pos.x = 0;
    vta->pos.z = 0; vta->pos.y = 1; vta->pos.x = n_lv_u + 2;
    pvi->pos.z = 0; pvi->pos.y = 4; pvi->pos.x = 0;
    lvi->pos.z = 0; lvi->pos.y = 2; lvi->pos.x = 0;
    lve->pos.z = 0; lve->pos.y = 0; lve->pos.x = 0;
  }

  if(pvi->n_units != n_lv_u) { pvi->n_units = n_lv_u; pvi->geom.x = n_lv_u; pvi->geom.y = 1; }
  if(lve->n_units != n_lv_u) { lve->n_units = n_lv_u; lve->geom.x = n_lv_u; lve->geom.y = 1; }
  if(lvi->n_units != n_lv_u) { lvi->n_units = n_lv_u; lvi->geom.x = n_lv_u; lvi->geom.y = 1; }
  if(pve->n_units != n_lv_u) { pve->n_units = n_lv_u; pve->geom.x = n_lv_u; pve->geom.y = 1; }
  vta->n_units = 1;
  rew_targ_lay->n_units = 1;

  //////////////////////////////////////////////////////////////////////////////////
  // apply specs to objects

  pve->SetLayerSpec(pvesp);	pve->SetUnitSpec(pv_units);
  pvi->SetLayerSpec(pvisp);	pvi->SetUnitSpec(pv_units);
  lve->SetLayerSpec(lvesp);	lve->SetUnitSpec(lv_units);
  lvi->SetLayerSpec(lvisp);	lvi->SetUnitSpec(lv_units);
  vta->SetLayerSpec(dasp);	vta->SetUnitSpec(da_units);

  pv_units->bias_spec.SetSpec(bg_bias);
  lv_units->bias_spec.SetSpec(bg_bias);
  da_units->bias_spec.SetSpec(fixed_bias);
  
  //////////////////////////////////////////////////////////////////////////////////
  // make projections

  // FindMakePrjn(Layer* recv, Layer* send,
  net->FindMakePrjn(pve, rew_targ_lay, onetoone, marker_cons);
  net->FindMakePrjn(pvi, pve, onetoone, marker_cons);
  net->FindMakePrjn(lve, pvi, onetoone, marker_cons);
  net->FindMakePrjn(lvi, pvi, onetoone, marker_cons);

  net->FindMakePrjn(vta, pvi, onetoone, marker_cons);
  net->FindMakePrjn(vta, lve, onetoone, marker_cons);
  net->FindMakePrjn(vta, lvi, onetoone, marker_cons);

  net->FindMakePrjn(pvi,  vta, onetoone, marker_cons);
  net->FindMakePrjn(lve,  vta, onetoone, marker_cons);
  net->FindMakePrjn(lvi,  vta, onetoone, marker_cons);

  if(lve_new || fm_hid_cons || fm_out_cons) {
    for(i=0;i<input_lays.size;i++) {
      Layer* il = (Layer*)input_lays[i];
      net->FindMakePrjn(pvi, il, fullprjn, pvi_cons);
      net->FindMakePrjn(lve, il, fullprjn, lve_cons);
      net->FindMakePrjn(lvi, il, fullprjn, lvi_cons);
    }
    if(fm_hid_cons) {
      for(i=0;i<hidden_lays.size;i++) {
	Layer* hl = (Layer*)hidden_lays[i];
	net->FindMakePrjn(pvi, hl, fullprjn, pvi_cons);
	net->FindMakePrjn(lve, hl, fullprjn, lve_cons);
	net->FindMakePrjn(lvi, hl, fullprjn, lvi_cons);
      }
    }
    if(fm_out_cons) {
      for(i=0;i<output_lays.size;i++) {
	Layer* ol = (Layer*)output_lays[i];
	net->FindMakePrjn(pvi, ol, fullprjn, pvi_cons);
	net->FindMakePrjn(lve, ol, fullprjn, lve_cons);
	net->FindMakePrjn(lvi, ol, fullprjn, lvi_cons);
      }
    }
  }
  if(da_mod_all) {
    for(i=0;i<other_lays.size;i++) {
      Layer* ol = (Layer*)other_lays[i];
      net->FindMakePrjn(ol, vta, fullprjn, marker_cons);
    }
  }

  for(i=0;i<output_lays.size;i++) {
    Layer* ol = (Layer*)output_lays[i];
    net->FindMakePrjn(pve, ol, onetoone, marker_cons);
  }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  net->Build();
  net->Connect();

  bool ok = pvisp->CheckConfig(pvi, net, true) && lvesp->CheckConfig(lve, net, true)
    && lvisp->CheckConfig(lve, net, true)
    && dasp->CheckConfig(vta, net, true) && pvesp->CheckConfig(pve, net, true);

  if(!ok) {
    msg =
      "PVLV: An error in the configuration has occurred (it should be the last message\
 you received prior to this one).  The network will not run until this is fixed.\
 In addition, the configuration process may not be complete, so you should run this\
 function again after you have corrected the source of the error.";
  }
  else {
    msg = 
    "PVLV configuration is now complete.  Do not forget the one remaining thing\
 you need to do manually:\n\n" + man_msg;
  }
  taMisc::Choice(msg,"Ok");

  pvesp->UpdateAfterEdit();
  pvisp->UpdateAfterEdit();
  lvesp->UpdateAfterEdit();
  lvisp->UpdateAfterEdit();
  
  for(j=0;j<net->specs.leaves;j++) {
    BaseSpec* sp = (BaseSpec*)net->specs.Leaf(j);
    sp->UpdateAfterEdit();
  }

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);
//   winbMisc::DelayedMenuUpdate(proj);

  //////////////////////////////////////////////////////////////////////////////////
  // select edit

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = pdpMisc::FindSelectEdit(proj);
  if(edit != NULL) {
    pvi_cons->SelectForEditNm("lrate", edit, "pvi");
    lve_cons->SelectForEditNm("lrate", edit, "lve");
    lvi_cons->SelectForEditNm("lrate", edit, "lvi");
    //    lve_cons->SelectForEditNm("syn_dep", edit, "lve");
    pvesp->SelectForEditNm("rew", edit, "pve");
//     lvesp->SelectForEditNm("lv", edit, "lve");
    pvisp->SelectForEditNm("pv_detect", edit, "pvi");
//     pvisp->SelectForEditNm("scalar", edit, "pvi");
//     lvesp->SelectForEditNm("scalar", edit, "lve");
//     pvisp->SelectForEditNm("bias_val", edit, "pvi");
//     lvesp->SelectForEditNm("bias_val", edit, "lve");
//    dasp->SelectForEditNm("avg_da", edit, "vta");
    dasp->SelectForEditNm("da", edit, "vta");
  }
}

///////////////////////////////////////////////////////////////
//			BgPFC
///////////////////////////////////////////////////////////////

static void set_n_stripes(LeabraNetwork* net, char* nm, int n_stripes, int n_units, bool sp) {
  LeabraLayer* lay = (LeabraLayer*)net->FindLayer(nm);
  if(lay == NULL) return;
  lay->geom.z = n_stripes;
  if(n_units > 0) lay->n_units = n_units;
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
  if(lay->n_units == 0) {
    lay->geom.x = 1; lay->geom.y = 1; lay->n_units = 1;
  }
  lay->geom.z = half_stripes * 2;
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

  snc->n_units = 1;
  if(snc_new) { 
    snc->pos.z = 0; snc->pos.y = 3; snc->pos.x = vta->pos.x; 
  }
  lay_set_geom(snc, half_stripes);

  if(pfc_m_new) {
    pfc_m->pos.z = 2; pfc_m->pos.y = 0; pfc_m->pos.x = mx_z2 + 1;
    if(nolrn_pfc && (input_lays.size > 0)) {
      Layer* il = (Layer*)input_lays[0];
      pfc_m->n_units = il->n_units; pfc_m->geom.x = il->geom.x; pfc_m->geom.y = il->geom.y;
    }
    else {
      pfc_m->n_units = 30; pfc_m->geom.x = 5; pfc_m->geom.y = 6;
    }
  }
  lay_set_geom(pfc_m, half_stripes);

  if(matrix_m_new) { 
    matrix_m->pos.z = 1; matrix_m->pos.y = 0; matrix_m->pos.x = mx_z1 + 1; 
    matrix_m->n_units = 28; matrix_m->geom.x = 4; matrix_m->geom.y = 7;
  }
  lay_set_geom(matrix_m, half_stripes);

  snrthal_m->n_units = 1;
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
	pfc_o->n_units = il->n_units; pfc_o->geom.x = il->geom.x; pfc_o->geom.y = il->geom.y;
      }
      else {
	pfc_o->n_units = pfc_m->n_units; pfc_o->geom = pfc_m->geom;
      }
    }
    lay_set_geom(pfc_o, half_stripes);

    if(matrix_o_new) { 
      matrix_o->pos.z = matrix_m->pos.z; matrix_o->pos.y = matrix_m->pos.y;
      matrix_o->pos.x = matrix_m->pos.x + matrix_m->act_geom.x + 2; 
      matrix_o->n_units = 28; matrix_o->geom.x = 4; matrix_o->geom.y = 7;
    }
    lay_set_geom(matrix_o, half_stripes);

    snrthal_o->n_units = 1;
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
//       patch->n_units = 20; patch->geom.x = 20; patch->geom.y = 1;
//       patch->gp_geom.x = 1; patch->gp_geom.y = n_stripes;
//       patch->UpdateAfterEdit();
//     }
//     lay_set_geom(patch, half_stripes);
//     patch->gp_geom.x = 1; patch->gp_geom.y = n_stripes;
//   }

  //////////////////////////////////////////////////////////////////////////////////
  // build and check

  SetPFCStripes(net, n_stripes);

  bool ok = pfcmsp->CheckConfig(pfc_m, net, true) && matrixsp->CheckConfig(matrix_m, net, true)
    && snrthalsp->CheckConfig(snrthal_m, net, true) && sncsp->CheckConfig(snc, net, true);

  if(ok && out_gate) {
    ok = pfcosp->CheckConfig(pfc_o, net, true) && matrixosp->CheckConfig(matrix_o, net, true)
      && snrthalosp->CheckConfig(snrthal_o, net, true);
  }

//   if(ok && make_patch) ok = patchsp->CheckConfig(patch, net, true);

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


